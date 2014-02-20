#if defined FOR_FUCK_SAKE_L_SAM
PLEASE DEFINE FILTERSCRIPT IF YOU ARE USING IT AS A FILTERSCRIPT
I've spent too much time for forgetting that define, I thought my include was
fucked up. But nope, you must define it to use it as a filterscript!
#endif

/*______________________________________________________________________________

					L-SAM "Lordz's Surface To Air Missiles" - v1.3
					Copyright(c) 2014 - L_SAM3.inc
					Author : Lordzâ„¢
					Version : 1.3
					Initial v1 Released Date : 04-01-2014
					Thread : http://forum.sa-mp.com/showthread.php?t=484765
					Thanks to the person who created randomEx,
					Thanks to RyDeR` for SetObjectFaceCoords3D function,
 										&
		   			All the players of YoUnG Generations for Testing this!

* Happy New Year!

//Functions:

native CreateSAM(Float:X, Float:Y, Float:Z, Float:rX=0.0, Float:rY=0.0, Float:rZ=0.0, Float:DrawDistance=200.0, Float:Range=300.0, Speed=80, Power=SAM_POWER_NORMAL, bool:follow=false, bool:followlimit=false);
native IsSAMMissilesFollowing(samid);
native DestroySAM(samid);
native AvoidSAMAttacksOnPlayer(playerid, samid);
native EnableSAMAttacksOnPlayer(playerid, samid);
native ToggleNoSAMAttacksOnPlayer(playerid, samid, bool:noattack);
native SetSAMMissileSpeed(samid, speed);
native SetSAMMissilePower(samid, Power);
native SetSAMMissileColor(samid, color);
native GetSAMMissileSpeed(samid);
native GetSAMMissilePower(samid);
native GetSAMObjectID(samid);
native GetSAMMissileColor(samid);
native IsMissileFollowingLimited(samid);
native DestroySAMMissile(playerid);
native IsSAMMissileChasing(playerid);

//Callbacks:
OnSAMMissileHitPlayer(playerid, samid, power);

ChangeLogs:
v1.0 (01/01/2014)
Initial release.

v1.2 (02/01/2014)
+ The following missile system has been done! Parameter is at the end of CreateSAM.
  Set it to true to allow the SAM to target lock players.
+ Added a function called IsSAMMissilesFollowing which returns true in case if SAM
  got following missiles, false if not.
+ Missiles can now have colors specified on radar. It's per-sam colors.
+ Added two functions called GetSAMMissileColor and SetSAMMissileColor.

v2 <B>

+ Fixed the tag mismatch problems in AvoidSAMAttacksOnPlayer and on
  EnableSAMAttacksOnPlayer.

v1.3
+ This include no longer uses Streamer as it's use is very less for further
  things coming. So, it doesn't use Streamer function anymore.
+ The missiles now set it's facing position according to the player's position.
+ Added smoke effects on missiles. You can disable it by removing :
  #define L_SAMUSE_SMOKES from the include.
+ Added callback called "OnSAMMissileLaunch" which is called when a SAM
  launches a missile on player.
+ Added callback called "OnSAMMissileHitPlayer" which is called when a SAM
  missile hits the player. (Player must be connected)
+ Added a new parameter called "bool:followlimit" on CreateSAM. It creates
  limitations for the following range according to the area range specified.
  It's set to false as default so it follows the player everywhere until it
  hits the player, if not set to true.
+ Added function called "IsMissileFollowingLimited" which returns true in case
  if the following missiles got range limit, else false.
+ Added function called "DestroySAMMissile" which destroys the missile if player
  is being chased by any missiles.
+ Added function called "IsSAMMissileChasing" which return true in case if the
  player is being chased by a missile. False if not.
_______________________________________________________________________________*/

//#define FILTERSCRIPT

#define L_SAMUSE_SMOKES

#if defined _included_l_sam
 #endinput
#endif

#define _included_l_sam

#include <a_samp>



#define MAX_LSAMS 100 //Change this value to create more SAM launchers.

#define SAM_POWER_LOW    0
#define SAM_POWER_NORMAL 1
#define SAM_POWER_HIGH   2
#define SAM_POWER_BEST 3

#define MISSILE_RADAR_COLOR 0x660000FF



