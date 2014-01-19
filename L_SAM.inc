/*______________________________________________________________________________

					L-SAM "Lordz's Surface To Air Missiles" - v1.0
					Copyright(c) 2014 - L_SAM.inc
					Author : Lordzâ„¢
					Thanks to Incognito for Streamer
					    &
		   All the players of YoUnG Generations for BETA Testing.

* Happy New Year!

//Functions:

native CreateSAM(Float:X, Float:Y, Float:Z, Float:rX=0.0, Float:rY=0.0, Float:rZ=0.0, Float:DrawDistance=200.0, Float:Range=300.0, Speed=80, Power=SAM_POWER_NORMAL, bool:follow=false)
native SetSAMToAttackVehicles(samid, bool:occupied = true);
native DestroySAM(samid);
native AvoidSAMAttacksOnPlayer(playerid, samid);
native EnableSAMAttacksOnPlayer(playerid, samid);
native ToggleNoSAMAttacksOnPlayer(playerid, samid, bool:noattack);
native SetSAMMissileSpeed(samid, speed);
native SetSAMMissilePower(samid, Power);
native GetSAMMissileSpeed(samid);
native GetSAMMissilePower(samid);
native GetSAMObjectID(samid);

_______________________________________________________________________________*/

#if defined _included_l_sam
 #endinput
#endif

#define _included_l_sam

#include <a_samp>

//#define USE_STREAMER //Uncomment this line in case if you want to use streamer -
					// SAM objects.

#if defined USE_STREAMER
 #include <streamer>
#endif

#define MAX_LSAMS 100 //Change this value to create SAM launchers more than 100.

#define SAM_POWER_LOW    0
#define SAM_POWER_NORMAL 1
#define SAM_POWER_HIGH   2
#define SAM_POWER_BEST   3



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
 bool:L_SAMCreated,
 L_SAMObjectID,
}

new L_SAMData[MAX_LSAMS][L_saminfo];
new L_CountSAMS = 0;
new Float:L_SAMPlayerPos[MAX_PLAYERS][3];
new PlayerMissile[MAX_PLAYERS];
new bool:MissileInProgress[MAX_PLAYERS];
new L_SAMMissileObject[MAX_OBJECTS];
new L_SAMMissilePower[MAX_OBJECTS];
new L_SAMMissileProgress[MAX_OBJECTS];
new L_SAMMissileMarker[MAX_PLAYERS];
new bool:L_SAM_ExceptionalPlayer[MAX_PLAYERS][MAX_LSAMS];
#if defined USE_STREAMER
stock CreateStreamedObject(modelid, Float:X, Float:Y, Float:Z, Float:rX, Float:rY, Float:rZ, vworld = -1, interiorid = -1, playerid = -1, Float:streamdistance = 300.0, Float:drawdistance = 300.0)
{
 new Object_ID = CreateDynamicObject(modelid, X, Y, Z, rX, rY, rZ, vworld, interiorid, playerid, streamdistance);
 Streamer_SetFloatData(STREAMER_TYPE_OBJECT, Object_ID, E_STREAMER_DRAW_DISTANCE, drawdistance);
 return Object_ID;
}
#endif

