#pragma once

#include "stdafx.h"

enum CPacketsID : unsigned short
{
	CHECK_VERSION = 0, // must be always 0! TODO
	PLAYER_CONNECTED,
	PLAYER_DISCONNECTED,
	PLAYER_ONFOOT,
	PLAYER_BULLET_SHOT,
	PLAYER_HANDSHAKE,
	PLAYER_PLACE_WAYPOINT,
	PLAYER_GET_NAME,
	VEHICLE_SPAWN,
	PLAYER_SET_HOST,
	ADD_EXPLOSION,
	VEHICLE_REMOVE,
	VEHICLE_IDLE_UPDATE,
	VEHICLE_DRIVER_UPDATE,
	VEHICLE_ENTER,
	VEHICLE_EXIT,
	VEHICLE_DAMAGE,
	VEHICLE_COMPONENT_ADD,
	VEHICLE_COMPONENT_REMOVE,
	VEHICLE_PASSENGER_UPDATE,
	PLAYER_CHAT_MESSAGE,
	PED_SPAWN,
	PED_REMOVE,
	PED_ONFOOT,
	GAME_WEATHER_TIME,
	PED_ADD_TASK,
	PED_REMOVE_TASK,
	PLAYER_KEY_SYNC,
	PED_DRIVER_UPDATE,
	PED_SHOT_SYNC,
	PED_PASSENGER_UPDATE,
	PLAYER_AIM_SYNC,
	VEHICLE_CONFIRM,
	PED_CONFIRM,
	PLAYER_STATS,
	REBUILD_PLAYER,
	RESPAWN_PLAYER
};

class CPackets
{
public:

	struct PlayerConnected
	{
		int id;
	};

	struct PlayerDisconnected
	{
		int id;
		unsigned char reason;
	};

	struct PlayerOnFoot
	{
		int id = 0;
		CVector position = CVector();
		CVector velocity = CVector();
		float rotation = 0.0f;
		unsigned char health = 100;
		unsigned char armour = 0;
		unsigned char weapon = 0;
		unsigned short ammo = 0;
		bool ducking = false;
		bool hasJetpack = false;
		char fightingStyle = 4;
	};

	#pragma pack(1)
	struct PlayerBulletShot
	{
		int playerid;
		int targetid;
		CVector startPos;
		CVector endPos;
		CColPoint colPoint;
		int incrementalHit;
		unsigned char entityType;
	};

	struct PlayerHandshake
	{
		int yourid;
	};

	struct PlayerPlaceWaypoint
	{
		int playerid;
		bool place;
		CVector position;
	};

	struct PlayerGetName
	{
		int playerid;
		char name[32 + 1];
	};

	struct PlayerSetHost
	{
		int playerid;
	};

	struct AddExplosion
	{
		unsigned char type;
		CVector pos;
		int time;
		bool usesSound;
		float cameraShake;
		bool isVisible;
		int entityid;
		unsigned char entityType;
	};

	struct VehicleSpawn
	{
		int vehicleid;
		unsigned char tempid;
		unsigned short modelid;
		CVector pos;
		float rot;
		unsigned char color1;
		unsigned char color2;
		unsigned char createdBy;
	};

	struct VehicleRemove
	{
		int vehicleid;
	};

	struct VehicleIdleUpdate
	{
		int vehicleid;
		CVector pos;
		CVector rot;
		CVector roll;
		CVector velocity;
		CVector turnSpeed;
		unsigned char color1;
		unsigned char color2;
		float health;
		char paintjob;
		float planeGearState;
		unsigned char locked;
	};

	struct VehicleDriverUpdate
	{
		int playerid;
		int vehicleid;
		CVector pos;
		CVector rot;
		CVector roll;
		CVector velocity;
		unsigned char playerHealth;
		unsigned char playerArmour;
		unsigned char weapon;
		unsigned short ammo;
		unsigned char color1;
		unsigned char color2;
		float health;
		char paintjob;
		float bikeLean;
		unsigned short miscComponentAngle; // hydra thrusters
		float planeGearState;
		unsigned char locked;
	};