enum L_saminfo
{
 	Float:L_SAMX,
 	Float:L_SAMY,
 	Float:L_SAMZ,
 	Float:L_SAMrX,
 	Float:L_SAMrY,
 	Float:L_SAMrZ,
 	Float:L_SAMDrawDistance,
 	Float:L_SAMRange,
 	L_SAMSpeed,
 	L_SAMPower,
 	bool:L_SAMFollow,
 	bool:L_SAMFollowLimit,
 	Float:L_SAMFollowLimitRange,
 	bool:L_SAMCreated,
 	L_SAMObjectID,
 	L_SAMMissileColor,
}

new L_SAMData[MAX_LSAMS][L_saminfo],
	L_CountSAMS = 0,
	Float:L_SAMPlayerPos[MAX_PLAYERS][3],
	PlayerMissile[MAX_PLAYERS],
#if defined L_SAMUSE_SMOKES
	PlayerMissileSmoke[MAX_PLAYERS],
#endif
	bool:MissileInProgress[MAX_PLAYERS],
	L_SAMMissileObject[MAX_OBJECTS],
#if defined L_SAMUSE_SMOKES
	L_SAMMissileSmoke[MAX_OBJECTS],
#endif
	L_SAMMissilePower[MAX_OBJECTS],
	L_SAMMissileSAMID[MAX_OBJECTS],
	L_SAMMissileProgress[MAX_OBJECTS],
	L_SAMMissileMarker[MAX_PLAYERS],
	bool:L_SAMFollowStopped[MAX_PLAYERS],
	bool:L_SAM_ExceptionalPlayer[MAX_PLAYERS][MAX_LSAMS],
	L_SAMAssignedSpeed[MAX_OBJECTS],
	bool:L_SAMMissileStopped[MAX_OBJECTS];

stock CreateSAM(Float:X, Float:Y, Float:Z, Float:rX=0.0, Float:rY=0.0, Float:rZ=0.0, Float:DrawDistance=200.0, Float:Range=300.0, Speed=80, Power=SAM_POWER_NORMAL, bool:follow=false, bool:followlimit=false)
{
 	if(L_CountSAMS > MAX_LSAMS) return printf("â€¢ <L_SAM> Error : Couldn't create SAM, maximum SAMS have already been done!"), 0;
 	new samid = L_CountSAMS;
 	L_SAMData[samid][L_SAMX] = X;
 	L_SAMData[samid][L_SAMY] = Y;
 	L_SAMData[samid][L_SAMZ] = Z;
 	L_SAMData[samid][L_SAMrX] = rX;
 	L_SAMData[samid][L_SAMrY] = rY;
 	L_SAMData[samid][L_SAMrZ] = rZ;
 	L_SAMData[samid][L_SAMDrawDistance] = DrawDistance;
 	L_SAMData[samid][L_SAMRange] = Range;
 	L_SAMData[samid][L_SAMSpeed] = Speed;
 	if(Power >= SAM_POWER_BEST)
	 L_SAMData[samid][L_SAMPower] = SAM_POWER_BEST;
 	else if(Power <= SAM_POWER_LOW)
	 L_SAMData[samid][L_SAMPower] = SAM_POWER_LOW;
 	else L_SAMData[samid][L_SAMPower] = Power;
 	L_SAMData[samid][L_SAMFollow] = follow;
 	L_SAMData[samid][L_SAMFollowLimit] = followlimit;
 	L_SAMData[samid][L_SAMFollowLimitRange] = Range;
 	L_SAMData[samid][L_SAMObjectID] = CreateObject(18848, X, Y, Z, rX, rY, rZ, DrawDistance);
 	L_SAMData[samid][L_SAMMissileColor] = MISSILE_RADAR_COLOR;
 	L_SAMData[samid][L_SAMCreated] = true;
 	L_CountSAMS++;
 	return samid;
}

stock DestroySAM(samid)
{
  	if(L_SAMData[samid][L_SAMCreated] == false) return 0;
  	L_SAMData[samid][L_SAMX] = 0.0;
	L_SAMData[samid][L_SAMY] = 0.0;
  	L_SAMData[samid][L_SAMZ] = 0.0;
  	L_SAMData[samid][L_SAMrX] = 0.0;
  	L_SAMData[samid][L_SAMrY] = 0.0;
  	L_SAMData[samid][L_SAMrZ] = 0.0;
  	L_SAMData[samid][L_SAMDrawDistance] = 0.0;
  	L_SAMData[samid][L_SAMRange] = 0.0;
  	L_SAMData[samid][L_SAMSpeed] = 0;
  	L_SAMData[samid][L_SAMPower] = 0;
  	L_SAMData[samid][L_SAMFollow] = false;
  	L_SAMData[samid][L_SAMCreated] = false;
  	DestroyObject(L_SAMData[samid][L_SAMObjectID]);
  	L_SAMData[samid][L_SAMObjectID] = -1;
  	L_SAMData[samid][L_SAMMissileColor] = MISSILE_RADAR_COLOR;
  	return 1;
}

