#ifndef _NETWORK_H
#define _NETWORK_H

#include "RakNetworkFactory.h"
#include "RakPeerInterface.h"

#include "RakNetTime.h"
#include "GetTime.h"

#include "GlobalVariables.h"
#include "Database.h"
#include "LinkedList.h"

class Network{

private:

	GlobalVariables		*gVar;
	Database			*dbPtr;
	LinkedList			*userList;

	RakPeerInterface	*peer;
	Packet				*packet;

	// Packets
	PositionPack		*tempPosPack;		// Temp Position & Orientation packet
	OrientationPack		*tempOrntPack;

	int uOnline;

	//debug
	int pktCnt;
	int min, max;
	int x, z;
	char clientIp[50];

	bool networkState;						// if the network has been initialized, true.

public:
	Network();
	~Network();

	bool initializeNetwork();
	bool terminateNetwork();

	void readPacket();

	// Send...
	bool sendInitPack(userData *currentUser);
	bool sendErrorPackToClient(unsigned char _errorID, SystemAddress trgClient);
	bool sendIDLEPacket(char uuid[UUID_LEN], SystemAddress trgClient);

	bool sendUserDescriptionPack(char uuid[UUID_LEN], SystemAddress trgClient);
	bool sendUserDescriptionPack(userData *currentUser, SystemAddress trgClient);

	// * !!! *  TODO:: just relay the packet to those in range (instead of uuid, pozition) -> just use PozitionPacket *posPack
	// * !!! *  same with Orientation packet
	bool sendPositionPack(char uuid[UUID_LEN], SystemAddress trgClient);
	bool sendPositionPack(char uuid[UUID_LEN], myPosition poz, SystemAddress trgClient);
	bool sendPositionPack(PositionPack *_p, SystemAddress trgClient);

	bool sendOrientationPack(char uuid[UUID_LEN], float orientation, SystemAddress trgClient);
	bool sendOrientationPack(OrientationPack *_p, SystemAddress trgClient);

	// send destruction for user uuid to user trgClient.
	// this packet will cause the target client to remove the user (uuid) from it's List.
	bool sendUserDestructionPack(char uuid[UUID_LEN], SystemAddress trgClient);

	// Proccess received...
	void proccessLoginPack(Packet *p);
	void proccessPositionPack(Packet *p);
	void proccessOrientationPack(Packet *p);
	void proccessClientDisconnection(Packet *p);
	void proccessCharDescriptionRequest(Packet *p);
	void proccessIDLEPacket(Packet *p);

	// set functions
	void setGlobalVariablesPtr(GlobalVariables *g){gVar = g;};
	void setDatabasePtr(Database *d){dbPtr = d;};
	void setLinkedListPtr(LinkedList *l){userList = l;};

	// get functions

	// mass user functions [temp]
	void sendToAllInRange(char uuid[UUID_LEN], myPosition pozition);
	void sendToAllInRange(char uuid[UUID_LEN], float orientation);
	void sendToAllInRange(char uuid[UUID_LEN]);

	void relayPacketToAll(PositionPack *_p);
	void relayPacketToAll(OrientationPack *_p);

	void sendIDLEtoAllInRange(char uuid[UUID_LEN]);
	void SendInitUserDescriptions(SystemAddress targetClient);

	void SendUserDestructionToAll(char uuid[UUID_LEN]);

	void CheckWhoIsAlive();
};


#endif

