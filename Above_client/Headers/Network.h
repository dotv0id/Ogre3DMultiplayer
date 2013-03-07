/* ------------------------------------------------------------------------------------ *
 * Network class																		*
 * Filename: Network.h																	*
 *																						*
 * ------------------------------------------------------------------------------------ *
 * Description:																			*
 * ------------------------------------------------------------------------------------ *
 *	Handles communication between client and server. 									*
 *																						*
 * ------------------------------------------------------------------------------------ *
 */

#ifndef _NETWORK_H
#define _NETWORK_H

// Raknet Includes

#include "Raknet/RakNetworkFactory.h"
#include "Raknet/RakPeerInterface.h"
#include "Raknet/RakNetTypes.h"
#include "GetTime.h"

#include "globalVars.h"
#include "LinkedList.h"
#include "gameWorld.h"
#include "userCharacter.h"

class Network{
private:
	GlobalVariables		*n_glVar;			// Pointer to GlobalVariables Class
	LinkedList			*nUserUnit;			// Pointer to the User Unit Linked List
	GameWorld			*gWorldPtr;
	userCharacter		*mUserChar;
	
	SystemAddress		serverSAd;			// Server System Address (IP & port)
	RakPeerInterface	*peer;
	Packet				*packet;			// Temp Packet to hold incoming data
	
	// Packets
	PositionPack		*TempPosPack;
	LoginPack			*loginData;

	bool networkState;						// if the network has been initialized, true.
	bool connected;

	// Error checking	

public:
	bool idleWasSent;

	Network();
	~Network();

	bool initializeNetwork();
	bool connectToServer(const char *serverIP, int serverPort);
	void disconnectFromServer(bool sendNotification);

	bool terminateNetwork();

	void setupLoginPacket(std::string username, std::string password);	
	bool RequestCharDescription(char uuid[UUID_LEN]);
	bool RequestCharDescription();

	// Send packets...
	void sendLoginPacket();
	bool sendPosition(Vector3 currentPosition);
	bool sendOrientation(float yRot);
	bool sendIDLE();

	// Proccess received packets...
	void proccessInitUpdate(Packet *p);
	void proccessCharDescription(Packet *p);
	void proccessErrorPacket(Packet *p);
	void proccessPositionPacket(Packet *p);
	void proccessOrientationPacket(Packet *p);
	void proccessIDLEPacket(Packet *p);
	void proccessUserDestructionpacket(Packet *p);

	void printErrorMessage(const char *_errMsg);

	void readPacket();
	
	// Set fumctions
	void setGlobalVariablesPtr(GlobalVariables *gv){n_glVar = gv;};
	void setUnitPtr(LinkedList *u){nUserUnit = u;};
	void setWorldPtr(GameWorld *gw){gWorldPtr = gw;};

	// get functions
	userCharacter *getUserCharPtr(){return mUserChar;};
	bool getNetworkState(){return networkState;};
	bool isConnected(){return connected;};
};

#endif