stock IsMissileFollowingLimited(samid)
{
 	if(L_SAMData[samid][L_SAMCreated] == false) return -1;
 	if(L_SAMData[samid][L_SAMFollow] == false) return false;
 	if(L_SAMData[samid][L_SAMFollowLimit] == false) return false;
 	return true;
}

//Thanks to RyDeR` for this function!
stock L_SetObjectFaceCoords3D(iObject, Float: fX, Float: fY, Float: fZ, Float: fRollOffset = 0.0, Float: fPitchOffset = 0.0, Float: fYawOffset = 0.0) {
    new
        Float: fOX,
        Float: fOY,
        Float: fOZ,
        Float: fPitch
    ;
    GetObjectPos(iObject, fOX, fOY, fOZ);

    fPitch = floatsqroot(floatpower(fX - fOX, 2.0) + floatpower(fY - fOY, 2.0));
    fPitch = floatabs(atan2(fPitch, fZ - fOZ));

    fZ = atan2(fY - fOY, fX - fOX) - 90.0; // Yaw

    SetObjectRot(iObject, fRollOffset, fPitch + fPitchOffset, fZ + fYawOffset);
}


#if defined FILTERSCRIPT

public OnFilterScriptInit()
{
 	for(new samid; samid< MAX_LSAMS; samid++)
 	{
  		L_SAMData[samid][L_SAMX] = 0.0;
  		L_SAMData[samid][L_SAMY] = 0.0;
  		L_SAMData[samid][L_SAMZ] = 0.0;
  		L_SAMData[samid][L_SAMrX] = 0.0;
  		L_SAMData[samid][L_SAMrY] = 0.0;
  		L_SAMData[samid][L_SAMrZ] = 0.0;
  		L_SAMData[samid][L_SAMDrawDistance] = 0.0;
  		L_SAMData[samid][L_SAMRange] = 0.0;
  		L_SAMData[samid][L_SAMSpeed] = 0;
  		L_SAMData[samid][L_SAMPower] = 0;
  		L_SAMData[samid][L_SAMFollow] = false;
  		L_SAMData[samid][L_SAMCreated] = false;
  		L_SAMData[samid][L_SAMObjectID] = -1;
  		L_SAMData[samid][L_SAMMissileColor] = MISSILE_RADAR_COLOR;
  		for(new a; a< MAX_PLAYERS; a++) L_SAM_ExceptionalPlayer[a][samid] = false;
 	}
 	for(new i; i< MAX_OBJECTS; i++)
 	{
  		L_SAMMissileObject[i] = 0;
  		L_SAMMissilePower[i] = 0;
  		L_SAMMissileSAMID[i] = -1;
  		L_SAMMissileProgress[i] = -1;
 	}
 	for(new i; i< MAX_PLAYERS; i++)
 	{
		MissileInProgress[i] = false;
  		L_SAMPlayerPos[i][0] = 0.0;
  		L_SAMPlayerPos[i][1] = 0.0;
  		L_SAMPlayerPos[i][2] = 0.0;
  		PlayerMissile[i] = -1;
  		L_SAMFollowStopped[i] = false;
  #if defined L_SAMUSE_SMOKES
  		PlayerMissileSmoke[i] = -1;
  #endif
 	}
	L_CountSAMS = 0;
 	SetTimer("LordzSAMTimer", 2500, true);
 	CallLocalFunction("L_SAM_OnFS", "");
 	return 1;
}

#if defined _ALS_OnFilterScriptInit
 #undef OnFilterScriptInit
#else
 #define _ALS_OnFilterScriptInit
#endif