stock CreateSAM(Float:X, Float:Y, Float:Z, Float:rX=0.0, Float:rY=0.0, Float:rZ=0.0, Float:DrawDistance=200.0, Float:Range=300.0, Speed=80, Power=SAM_POWER_NORMAL, bool:follow=false)
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
 if(Power >= SAM_POWER_BEST) L_SAMData[samid][L_SAMPower] = SAM_POWER_BEST;
 else if(Power <= SAM_POWER_LOW) L_SAMData[samid][L_SAMPower] = SAM_POWER_LOW;
 else L_SAMData[samid][L_SAMPower] = Power;
 L_SAMData[samid][L_SAMFollow] = follow;
 #if defined USE_STREAMER
 L_SAMData[samid][L_SAMObjectID] = CreateStreamedObject(3267, X, Y, Z, rX, rY, rZ, -1, -1, -1, DrawDistance, DrawDistance);
 #else
 L_SAMData[samid][L_SAMObjectID] = CreateObject(3267, X, Y, Z, rX, rY, rZ, DrawDistance);
 #endif
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
  return 1;
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
  for(new a; a< MAX_PLAYERS; a++) L_SAM_ExceptionalPlayer[a][samid] = false;
 }
 for(new i; i< MAX_OBJECTS; i++)
 {
  L_SAMMissileObject[i] = 0;
  L_SAMMissilePower[i] = 0;
  L_SAMMissileProgress[i] = -1;
 }
 for(new i; i< MAX_PLAYERS; i++)
 {
  MissileInProgress[i] = false;
  L_SAMPlayerPos[i][0] = 0.0;
  L_SAMPlayerPos[i][1] = 0.0;
  L_SAMPlayerPos[i][2] = 0.0;
  PlayerMissile[i] = -1;
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
 #if !defined USE_STREAMER
 for(new i; i< MAX_LSAMS; i++)
 {
  if(L_SAMData[i][L_SAMCreated])
  {
   DestroyObject(L_SAMData[i][L_SAMObjectID]);
  }
 }
 #endif
 for(new i; i< MAX_OBJECTS; i++)
 {
  if(L_SAMMissileProgress[i])
  {
   DestroyObject(i);
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
  for(new a; a< MAX_PLAYERS; a++) L_SAM_ExceptionalPlayer[a][samid] = false;
 }
 for(new i; i< MAX_OBJECTS; i++)
 {
  L_SAMMissileObject[i] = 0;
  L_SAMMissilePower[i] = 0;
  L_SAMMissileProgress[i] = -1;
 }
 for(new i; i< MAX_PLAYERS; i++)
 {
  MissileInProgress[i] = false;
  L_SAMPlayerPos[i][0] = 0.0;
  L_SAMPlayerPos[i][1] = 0.0;
  L_SAMPlayerPos[i][2] = 0.0;
  PlayerMissile[i] = -1;
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
 #if !defined USE_STREAMER
 for(new i; i< MAX_LSAMS; i++)
 {
  if(L_SAMData[i][L_SAMCreated])
  {
   DestroyObject(L_SAMData[i][L_SAMObjectID]);
  }
 }
 #endif
 for(new i; i< MAX_OBJECTS; i++)
 {
  if(L_SAMMissileProgress[i])
  {
   DestroyObject(i);
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
 L_SAM_ExceptionalPlayer[playerid][samid] = 1;
 return 1;
}

stock EnableSAMAttacksOnPlayer(playerid, samid)
{
 if(L_SAMData[samid][L_SAMCreated] == false) return -1;
 L_SAM_ExceptionalPlayer[playerid][samid] = 0;
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
	PlayerMissile[a] = p_missileobj;
	L_SAMMissileObject[p_missileobj] = 1;
	L_SAMMissilePower[p_missileobj] = L_SAMData[i][L_SAMPower];
	L_SAMMissileProgress[p_missileobj] = a;
	MoveObject(p_missileobj, L_SAMPlayerPos[a][0], L_SAMPlayerPos[a][1], L_SAMPlayerPos[a][2], L_SAMData[i][L_SAMSpeed]);
	MissileInProgress[a] = true;
	SetPlayerMapIcon(a, 99, L_SAMData[i][L_SAMX], L_SAMData[i][L_SAMY], L_SAMData[i][L_SAMZ], 0, 0xFF0000FF);
	L_SAMMissileMarker[a] = SetTimerEx("L_SAMUpdateMapIcon", 250, true, "i", a);
   }
  }
 }
 return 1;
}


forward L_SAMUpdateMapIcon(playerid);
public L_SAMUpdateMapIcon(playerid)
{
 if(!MissileInProgress[playerid]) return KillTimer(L_SAMMissileMarker[playerid]);
 new Float:X, Float:Y, Float:Z;
 GetObjectPos(PlayerMissile[playerid], X, Y, Z);
 SetPlayerMapIcon(playerid, 99, X, Y, Z, 0, 0xFF0000FF);
 return 1;
}

public OnObjectMoved(objectid)
{
 if(L_SAMMissileObject[objectid] == 1)
 {
  new Float:X, Float:Y, Float:Z;
  GetObjectPos(objectid, X, Y, Z);
  switch(L_SAMMissilePower[objectid])
  {
   case 0: CreateExplosion(X+4.0, Y+4.0, Z, 1, 10.0);
   case 1: CreateExplosion(X+4.0, Y+4.0, Z, 7, 10.0);
   case 2:
   {
      CreateExplosion(X+8.0,Y, Z, 7, 10.0);
  	  CreateExplosion(X-8.0,Y, Z, 7, 10.0);
	  CreateExplosion(X, Y+8.0,Z, 7, 10.0);
  	  CreateExplosion(X, Y-8.0,Z, 7, 10.0);
   }
   case 3:
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
  DestroyObject(objectid);
  RemovePlayerMapIcon(L_SAMMissileProgress[objectid], 99);
  L_SAMMissileObject[objectid] = 0;
  L_SAMMissilePower[objectid] = 0;
  MissileInProgress[L_SAMMissileProgress[objectid]] = false;
  L_SAMMissileProgress[objectid] = -1;
 }
 CallLocalFunction("L_SAM_OnObjMoved", "i", objectid);
 return 1;
}

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

#define OnObjectMoved L_SAM_OnObjMoved
#if defined FILTERSCRIPT
#define OnFilterScriptInit L_SAM_OnFS
#define OnFilterScriptExit L_SAM_OnFSExit
#else
#define OnGameModeInit L_SAM_OnGM
#define OnGameModeExit L_SAM_OnGMExit
#endif
#define OnPlayerConnect L_SAM_OPC
