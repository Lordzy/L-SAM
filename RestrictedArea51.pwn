/*______________________________________________________________________________

						Lordz's Restricted Area 51
						Copyright(c) - 2014 "L_Area51"

This filterscript is an example of L_SAM3 (L_SAM version 1.3). Many of you might
have seen S-A-M missiles on Restricted Area of Area 51 on GTA San Andreas.
However, those don't work really on SA-MP. Using this filterscript, it'll work!

NOTES:
If you got any teams spawning on that location and if you don't want them to get
attacked by S-A-M, use "Area51_NoAttacks" function. Else if they want to get
attacked, use "Area51_AttackPlayer" function on them.

The missiles are kept to follow and while following if player goes out of range,
it will stop following and gets exploded as "followlimit" is kept to true.

Once if you're logged in as RCON, the S-A-M missiles won't attack you.

Example:

public OnPlayerSpawn(playerid)
{
 if(GetPlayerTeam(playerid) == TEAM_ARMY) Area51_NoAttacks(playerid);
 else Area51_AttackPlayer(playerid);
 return 1;
}


PS : My indentation isn't so clean, so here by, I'm trying to do better
	 indentations...
________________________________________________________________________________*/

#define FILTERSCRIPT

#include <a_samp>
#include <L_SAM3>

#define AREA51_RANGE 170.0 //The range of detection on the SAM missiles kept in area.

new L_Area51[4];


stock Area51_NoAttacks(playerid)
{
	for(new i; i< 4; i++)
  	{
  		AvoidSAMAttacksOnPlayer(playerid, L_Area51[i]);
 	}
 	return 1;
}

stock Area51_AttackPlayer(playerid)
{
 	for(new i; i< 4; i++)
 	{
  		EnableSAMAttacksOnPlayer(playerid, L_Area51[i]);
 	}
 	return 1;
}



public OnPlayerConnect(playerid)
{
 	//Removes the original S-A-M objects placed on Area 51 which don't work.
 	RemoveBuildingForPlayer(playerid, 3267, 188.2422, 2081.6484, 22.4453, 0.25);
 	RemoveBuildingForPlayer(playerid, 3267, 15.6172, 1719.1641, 22.4141, 0.25);
 	RemoveBuildingForPlayer(playerid, 3267, 237.6953, 1696.8750, 22.4141, 0.25);
 	RemoveBuildingForPlayer(playerid, 3267, 354.4297, 2028.4922, 22.4141, 0.25);
 	Area51_AttackPlayer(playerid);
 	return 1;
}

public OnFilterScriptInit()
{
 	for(new playerid; playerid< GetMaxPlayers(); playerid++)
 	{
  		if(!IsPlayerConnected(playerid)) continue;
  		RemoveBuildingForPlayer(playerid, 3267, 188.2422, 2081.6484, 22.4453, 0.25);
  		RemoveBuildingForPlayer(playerid, 3267, 15.6172, 1719.1641, 22.4141, 0.25);
  		RemoveBuildingForPlayer(playerid, 3267, 237.6953, 1696.8750, 22.4141, 0.25);
  		RemoveBuildingForPlayer(playerid, 3267, 354.4297, 2028.4922, 22.4141, 0.25);
  		Area51_AttackPlayer(playerid);
 	}
 	L_Area51[0] = CreateSAM(188.24, 2081.65, 22.51,   0.00, 0.00, -0.66, 300.0, AREA51_RANGE, 80, 2, true,true);
 	L_Area51[1] = CreateSAM(354.13, 2028.33, 21.98,   0.00, 0.00, 0.00, 300.0, AREA51_RANGE, 80, 2, true,true);
 	L_Area51[2] = CreateSAM(238.68, 1696.95, 22.35,   0.00, 0.00, 0.00, 300.0, AREA51_RANGE, 80, 2, true,true);
 	L_Area51[3] = CreateSAM(16.20, 1719.31, 22.42,   0.00, 0.00, 0.00, 300.0, AREA51_RANGE, 80, 2, true,true);
	printf("------------------------------------");
 	printf("Restricted Area 51 by Lordz Loaded!");
 	printf("------------------------------------");
 	return 1;
}

public OnFilterScriptExit()
{
 	printf("------------------------------------");
 	printf("Restricted Area 51 by Lordz unloaded!");
 	printf("------------------------------------");
 	return 1;
}

public OnSAMMissileHitPlayer(playerid, samid)
{
 	for(new i; i< 4; i++)
 	{
  		if(samid == L_Area51[i]) GameTextForPlayer(playerid, "~Y~STAY AWAY FROM THE ~R~BOUNDARIES OF AREA51!", 1800, 3);
 	}
 	return 1;
}

public OnSAMMissileLaunch(playerid, samid)
{
 	for(new i; i< 4; i++)
 	{
  		if(samid == L_Area51[i]) GameTextForPlayer(playerid, "~R~WARNING:~N~~G~STAY AWAY FROM AREA51!", 1200, 3);
 	}
 	return 1;
}

public OnPlayerUpdate(playerid)
{
 	if(IsPlayerAdmin(playerid))
 	{
  		for(new i; i< 4; i++)
  		{
   			AvoidSAMAttacksOnPlayer(playerid, L_Area51[i]);
  		}
 	}
 	return 1;
}