public OnFilterScriptExit()
{
 	for(new i; i< MAX_LSAMS; i++)
 	{
  		if(L_SAMData[i][L_SAMCreated])
  		{
   			DestroyObject(L_SAMData[i][L_SAMObjectID]);
  		}
 	}
 	for(new i; i< MAX_OBJECTS; i++)
 	{
  		if(L_SAMMissileProgress[i])
  		{
   			DestroyObject(i);
   #if defined L_SAMUSE_SMOKES
   			DestroyObject(L_SAMMissileSmoke[i]);
   #endif
  		}
 	}
	for(new i; i< MAX_PLAYERS; i++)
 	{
  		if(MissileInProgress[i]) RemovePlayerMapIcon(i, 99);
 	}
 	CallLocalFunction("L_SAM_OnFSExit", "");
 	return 1;
}

#if defined _ALS_OnFilterScriptExit
	#undef OnFilterScriptExit
#else
	#define _ALS_OnFilterScriptExit
#endif

forward L_SAM_OnFS();
forward L_SAM_OnFSExit();

#else

public OnGameModeInit()
{
 	for(new samid; samid< MAX_LSAMS; samid++)
 	{
  		L_SAMData[samid][L_SAMX] = 0.0;
  		L_SAMData[samid][L_SAMY] = 0.0;
  		L_SAMData[samid][L_SAMZ] = 0.0;
  		L_SAMData[samid][L_SAMrX] = 0.0;
  		L_SAMData[samid][L_SAMrY] = 0.0;
  		L_SAMData[samid][L_SAMrZ] = 0.0;
  		L_SAMData[samid][L_SAMDrawDistance] = 0.0;
  		L_SAMData[samid][L_SAMRange] = 0.0;
  		L_SAMData[samid][L_SAMSpeed] = 0;
  		L_SAMData[samid][L_SAMPower] = 0;
  		L_SAMData[samid][L_SAMFollow] = false;
  		L_SAMData[samid][L_SAMCreated] = false;
  		L_SAMData[samid][L_SAMObjectID] = -1;
  		L_SAMData[samid][L_SAMMissileColor] = MISSILE_RADAR_COLOR;
  		for(new a; a< MAX_PLAYERS; a++) L_SAM_ExceptionalPlayer[a][samid] = false;
 	}
 	for(new i; i< MAX_OBJECTS; i++)
 	{
  		L_SAMMissileObject[i] = 0;
  		L_SAMMissilePower[i] = 0;
  		L_SAMMissileSAMID[i] = -1;
  		L_SAMMissileProgress[i] = -1;
 	}
 	for(new i; i< MAX_PLAYERS; i++)
 	{
		MissileInProgress[i] = false;
  		L_SAMPlayerPos[i][0] = 0.0;
  		L_SAMPlayerPos[i][1] = 0.0;
  		L_SAMPlayerPos[i][2] = 0.0;
  		PlayerMissile[i] = -1;
  		L_SAMFollowStopped[i] = false;
  #if defined L_SAMUSE_SMOKES
  		PlayerMissileSmoke[i] = -1;
  #endif
 	}
	L_CountSAMS = 0;
 	SetTimer("LordzSAMTimer", 2500, true);
 	CallLocalFunction("L_SAM_OnGM", "");
 	return 1;
}

#if defined _ALS_OnGameModeInit
	#undef OnGameModeInit
#else
	#define _ALS_OnGameModeInit
#endif

forward L_SAM_OnGM();
public OnGameModeExit()
{
 	for(new i; i< MAX_LSAMS; i++)
 	{
  		if(L_SAMData[i][L_SAMCreated])
  		{
   			DestroyObject(L_SAMData[i][L_SAMObjectID]);
  		}
 	}
 	for(new i; i< MAX_OBJECTS; i++)
 	{
  		if(L_SAMMissileProgress[i])
  		{
   			DestroyObject(i);
   #if defined L_SAMUSE_SMOKES
   			DestroyObject(L_SAMMissileSmoke[i]);
   #endif
  		}
 	}
	for(new i; i< MAX_PLAYERS; i++)
 	{
  		if(MissileInProgress[i]) RemovePlayerMapIcon(i, 99);
 	}
 	CallLocalFunction("L_SAM_OnGMExit", "");
 	return 1;
}

#if defined _ALS_OnGameModeExit
	#undef OnGameModeExit
#else
	#define _ALS_OnGameModeExit
#endif
forward L_SAM_OnGMExit();
#endif

forward LordzSAMTimer();

stock AvoidSAMAttacksOnPlayer(playerid, samid)
{
 	if(L_SAMData[samid][L_SAMCreated] == false) return -1;
 	L_SAM_ExceptionalPlayer[playerid][samid] = true;
 	return 1;
}

