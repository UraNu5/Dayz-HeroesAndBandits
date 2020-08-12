class HeroesAndBanditsZone
{
	int Index; //0 top level zones, 1 sub zone, 2 sub zone of a sub zone etc.
	int ZoneID;
    string Name;
	float X;
	float Z;
	float Radius;
	bool ShowWarningMsg;
	string WarningMessage;
	bool ShowWelcomeMsg;
	string WelcomeMessage;
	string WelcomeIcon;
	int WelcomeMessageColor;
	float KillRadius;
    float MinHumanity;
    float MaxHumanity;
	bool OverrideSafeZone;
	bool GodModPlayers;
	bool PreventWeaponRaise;
	bool PreventActions;
	bool BlockTrader;
	bool KillAggressors;
	float MaxDistance = 999;
	ref array< ref habZoneAffinity > Affinities = new ref array< ref habZoneAffinity >;
	ref array< ref HeroesAndBanditsGuard > Guards = new ref array< ref HeroesAndBanditsGuard >;
	ref array< ref HeroesAndBanditsZone > SubZones = new ref array< ref HeroesAndBanditsZone >;

	void Init(habZone zoneToLoad, int zoneID, int index = 0){
		
		Index = index;
		ZoneID = zoneID;
		Name = zoneToLoad.Name;
		X = zoneToLoad.X;
		Z = zoneToLoad.Z;
		MinHumanity = zoneToLoad.MinHumanity;
		MaxHumanity = zoneToLoad.MaxHumanity;
		Radius = zoneToLoad.WarningRadius;
		KillRadius = zoneToLoad.KillRadius;
		WelcomeMessageColor = zoneToLoad.getWelcomeMessageColor();
		ShowWelcomeMsg = zoneToLoad.ShowWelcomeMsg;
		WelcomeMessage = zoneToLoad.WelcomeMessage;
		WelcomeIcon = zoneToLoad.WelcomeIcon;
		ShowWarningMsg = zoneToLoad.ShowWarningMsg;
		WarningMessage = zoneToLoad.WarningMessage;
		OverrideSafeZone = zoneToLoad.OverrideSafeZone;
		GodModPlayers = zoneToLoad.GodModPlayers;
		PreventWeaponRaise = zoneToLoad.PreventWeaponRaise;
		PreventActions = zoneToLoad.PreventActions;
		Affinities = zoneToLoad.Affinities;
		BlockTrader = zoneToLoad.BlockTrader;
		KillAggressors = zoneToLoad.KillAggressors;
		habPrint("[Zone] " + Name + " at X: "+ X + " Z:" + Z + " Index: " + Index + " ZoneID: "+ ZoneID + " PreventActions: " + PreventActions,"Debug");
		if (zoneToLoad.Guards){
			for ( int j = 0; j < zoneToLoad.Guards.Count(); j++ )
			{	
				float guardX = zoneToLoad.Guards.Get(j).X;
				float guardY = zoneToLoad.Guards.Get(j).Y; 
				float guardZ = zoneToLoad.Guards.Get(j).Z; 
				float orientation = zoneToLoad.Guards.Get(j).Orientation; 
				string skin = zoneToLoad.Guards.Get(j).Skin; 
				string weaponInHands = zoneToLoad.Guards.Get(j).WeaponInHands; 
				string weaponInHandsMag = zoneToLoad.Guards.Get(j).WeaponInHandsMag;
				TStringArray weaponInHandsAttachments = zoneToLoad.Guards.Get(j).WeaponInHandsAttachments; 
				TStringArray guardGear = zoneToLoad.Guards.Get(j).GuardGear;
				Guards.Insert(new ref HeroesAndBanditsGuard(guardX, guardY, guardZ, orientation, skin, weaponInHands, weaponInHandsMag, weaponInHandsAttachments, guardGear));
				Guards.Get(j).GunSound = zoneToLoad.Guards.Get(j).GunSound;
				Guards.Get(j).DamagePerTickMin = zoneToLoad.Guards.Get(j).DamagePerTickMin;
				Guards.Get(j).DamagePerTickRand = zoneToLoad.Guards.Get(j).DamagePerTickRand;
				Guards.Get(j).GunTickMulitplier  = zoneToLoad.Guards.Get(j).GunTickMulitplier;
				Guards.Get(j).HitChance  = zoneToLoad.Guards.Get(j).HitChance;
				Guards.Get(j).ZoneName = Name;
				Guards.Get(j).RespawnTimer = zoneToLoad.Guards.Get(j).RespawnTimer;
				Guards.Get(j).CanBeKilled = zoneToLoad.Guards.Get(j).CanBeKilled;
				Guards.Get(j).RequireLineOfSight = zoneToLoad.Guards.Get(j).RequireLineOfSight;
				Guards.Get(j).Spawn();
				//GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLaterByName(Guards.Get(j), "ReloadWeapon", 120 * 1000 , false); //Reload gun 2 minutes after server start
			}
		}
		if (zoneToLoad.SubZones) //Check if any Sub Zones are defined before loading
		{
			for ( int i = 0; i < zoneToLoad.SubZones.Count(); i++ )
			{
		    	habPrint("Loading Sub Zone " + zoneToLoad.SubZones.Get(i).Name , "Verbose");
				string name = zoneToLoad.SubZones.Get(i).Name;
				int x = zoneToLoad.SubZones.Get(i).X;
				int z = zoneToLoad.SubZones.Get(i).Z;
				SubZones.Insert(new ref HeroesAndBanditsZone(name, x, z));
				SubZones.Get(i).Init(zoneToLoad.SubZones.Get(i), i, Index + 1);
			}
		}
		
	}
	
	//Returns True if player left the zone
	bool CheckPlayer(PlayerBase inPlayer, bool allowSpinOff = true){
		bool PlayerLeftZone = false;
		bool PlayerLeftSubZone = false;
		PlayerBase player = PlayerBase.Cast(inPlayer);
		HeroesAndBanditsPlayer pdata = GetHeroesAndBandits().GetPlayer(player.GetIdentity().GetPlainId());
		float trackingBonus = GetHeroesAndBanditsZones().ZoneCheckTimer / 2;
		ref HeroesAndBanditsGuard guard;
		if ( !player ){
			return PlayerLeftZone;
		}
		
		if (player.IsAlive()){
			
			if (!validPlayer(pdata)){
				guard = GetClosestGuard(player);
				if (guard){
					habPrint("Guard is not null", "Debug");
				} else {
					habPrint("GUARD IS NULL !!!!!!!!!!!!!!!!!!!!!!!!!!! BUT WHY", "Debug");
				}
			}
			/*habPrint("Checking if Player: " + player.GetIdentity().GetName() + " ("+player.GetIdentity().GetPlainId()+") is in Zone " + Name, "Debug");	
			if ( GetHeroesAndBanditsSettings().DebugLogs ){
				player.m_HeroesAndBandits_InZones.Debug();
			}*/
			if (validPlayer(pdata) && vector.Distance(player.GetPosition(), getVector()) <= Radius && !player.habIsInZone(ZoneID, Index)){
				habPrint("Player: " + player.GetIdentity().GetName() + " ("+player.GetIdentity().GetPlainId()+") Entered: " + Name, "Verbose");
				player.habEnteredZone(ZoneID, Index);
				if ( GetHeroesAndBanditsSettings().DebugLogs ){
					player.m_HeroesAndBandits_InZones.Debug();
				}
				if ( ShowWelcomeMsg )
				{
					GetHeroesAndBandits().WelcomePlayer(Name, WelcomeMessage, WelcomeIcon, player.GetIdentity().GetPlainId(), WelcomeMessageColor);
				}
				if ( GodModPlayers && validPlayer(pdata) && !player.habCheckGodMod() )
				{
					player.SetAllowDamage(false);
				}
				if (PreventWeaponRaise && !player.habCanRaiseWeapon() &&  validPlayer(pdata)){
					player.habSetCanRaiseWeapon(false, Index);
				}
			}
			else if (!validPlayer(pdata) && player.habIsInZone(ZoneID, Index) && vector.Distance(player.GetPosition(), getVector()) <= KillRadius && KillRadius != 0)
			{
				habPrint("Player: " + player.GetIdentity().GetName() + " ("+player.GetIdentity().GetPlainId()+") Entered: " + Name + " Kill Radius trying to kill them", "Debug");
				if ( OverrideSafeZone && !player.habCheckGodMod() )
				{
					player.SetAllowDamage(true);
				}
				if (!player.habCheckGodMod() && guard){
					habPrint("Player: " + player.GetIdentity().GetName() + " ("+player.GetIdentity().GetPlainId()+") Doesn't have god mod and is has a guard assigned", "Debug");
					if (guard.IsReadyToTrack()){
						habPrint("Tracking: " + player.GetIdentity().GetName() + " ("+player.GetIdentity().GetPlainId()+")", "Debug");
						guard.TrackPlayer(player, trackingBonus);
					}
					//guard.SetDirection(vector.Direction(guard.GetPosition(),player.GetPosition()).Normalized());
					guard.FireWeapon(player);
					if ( guard.GunTickMulitplier > 2 && allowSpinOff){
						float maxDelayGun = GetHeroesAndBanditsZones().ZoneCheckTimer * 1000;
						float delayadd = maxDelayGun / guard.GunTickMulitplier;
						float delay = delayadd;
						while( delay < maxDelayGun){
							GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater(this.CheckPlayer, delay, false, player, false);
							delay = delay + delayadd;
						}
					}
				} else if (!Guards || Guards.Count() == 0){
					player.habSetKilledByZone(Name);
					player.SetHealth(0.0);
				} else if ( allowSpinOff ){
				
					GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater(this.CheckPlayer, trackingBonus * 1000, false, player, false);
				}
				//Player Entered Kill Zone Kill Player if warning has been given
				if (!player.IsAlive()){
					player.habLeftZone(0); //Removed from all zones
					habPrint("Killed Player: " + player.GetIdentity().GetName() + " ("+player.GetIdentity().GetPlainId()+") for Entering Zone: " + Name, "Always");
					if ( PreventWeaponRaise ){
						player.habSetCanRaiseWeapon(true, -1);
					}
				}
				if ( GetHeroesAndBanditsSettings().DebugLogs ){
					player.m_HeroesAndBandits_InZones.Debug();
				}

				
			}
			else if (!validPlayer(pdata) && !player.habIsInZone(ZoneID, Index) && vector.Distance(player.GetPosition(), getVector()) <= Radius)
			{
				habPrint("Player: " + player.GetIdentity().GetName() + " ("+player.GetIdentity().GetPlainId()+") Entered: " + Name + " Warning Radius sending warning", "Verbose");
				//Player Entered Warning Zone Issue Warning
				player.habEnteredZone(ZoneID, Index);
				if ( GetHeroesAndBanditsSettings().DebugLogs ){
					player.m_HeroesAndBandits_InZones.Debug();
				}
				habPrint("Player: " + player.GetIdentity().GetName() + " ("+player.GetIdentity().GetPlainId()+") Entered: " + Name, "Verbose");
				if ( ShowWarningMsg )
				{
					GetHeroesAndBandits().WarnPlayer(Name, WarningMessage, player.GetIdentity().GetPlainId());
				}
				if ( BlockTrader )
				{
					player.habSetTraderBlocked(true, -1);
				}
				if ( PreventWeaponRaise )
				{
					player.habSetCanRaiseWeapon(true, -1);
				}
				if (guard && allowSpinOff){
					GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater(this.CheckPlayer, trackingBonus * 1000, false, player, false);
				}
				if (guard && guard.IsReadyToTrack()){
					habPrint("Tracking: " + player.GetIdentity().GetName() + " ("+player.GetIdentity().GetPlainId()+")", "Debug");
					guard.TrackPlayer(player, trackingBonus);
				}
			}
			else if (vector.Distance(player.GetPosition(), getVector()) > Radius && player.habIsInZone(ZoneID, Index))
			{
				if ( GodModPlayers  && !player.habCheckGodMod() )
				{
					player.SetAllowDamage(true);
				}
				//Player Left Warning Radius
				habPrint("Player: " + player.GetIdentity().GetName() + " ("+player.GetIdentity().GetPlainId()+") Left: " + Name, "Verbose");
				if (!validPlayer(pdata) && guard ){
					guard.UnTrackPlayer( player);
				}
				player.habLeftZone(Index);		
				if ( GetHeroesAndBanditsSettings().DebugLogs ){
					player.m_HeroesAndBandits_InZones.Debug();
				}
				PlayerLeftZone = true;
				if (!validPlayer(pdata) && BlockTrader )
				{
					player.habSetTraderBlocked(false, Index);
				}
			}
			if ( SubZones && player.habIsInZone(ZoneID, Index) )
			{ 
				for (int subI = 0; subI < SubZones.Count(); subI++)
				{
					if (SubZones.Get(subI).CheckPlayer(player)){
						PlayerLeftSubZone = true;
					}
				}
			}
		}
		if (PlayerLeftSubZone){ //Left Subzone make sure all the required restrictions are renabled
		
			if ( validPlayer(pdata) && GodModPlayers  && !player.habCheckGodMod() )
			{
				player.SetAllowDamage(true);
			}
			if (PreventWeaponRaise && !player.habCanRaiseWeapon()){
				player.habSetCanRaiseWeapon(false, Index);
			}
			if (!validPlayer(pdata) && BlockTrader )
			{
				player.habSetTraderBlocked(true, Index);
			}		
		}
		return PlayerLeftZone;
	}
	
    void HeroesAndBanditsZone(string name, float x, float z) 
	{
		Name = name;
		X = x;
		Z = z;
    }
	
	vector getVector(){
		return Vector( X, GetGame().SurfaceY(X, Z), Z );
	}
	
	bool validHumanity(float humanity){
			if ( MinHumanity != -1 && MaxHumanity != -1 && humanity >= MinHumanity && humanity <= MaxHumanity){
				return true;
			}else if (MinHumanity == -1 && MaxHumanity == -1){
				return true;
			}else if (MinHumanity == -1 && MaxHumanity != -1 && humanity <= MaxHumanity){
				return true;
			}else if (MinHumanity != -1 && MaxHumanity == -1 && humanity >= MinHumanity){
				return true;
			}
		return false;
	}
	

	
	bool validPlayer(HeroesAndBanditsPlayer pdata){
		
		if (GetHeroesAndBanditsSettings().Mode != 1 && Affinities.Count() > 0){
			string affinity = pdata.getAffinity().Name;
			for (int i = 0; i < Affinities.Count(); i++){
				if (Affinities.Get(i).Check(pdata.getAffinityPoints(affinity) , affinity))
				{
					return true;
				}
			}
		} 
		if (GetHeroesAndBanditsSettings().Mode != 0 && Affinities.Count() > 0){
			bool DefaultAffinty = false;
			for (int j = 0; j < pdata.Affinities.Count(); j++){
				if ( ( pdata.Affinities.Get(j).Name == "bandit" || pdata.Affinities.Get(j).Name == "hero" ) && GetHeroesAndBanditsSettings().Mode == 2 ){
					//Skipping for Hero and Bandit as it was checked above if in mixed mode
				} else {
					for (int k = 0; k < Affinities.Count(); k++){
						if ( Affinities.Get(i).Check(pdata.Affinities.Get(j).Points,  pdata.Affinities.Get(j).Name)){
							return true;
						} else if (Affinities.Get(i).Affinity == GetHeroesAndBanditsLevels().DefaultAffinity.Name){
							DefaultAffinty = true;
						}
					}
				}
			}
			if (DefaultAffinty && pdata.getAffinity() == GetHeroesAndBanditsLevels().DefaultAffinity)
			{
				return true;
			}
		}
		return false;
	} 
		
	ref HeroesAndBanditsGuard GetClosestGuard(PlayerBase inPlayer)
	{ 
		PlayerBase player = PlayerBase.Cast(inPlayer);
		if (!Guards || !player)//If no guards defined exit
		{
			return NULL;
		}
		vector playerPostion = player.GetPosition();
		int closestGuardIndex = -1;
		float closestGuardDistance = MaxDistance;
		for ( int i = 0; i < Guards.Count(); i++ )
		{	
			float currentGuardDistance = vector.Distance( Guards.Get(i).getVector(), playerPostion);
			bool lineOfSight = (Guards.Get(i).HasLineOfSight(player) > 0 || !Guards.Get(i).RequireLineOfSight);
			if (lineOfSight)
			{
				habPrint("Guard has line of sight", "Debug");
			}
			if (Guards.Get(i).IsAlive() && Guards.Get(i).GetClosetPlayerID() == player.GetIdentity().GetPlainId() && currentGuardDistance < MaxDistance && lineOfSight){
				
				habPrint("Guard is already tracking player", "Debug");
				return Guards.Get(i);
			}
			if ( Guards.Get(i).IsAlive() && lineOfSight && currentGuardDistance < closestGuardDistance) {
				closestGuardIndex = i;
				closestGuardDistance = currentGuardDistance;
			}
		}
		if ( closestGuardIndex == -1 ){
			habPrint("No close enough Guard", "Debug");
			return NULL;
		} else {
			habPrint("Getting Guard at Index: " + closestGuardIndex, "Debug");
			habPrint("Getting Guard at Zone: " + Guards.Get(closestGuardIndex).ZoneName, "Debug");
			return Guards.Get(closestGuardIndex);
		} 
	}
	
	HeroesAndBanditsZone GetChild( array< int > inZones){
		int maxIndex = inZones.Count() - 1;
		if (maxIndex == Index){
			return this;
		} else {
			return SubZones.Get(inZones.Get(Index + 1)).GetChild( inZones );
		}
	}
}