/* ------------------------------------------------------------------------------------ *
 * DataTypes Header																		*
 * Filename: DataTypes.h																*
 *																						*
 * ------------------------------------------------------------------------------------ *
 * Description:																			*
 * ------------------------------------------------------------------------------------ *
 * Contains the data types used throghout the game, such as packet structures,			*
 * Link List data types, etc..															*
 *																						*
 * ------------------------------------------------------------------------------------ *
 */

#ifndef _DATATYPES_H
#define _DATATYPES_H

#include "MessageIdentifiers.h"
#include "RakNetTypes.h"

#include "Ogre.h"

using namespace Ogre;

#define UUID_LEN 4
// Custom Packet ID for use in RAKNET 
enum{
	ID_POSITION_PACK = ID_USER_PACKET_ENUM,		// Position & orientation packet
	ID_ORIENTATION_PACK,
	ID_INIT_UPDATE,								// the initial main character's update packet
	ID_REQUEST_INIT_UPDATE,						// send after login confirmation ?? maybe
	ID_INIT_CHAR_DESCRIPTION,
	ID_CHAR_DESCRIPTION,
	ID_DESTROY_USER,
	ID_LOGIN,
	ID_ERROR,
	ID_IDLE,	
};

// Error ID
enum __netErrorID{
	WRONG_LOGIN_DATA = 1,
	ALREADY_LOGGED_IN,
	USER_NOT_ONLINE,
	ERROR_SENDING_INIT_UPDATE,
};

struct myPosition{
	float x,y,z;
};

// -- Custom packets for use in Rakenet -----------------------------------------------+

// 1. Error Packet - used to display in client errors that occured to the server
// (eg. wrong password and/or username, on login)
// -------------------------------- *
#pragma pack(push, 1)
struct ErrorPack{
	unsigned char typeID;
	unsigned char ErrorID;
};
#pragma pack(pop)

// 2. Character Position Packet
// -------------------------------- *
#pragma pack(push, 1)
struct PositionPack{
	unsigned char typeID;
	char uuid[4];
	myPosition position;
};
#pragma pack(pop)

// 3. Character Orientation Packet
// -------------------------------- *
#pragma pack(push, 1)
struct OrientationPack{
	unsigned char typeID;
	char uuid[4];
	float Orientation;
};
#pragma pack(pop)

// 4. Initial Character Packet
// -------------------------------- *
#pragma pack(push, 1)
struct initUpdate{
	unsigned char typeID;
	char uuid[4];
	char username[26];
	char modelName[11];

	float movespeed;
	float range;

	myPosition position;
	float orientation;
};
#pragma pack(pop)

// 5. Request Character Description Packet
// -------------------------------- *
#pragma pack(push, 1) // this struct is used only to receive packet. shall i use push & pop?
struct OnlyUuidPack{
	unsigned char typeID;
	char uuid[4];
};
#pragma pack(pop)

// 6. Character Description Packet
// -------------------------------- *
#pragma pack(push, 1) // this struct is used only to receive packet. shall i use push & pop?
struct CharDescPack{
	unsigned char typeID;

	char uuid[4];
	char username[26];
	char modelName[11];

	float moveSpeed;

	myPosition position;
	float orientation;
};
#pragma pack(pop)

// 7. Login Packet
// -------------------------------- *
#pragma pack(push, 1)
struct LoginPack{
	unsigned char typeID;
	char loginName[25];
	char pwd_hash[18];
};
#pragma pack(pop)
// ---------------------------------------------------------------------------- end --+

// -- Structures that hold Unit data (mob, 3rd user, npc), for use in linked list ----+
struct userData{
	SystemAddress clientID;

	std::string uuid;
	std::string	username;	
	std::string	charModel;
	
	myPosition	userPos;
	float		orientation;

	float		range;
	float		moveSpeed;

	bool		ChangeSinceLastSave;
	bool		keepAlive;
	
	bool		isMoving;
	bool		savedInDb;

	bool		DestructionPacketSent;
};
// ---------------------------------------------------------------------------- end --+

#endif