stock EnableSAMAttacksOnPlayer(playerid, samid)
{
 	if(L_SAMData[samid][L_SAMCreated] == false) return -1;
 	L_SAM_ExceptionalPlayer[playerid][samid] = false;
 	return 1;
}

stock ToggleNoSAMAttacksOnPlayer(playerid, samid, bool:noattack)
{
 	if(L_SAMData[samid][L_SAMCreated] == false) return -1;
 	L_SAM_ExceptionalPlayer[playerid][samid] = noattack;
 	return 1;
}

stock SetSAMMissileSpeed(samid, speed)
{
 	if(L_SAMData[samid][L_SAMCreated] == false) return 0;
 	L_SAMData[samid][L_SAMSpeed] = speed;
 	return 1;
}

stock GetSAMMissileSpeed(samid)
{
  	if(L_SAMData[samid][L_SAMCreated] == false) return -1;
  	return L_SAMData[samid][L_SAMSpeed];
}

stock SetSAMMissilePower(samid, Power)
{
 	if(L_SAMData[samid][L_SAMCreated] == false) return -1;
 	if(Power >= SAM_POWER_BEST) L_SAMData[samid][L_SAMPower] = SAM_POWER_BEST;
 	else if(Power <= SAM_POWER_LOW) L_SAMData[samid][L_SAMPower] = SAM_POWER_LOW;
 	else L_SAMData[samid][L_SAMPower] = Power;
 	return Power;
}

stock GetSAMMissilePower(samid)
{
 	if(L_SAMData[samid][L_SAMCreated] == false) return -1;
 	return L_SAMData[samid][L_SAMPower];
}

stock GetSAMObjectID(samid)
{
 	if(L_SAMData[samid][L_SAMCreated] == false) return -1;
 	return L_SAMData[samid][L_SAMObjectID];
}

stock IsSAMMissilesFollowing(samid)
{
 	if(L_SAMData[samid][L_SAMCreated] == false) return -1;
 	if(L_SAMData[samid][L_SAMFollow] == true) return true;
 	else return false;
}

stock SetSAMMissileColor(samid, color)
{
 	if(L_SAMData[samid][L_SAMCreated] == false) return 0;
 	L_SAMData[samid][L_SAMMissileColor] = color;
 	return 1;
}

stock GetSAMMissileColor(samid)
{
 	return L_SAMData[samid][L_SAMMissileColor];
}

//Thanks to the person who created the function "randomEx"
stock L_randomEx(minnum = cellmin, maxnum = cellmax) return random(maxnum - minnum + 1) + minnum;


public LordzSAMTimer()
{
 	for(new i; i< MAX_LSAMS; i++)
 	{
  		if(L_SAMData[i][L_SAMCreated])
  		{
   			for(new a; a< GetMaxPlayers(); a++)
   			{
				if(!IsPlayerConnected(a)) continue;
				if(!IsPlayerInRangeOfPoint(a, L_SAMData[i][L_SAMRange], L_SAMData[i][L_SAMX], L_SAMData[i][L_SAMY], L_SAMData[i][L_SAMZ])) continue;
				if(L_SAM_ExceptionalPlayer[a][i] == true) continue;
				if(MissileInProgress[a] == true) continue;
				GetPlayerPos(a, L_SAMPlayerPos[a][0], L_SAMPlayerPos[a][1], L_SAMPlayerPos[a][2]);
				new p_missileobj = CreateObject(345, L_SAMData[i][L_SAMX], L_SAMData[i][L_SAMY], L_SAMData[i][L_SAMZ], 0.0, 0.0, 0.0, 300.0);
		#if defined L_SAMUSE_SMOKES
				new p_missilesmoke = CreateObject(18723, L_SAMData[i][L_SAMX], L_SAMData[i][L_SAMY], L_SAMData[i][L_SAMZ], 0.0, 0.0, 0.0, 300.0);
				AttachObjectToObject(p_missilesmoke, p_missileobj, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1);
		#endif
				PlayerMissile[a] = p_missileobj;
		#if defined L_SAMUSE_SMOKES
				PlayerMissileSmoke[a] = p_missilesmoke;
		#endif
				L_SAMMissileObject[p_missileobj] = 1;
		#if defined L_SAMUSE_SMOKES
				L_SAMMissileSmoke[p_missileobj] = p_missilesmoke;
		#endif
				L_SAMMissilePower[p_missileobj] = L_SAMData[i][L_SAMPower];
				L_SAMMissileSAMID[p_missileobj] = i;
				L_SAMMissileProgress[p_missileobj] = a;
				L_SetObjectFaceCoords3D(p_missileobj, L_SAMPlayerPos[a][0], L_SAMPlayerPos[a][1], L_SAMPlayerPos[a][2]);
				MoveObject(p_missileobj, L_SAMPlayerPos[a][0], L_SAMPlayerPos[a][1], L_SAMPlayerPos[a][2], L_SAMData[i][L_SAMSpeed]);
				MissileInProgress[a] = true;
				SetPlayerMapIcon(a, 99, L_SAMData[i][L_SAMX], L_SAMData[i][L_SAMY], L_SAMData[i][L_SAMZ], 0, L_SAMData[i][L_SAMMissileColor]);
				L_SAMMissileMarker[a] = SetTimerEx("L_SAMUpdateMapIcon", 250, true, "ii", a, i);
				if(L_SAMData[i][L_SAMFollow] == true) L_SAMFollowStopped[a] = false;
				CallLocalFunction("OnSAMMissileLaunch", "ii", a, i);
   			}
  		}
 	}
 	return 1;
}