	struct VehicleEnter
	{
		int playerid;
		int vehicleid;
		unsigned char seatid : 3;
		unsigned char force : 1;
		unsigned char passenger : 1;
	};

	struct VehicleExit
	{
		int playerid;
		bool force;
	};

	struct VehicleDamage
	{
		int vehicleid;
		CDamageManager damageManager;
	};

	struct VehicleComponentAdd
	{
		int vehicleid;
		int componentid;
	};

	struct VehicleComponentRemove
	{
		int vehicleid;
		int componentid;
	};

	struct VehiclePassengerUpdate
	{
		int playerid;
		int vehicleid;
		unsigned char playerHealth;
		unsigned char playerArmour;
		unsigned char weapon;
		unsigned short ammo;
		unsigned char driveby;
		unsigned char seatid;
	};

	struct PlayerChatMessage
	{
		int playerid;
		char message[CChat::MAX_MESSAGE_SIZE+1];
	};

	struct PedSpawn
	{
		int pedid;
		unsigned char tempid;
		short modelId;
		unsigned char pedType;
		CVector pos;
		unsigned char createdBy;
	};

	struct PedRemove
	{
		int pedid;
	};

	struct PedOnFoot
	{
		int pedid = 0;
		CVector pos = CVector();
		CVector velocity = CVector();
		unsigned char health = 100;
		unsigned char armour = 0;
		unsigned char weapon = 0;
		unsigned short ammo = 0;
		float aimingRotation = 0.0f;
		float currentRotation = 0.0f;
		int lookDirection = 0;
		struct
		{
			unsigned char moveState : 3;
			unsigned char ducked : 1;
			unsigned char aiming : 1;
		};
		unsigned char fightingStyle = 4;
		CVector weaponAim;
	};
	
	struct GameWeatherTime
	{
		unsigned char newWeather;
		unsigned char oldWeather;
		unsigned char forcedWeather;
		unsigned char currentMonth;
		unsigned char currentDay;
		unsigned char currentHour;
		unsigned char currentMinute;
		unsigned int gameTickCount;
	};

	struct PedRemoveTask
	{
		int pedid;
		eTaskType taskid;
	};

	struct PlayerKeySync
	{
		int playerid;
		CCompressedControllerState newState;
	};

	struct PedDriverUpdate
	{
		int pedid;
		int vehicleid;
		CVector pos;
		CVector rot;
		CVector roll;
		CVector velocity;
		CVector turnSpeed;
		unsigned char pedHealth;
		unsigned char pedArmour;
		unsigned char weapon;
		unsigned short ammo;
		unsigned char color1;
		unsigned char color2;
		float health;
		char paintjob;
		float bikeLean;
		float planeGearState;
		unsigned char locked;
	};

	struct PedShotSync
	{
		int pedid;
		CVector origin;
		CVector effect;
		CVector target;
	};

	struct PedPassengerSync
	{
		int pedid;
		int vehicleid;
		unsigned char health;
		unsigned char armour;
		unsigned char weapon;
		unsigned short ammo;
		unsigned char seatid;
	};

	struct PlayerAimSync
	{
		int playerid;
		unsigned char cameraMode;
		float cameraFov;
		CVector front;
		CVector	source;
		CVector	up;
		float moveHeading;
		float aimY;
		float aimZ;
	};

	struct VehicleConfirm
	{
		unsigned char tempid = 255;
		int vehicleid;
	};

	struct PedConfirm
	{
		unsigned char tempid = 255;
		int pedid;
	};

	struct PlayerStats
	{
		int playerid;
		float stats[14];
	};

	struct RebuildPlayer
	{
		int playerid;
		CPedClothesDesc clothesData;
	};

	struct AssignVehicleSyncer
	{
		int vehicleid;
	};

	struct RespawnPlayer
	{
		int playerid;
	};
};