stock DestroySAMMissile(playerid)
{
 	if(!MissileInProgress[playerid]) return 0;
 	L_SAMMissileStopped[PlayerMissile[playerid]] = false;
 	L_SAMFollowStopped[playerid] = true;
 	return CallLocalFunction("OnObjectMoved", "i", PlayerMissile[playerid]);
}

stock IsSAMMissileChasing(playerid)
{
 	if(MissileInProgress[playerid]) return true;
 	else return false;
}

forward L_SAMUpdateMapIcon(playerid, samid);
public L_SAMUpdateMapIcon(playerid, samid)
{
 	if(!MissileInProgress[playerid]) return KillTimer(L_SAMMissileMarker[playerid]);
 	new Float:X, Float:Y, Float:Z, Float:oX, Float:oY, Float:oZ;
 	if(L_SAMData[samid][L_SAMFollow] == true)
 	{
  		L_SAMFollowStopped[playerid] = false;
  		GetPlayerPos(playerid, X, Y, Z);
  		GetObjectPos(PlayerMissile[playerid], oX, oY, oZ);
  		if(L_SAMData[samid][L_SAMFollowLimit] == true)
  		{
   			if(IsPlayerConnected(playerid))
   			{
				if(!(oX >= L_SAMData[samid][L_SAMX]-L_SAMData[samid][L_SAMFollowLimitRange] &&
				oX <= L_SAMData[samid][L_SAMX]+L_SAMData[samid][L_SAMFollowLimitRange] &&
				oY >= L_SAMData[samid][L_SAMY]-L_SAMData[samid][L_SAMFollowLimitRange] &&
				oY <= L_SAMData[samid][L_SAMY]+L_SAMData[samid][L_SAMFollowLimitRange]))
				{
					L_SAMMissileStopped[PlayerMissile[playerid]] = false;
					L_SAMFollowStopped[playerid] = true;
					return CallLocalFunction("OnObjectMoved", "i", PlayerMissile[playerid]);
  				}
   			}
  		}
  		if(!IsPlayerConnected(playerid) || IsPlayerInRangeOfPoint(playerid, 10.0, oX, oY, oZ) || L_SAM_ExceptionalPlayer[playerid][samid] == true)
  		{
			L_SAMMissileStopped[PlayerMissile[playerid]] = false;
  			return CallLocalFunction("OnObjectMoved", "i", PlayerMissile[playerid]);
  		}
  		else
  		{
   			L_SAMMissileStopped[PlayerMissile[playerid]] = true;
   			L_SAMAssignedSpeed[PlayerMissile[playerid]] = L_SAMData[samid][L_SAMSpeed]+L_randomEx(2,20); //Increasing the speed!
   			StopObject(PlayerMissile[playerid]);
   			L_SetObjectFaceCoords3D(PlayerMissile[playerid], X, Y, Z);
   			MoveObject(PlayerMissile[playerid], X, Y, Z, L_SAMAssignedSpeed[PlayerMissile[playerid]]);
  		}
	}
 	GetObjectPos(PlayerMissile[playerid], X, Y, Z);
 	SetPlayerMapIcon(playerid, 99, X, Y, Z, 0, L_SAMData[samid][L_SAMMissileColor]);
 	return 1;
}

public OnObjectMoved(objectid)
{
 	if(L_SAMMissileObject[objectid] == 1)
 	{
  		if(L_SAMMissileStopped[objectid] == false)
  		{
  			new Float:X, Float:Y, Float:Z;
  			GetObjectPos(objectid, X, Y, Z);
  			switch(L_SAMMissilePower[objectid])
  			{
  	 			case SAM_POWER_LOW: CreateExplosion(X+4.0, Y+4.0, Z, 2, 12.0);
   				case SAM_POWER_NORMAL: CreateExplosion(X+6.0, Y+6.0, Z, 6, 10.0);
   				case SAM_POWER_HIGH:
   				{
      				CreateExplosion(X+8.0,Y, Z, 7, 10.0);
  	  				CreateExplosion(X-8.0,Y, Z, 7, 10.0);
	  				CreateExplosion(X, Y+8.0,Z, 7, 10.0);
  	  				CreateExplosion(X, Y-8.0,Z, 7, 10.0);
   				}
   				case SAM_POWER_BEST:
   				{
	  				new Float:range = 8.0;
      				CreateExplosion(X+range,Y, Z, 6, 10.0);
  	  				CreateExplosion(X-range,Y, Z, 6, 10.0);
  					CreateExplosion(X, Y+range,Z, 6, 10.0);
  	  				CreateExplosion(X, Y-range,Z, 6, 10.0);
  					CreateExplosion(X+range,Y+range,Z, 6, 10.0);
      				CreateExplosion(X-range,Y+range,Z, 6, 10.0);
      				CreateExplosion(X-range,Y-range,Z, 6, 10.0);
   				}
  			}
  			if(IsPlayerConnected(L_SAMMissileProgress[objectid]) && L_SAMFollowStopped[L_SAMMissileProgress[objectid]] == false)
 			CallLocalFunction("OnSAMMissileHitPlayer", "ii", L_SAMMissileProgress[objectid], L_SAMMissileSAMID[objectid]);
  			DestroyObject(objectid);
  		#if defined L_SAMUSE_SMOKES
			DestroyObject(L_SAMMissileSmoke[objectid]);
  		#endif
  			RemovePlayerMapIcon(L_SAMMissileProgress[objectid], 99);
			L_SAMMissileObject[objectid] = 0;
  			L_SAMMissilePower[objectid] = 0;
  			L_SAMMissileSAMID[objectid] = -1;
  			MissileInProgress[L_SAMMissileProgress[objectid]] = false;
  			L_SAMMissileProgress[objectid] = -1;
  		}
	}
 	CallLocalFunction("L_SAM_OnObjMoved", "i", objectid);
 	return 1;
}

forward OnSAMMissileLaunch(playerid, samid);
forward OnSAMMissileHitPlayer(playerid, samid);


#if defined _ALS_OnObjectMoved
	#undef OnObjectMoved
#else
	#define _ALS_OnObjectMoved
#endif

forward L_SAM_OnObjMoved(objectid);

public OnPlayerConnect(playerid)
{
 	for(new i; i< MAX_LSAMS; i++) L_SAM_ExceptionalPlayer[playerid][i] = false;
 	CallLocalFunction("L_SAM_OPC", "i", playerid);
 	return 1;
}

#if defined _ALS_OnPlayerConnect
	#undef OnPlayerConnect
#else
	#define _ALS_OnPlayerConnect
#endif

forward L_SAM_OPC(playerid);

public OnPlayerSpawn(playerid)
{
 	if(MissileInProgress[playerid] == true)
 	{
  		L_SAMMissileStopped[PlayerMissile[playerid]] = false;
  		CallLocalFunction("OnObjectMoved", "i", PlayerMissile[playerid]);
 	}
 	CallLocalFunction("L_SAM_OPS", "i", playerid);
 	return 1;
}

#if defined _ALS_OnPlayerSpawn
	#undef OnPlayerSpawn
#else
	#define _ALS_OnPlayerSpawn
#endif

forward L_SAM_OPS(playerid);

#define OnObjectMoved L_SAM_OnObjMoved
#if defined FILTERSCRIPT
	#define OnFilterScriptInit L_SAM_OnFS
	#define OnFilterScriptExit L_SAM_OnFSExit
#else
	#define OnGameModeInit L_SAM_OnGM
	#define OnGameModeExit L_SAM_OnGMExit
#endif
#define OnPlayerConnect L_SAM_OPC
#define OnPlayerSpawn L_SAM_OPS
