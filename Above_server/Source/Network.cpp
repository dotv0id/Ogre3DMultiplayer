#include "Network.h"

using namespace std;

Network::Network(){
	networkState = false;
}

Network::~Network(){
}

bool Network::initializeNetwork(){

	// debug
	pktCnt = 0;
	min = 500; max = 800;
	
	x = 500;
	z = 500;
	// eo Debug

	peer = RakNetworkFactory::GetRakPeerInterface();
	gVar->LocalIP = peer->GetLocalIP(0);						// get Local IP

	// Start the server
	bool rslt = peer->Startup(gVar->MaxConnections, 30, &SocketDescriptor(gVar->ServerPort, gVar->LocalIP), 1);
	peer->SetMaximumIncomingConnections(gVar->MaxConnections);

	return rslt;
}

void Network::readPacket(){
	HANDLE cntThread;
	cntThread = GetCurrentThread();	
	
	packet = peer->Receive();
				
	while(packet){
		switch (packet->data[0]){
			case ID_NEW_INCOMING_CONNECTION:
				printf("  * A connection is incoming.\n");				
				break;

			case ID_LOGIN:				
				packet->systemAddress.ToString(1, clientIp);
				cout<<"\n  * "<<clientIp<<" has send a login packet\n";
				proccessLoginPack(packet);
				break;

			case ID_INIT_CHAR_DESCRIPTION:				
				// send all the users in the list to the client who requested initial character descriptions
				SendInitUserDescriptions(packet->systemAddress);
				break;

			case ID_CHAR_DESCRIPTION:
				// send the character description packet for this user, if he user exists in the (online) list
				proccessCharDescriptionRequest(packet);
				break;

			case ID_POSITION_PACK:
				proccessPositionPack(packet);
				break;

			case ID_ORIENTATION_PACK:
				proccessOrientationPack(packet);
				break;

			case ID_IDLE:
				this->proccessIDLEPacket(packet);
				break;

			case ID_NO_FREE_INCOMING_CONNECTIONS:
				printf("  * The server is full.\n");
				break;

			case ID_DISCONNECTION_NOTIFICATION:
				cout<<"  ! "<<clientIp<<" has been disconnected...\n";				
				proccessClientDisconnection(packet);				
				break;

			case ID_CONNECTION_LOST:
				cout<<"  ! "<<clientIp<<" has lost connection...\n";				
				proccessClientDisconnection(packet);				
				break;

			default:
				cout<< "  ? Message with ID: " << packet->data[0] <<" has arrived.\n";				
				break;
			}

			peer->DeallocatePacket(packet);
			packet = peer->Receive();
	}
}

bool Network::sendInitPack(userData *currentUser){

	//create an init update pack, passing data from the userList
	initUpdate iuPack;
		
	// setup the initial Update packet
	iuPack.typeID = ID_INIT_UPDATE;

	strcpy(iuPack.uuid, currentUser->uuid.c_str());
	strcpy(iuPack.username, currentUser->username.c_str());
	strcpy(iuPack.modelName, currentUser->charModel.c_str());

	iuPack.movespeed	= currentUser->moveSpeed;
	iuPack.range		= currentUser->range;

	iuPack.position.x = currentUser->userPos.x; 
	iuPack.position.y = currentUser->userPos.y;
	iuPack.position.z = currentUser->userPos.z;

	iuPack.orientation = currentUser->orientation;

	// send the initUpdate packet
	cout<<"  * uuid: "<<iuPack.uuid<<", poz x:"<<iuPack.position.x<<",y: 0 ,z: "<<iuPack.position.z<<"\n";

	if(peer->Send((char*)&iuPack, sizeof(iuPack)+1, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, 0))
		return 1;

	return 0;
}

bool Network::sendUserDescriptionPack(char uuid[UUID_LEN], SystemAddress trgClient){
	bool result;

	CharDescPack tempPack;

	// get User Data from the list
	userData *crntUser = userList->findUnit(uuid);

	// if the user exists, setup & send the description packet
	if(crntUser != NULL)
	{
		tempPack.typeID = ID_CHAR_DESCRIPTION;
	
		strcpy(tempPack.uuid, crntUser->uuid.c_str());
		strcpy(tempPack.username, crntUser->username.c_str());
		strcpy(tempPack.modelName, "ninja.mesh");
	
		tempPack.position	 = crntUser->userPos;
		tempPack.orientation = crntUser->orientation;
		tempPack.moveSpeed	 = crntUser->moveSpeed;
	
		std::cout<<"  * uuid: "<<tempPack.uuid<<"\n";
		
		// Now that the packet is ready, send it
		result = peer->Send((char*)&tempPack, sizeof(tempPack)+1, HIGH_PRIORITY, RELIABLE_ORDERED, 0, trgClient, 0);
	}
	
	// debug message
	char clientIp[50];
	trgClient.ToString(true, clientIp);	
	cout<<"  * User Description Packet has been sent to: "<<clientIp<<"\n";

	return result;
}

// overloaded
bool Network::sendUserDescriptionPack(userData *currentUser, SystemAddress trgClient){
	bool result;

	CharDescPack tempPack;

	tempPack.typeID = ID_CHAR_DESCRIPTION;
	
	strcpy(tempPack.uuid, currentUser->uuid.c_str());
	strcpy(tempPack.username, currentUser->username.c_str());
	strcpy(tempPack.modelName, "ninja.mesh");
	
	tempPack.position = currentUser->userPos;
	tempPack.moveSpeed = currentUser->moveSpeed;
	
	std::cout<<"  * uuid: "<<tempPack.uuid<<"\n";
		
	// Now that the packet is ready, send it
	result = peer->Send((char*)&tempPack, sizeof(tempPack)+1, HIGH_PRIORITY, RELIABLE_ORDERED, 0, trgClient, 0);
	
	// debug message
	char clientIp[50];
	trgClient.ToString(true, clientIp);	
	cout<<"  * User Description Packet has been sent to: "<<clientIp<<"\n";

	return result;
}

// sends the current position of user with uuid, ***(input), to user with systemAddress trgClient
// returns true if everything ok, 
// and false if the user could not be found in the list, or if the send of packet failed.
bool Network::sendPositionPack(char uuid[UUID_LEN], SystemAddress trgClient){
	PositionPack pozPack;
	
	userData * _tUser = userList->findUnit(uuid);

	if(_tUser != NULL)
	{
		// setup and send the position packet
		pozPack.typeID		=  ID_POSITION_PACK;
	
		pozPack.position.x	= _tUser->userPos.x;
		pozPack.position.y	= _tUser->userPos.y;
		pozPack.position.z	= _tUser->userPos.z;

		return peer->Send((char*)&pozPack, sizeof(pozPack)+1, HIGH_PRIORITY, RELIABLE_ORDERED, 0, trgClient, 0);		
	}	
	return false;
}

bool Network::sendPositionPack(char uuid[UUID_LEN], myPosition poz, SystemAddress trgClient){
	PositionPack pozPack;

	pozPack.typeID = ID_POSITION_PACK;
	pozPack.position = poz;
	strcpy(pozPack.uuid, uuid);
	
	bool result = peer->Send((char*)&pozPack, sizeof(pozPack)+1, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0, trgClient, 0);
	return result;
}

bool Network::sendPositionPack(PositionPack *_p, SystemAddress trgClient){
	return peer->Send((char*)_p, sizeof(*_p)+1, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0, trgClient, 0);	
}

bool Network::sendOrientationPack(char uuid[UUID_LEN], float orientation, SystemAddress trgClient){
	OrientationPack orntPack;

	orntPack.typeID = ID_ORIENTATION_PACK;
	orntPack.Orientation = orientation;
	strcpy(orntPack.uuid, uuid);
	//std::cout<<"[sop inc]"<<orientation<<"\n";
	//std::cout<<"[sop pkt]"<<orntPack.Orientation<<"\n";

	bool result = peer->Send((char*)&orntPack, sizeof(orntPack)+1, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0, trgClient, 0);
	return result;
}

bool Network::sendOrientationPack(OrientationPack *_p, SystemAddress trgClient){
	return peer->Send((char*)_p, sizeof(*_p)+1, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0, trgClient, 0);	
}

bool Network::sendIDLEPacket(char uuid[], SystemAddress trgClient){
	OnlyUuidPack idlePack;

	idlePack.typeID = ID_IDLE;
	strcpy(idlePack.uuid, uuid);

	bool result = peer->Send((char*)&idlePack, sizeof(idlePack)+1, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, trgClient, 0);
	return result;
}

bool Network::sendUserDestructionPack(char uuid[UUID_LEN], SystemAddress trgClient){
	OnlyUuidPack destructionOrder;

	destructionOrder.typeID = ID_DESTROY_USER;
	strcpy(destructionOrder.uuid, uuid);

	bool result = peer->Send((char*)&destructionOrder, sizeof(destructionOrder)+1, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, trgClient, 0);
	return result;
}

bool Network::sendErrorPackToClient(unsigned char _errorID, SystemAddress trgClient){
	ErrorPack _err;
	
	_err.typeID = ID_ERROR;
	_err.ErrorID = _errorID;
	
	if(peer->Send((char*)&_err, sizeof(_err)+1, HIGH_PRIORITY, RELIABLE_ORDERED, 0, trgClient, 0))
		return 1;
	
	return 0;
}

void Network::proccessLoginPack(Packet *p){
	LoginPack *loginData = (LoginPack*)p->data;
	
	string username(loginData->loginName);
	string password(loginData->pwd_hash);

	userData *dbUserData = new userData();	
	
	char _temp[50];
	p->systemAddress.ToString(false, _temp);
	//cout<<"  * [L][Packet] "<<_temp<<"\n";

	dbUserData->clientID = p->systemAddress;
	memcpy(&_temp, "0", sizeof(_temp));
	dbUserData->clientID.ToString(false, _temp);
	//cout<<"  * [L][dbUserData] "<<_temp<<"\n";

	if(dbPtr->CheckUserLogin(username, password, dbUserData))
	{
		char uuid[4];
		strcpy(uuid, dbUserData->uuid.c_str());

		if(!userList->isUserOnline(uuid))
		{
			// user not logged in, log him in
			cout<<"  @ successfull login for: "<< username <<"\n";
			cout<<"  @ uuid: "<<dbUserData->uuid<<"\n";

			dbUserData->ChangeSinceLastSave		= false;
			dbUserData->savedInDb				= true;
			dbUserData->keepAlive				= true;
			dbUserData->DestructionPacketSent	= false;
		
			userList->addNode(dbUserData);			// Add user to list (Online)
			sendToAllInRange(uuid);					// inform all other online users for his presence
		
			// send the initial update packet
			if(sendInitPack(dbUserData))
			{
				packet->systemAddress.ToString(1, clientIp);
				cout<<"  * The init update packet has been sent to: "<<clientIp<<"\n\n";				
			}else{
				// if we failed to send the init packet,
				// delete the user from the list, and inform him of an error
				userList->removeUnitFromList(uuid);
				sendErrorPackToClient(ERROR_SENDING_INIT_UPDATE, p->systemAddress);
			}
		}else{
			// user is already logged in the server
			cout<<"  ! user "<<uuid<<" is already Online...\n";			
			sendErrorPackToClient(ALREADY_LOGGED_IN, p->systemAddress);
			delete dbUserData;
		}

	}else{
		cout<<"  ! wrong username 0r passw0rd\n";
		delete dbUserData;

		if(sendErrorPackToClient(WRONG_LOGIN_DATA, p->systemAddress))
			cout<<"  ! Error Login Packet has been send...\n";
	}
}

//proccess the Position packet, and update the user's position in
// the linked list
void Network::proccessPositionPack(Packet *p){
	tempPosPack = (PositionPack*)p->data;

	// Log
	p->systemAddress.ToString(1, clientIp);	// debug
	pktCnt++;								//debug
	unsigned long time_received;
	
	time_received = RakNet::GetTime();

	//std::cout<< tempPosPack->uuid<<" : "<< time_received << " : " <<clientIp<<" |x: "<<tempPosPack->position.x<<", z: "<<tempPosPack->position.z<<"  || "<<pktCnt<<"\n";
	// end of log msg
	
	// TODO: in case user is not Online
	// find the user and update his pozition
	userData *thisUser = userList->findUnit(tempPosPack->uuid);
	if(thisUser != NULL)
	{
		// temp del
		thisUser->userPos.x = tempPosPack->position.x;
		thisUser->userPos.y = tempPosPack->position.y;
		thisUser->userPos.z = tempPosPack->position.z;
		
		//thisUser->isMoving = true;
		//std::cout<<"estimated x: "<< thisUser->userPos.x<<"\n";

		thisUser->ChangeSinceLastSave	= true;			// since poz has changed...
		thisUser->savedInDb				= false;

		// send pozition to all in range
		//sendToAllInRange(tempPosPack->uuid, tempPosPack->position);
		relayPacketToAll(tempPosPack);
	}else{
		std::cout<<"  [poz Pack]: no such uuid online\n";
	}
}

void Network::proccessIDLEPacket(Packet *p){
	OnlyUuidPack *idlePack = (OnlyUuidPack*)p->data;

	// Log
	p->systemAddress.ToString(1, clientIp);	// debug
	pktCnt++;								//debug
	unsigned long time_received;
	
	time_received = RakNet::GetTime();

	//std::cout<< idlePack->uuid<<" : "<< time_received << " : " <<clientIp<<" is IDLE\n";
	// end of log msg

	userData *thisUser = userList->findUnit(idlePack->uuid);
	if(thisUser != NULL){
		// tell all the other users in the world that this user is idle
		sendIDLEtoAllInRange(idlePack->uuid);
		thisUser->isMoving = false;
	}
}

void Network::proccessOrientationPack(Packet *p){
	tempOrntPack = (OrientationPack*)p->data;

	// Log
	p->systemAddress.ToString(1, clientIp);	// debug
	pktCnt++;										//debug
	//std::cout<<tempOrntPack->uuid<<" "<<clientIp<<" |y rotation: "<< tempOrntPack->Orientation <<"  || "<<pktCnt<<"\n";
	// end of log msg

	// TODO: in case user is not Online
	// find the user and update the orientation
	userData *thisUser = userList->findUnit(tempOrntPack->uuid);
	if(thisUser != NULL)
	{
		thisUser->orientation = tempOrntPack->Orientation;
		//std::cout<<"[pop]:"<<thisUser->orientation<<"\n";

		thisUser->ChangeSinceLastSave = true;
		thisUser->savedInDb			  = false;
	
		// send Orientation to all within range
		//sendToAllInRange(tempOrntPack->uuid, tempOrntPack->Orientation);
		relayPacketToAll(tempOrntPack);
	}else{
		std::cout<<"  [Orn Pack]: no such uuid online\n";
	}
}

void Network::proccessCharDescriptionRequest(Packet *p){
	OnlyUuidPack *_tCdescReq = (OnlyUuidPack*)p->data;

	userData *requestedUser = userList->findUnit(_tCdescReq->uuid);
	if(requestedUser != NULL){
		// the user exists, send his description to the client that made the request
		sendUserDescriptionPack(requestedUser, p->systemAddress);
	}else{
		// the user doesn't exist in server's List, send error message
		sendErrorPackToClient(USER_NOT_ONLINE, p->systemAddress);
	}
}

void Network::proccessClientDisconnection(Packet *p){
	p->systemAddress.ToString(1, clientIp);	

	// since the user has disconnected, we don't need him in the list...
	userData *_tUser = userList->findUnit(p->systemAddress);
	if(_tUser != NULL)
		_tUser->keepAlive = false;
}

bool Network::terminateNetwork(){
	peer->Shutdown(500, 0);
	RakNetworkFactory::DestroyRakPeerInterface(peer);	
	return 1;
}

// sends the position of the current user (uuid) to all online users that are within range
void Network::sendToAllInRange(char uuid[UUID_LEN], myPosition pozition){

	listNode *trg_user = userList->findNode(uuid);
	listNode *current = trg_user->next;

	while(current != trg_user){
		userData *crntUserData = (userData*)current->data;
		sendPositionPack(uuid, pozition, crntUserData->clientID);
		
		current = current->next;
	}
}

void Network::sendToAllInRange(char uuid[UUID_LEN], float orientation){
	
	listNode *trg_user = userList->findNode(uuid);
	listNode *current = trg_user->next;

	while(current != trg_user){
		userData *crntUserData = (userData*)current->data;
		//std::cout<<"[stair]"<<crntUserData->orientation<<"\n";
		sendOrientationPack(uuid, orientation, crntUserData->clientID);
		
		current = current->next;
	}
}

// sends user description packet to all users in range (in linked list)...
void Network::sendToAllInRange(char uuid[UUID_LEN]){

	listNode *trg_user = userList->findNode(uuid);
	listNode *current = trg_user->next;

	while(current != trg_user){
		userData *crntUserData = (userData*)current->data;
		sendUserDescriptionPack(uuid, crntUserData->clientID);		
		current = current->next;
	}
}

void Network::relayPacketToAll(PositionPack *_p){
	listNode *trg_user = userList->findNode(_p->uuid);
	listNode *current = trg_user->next;

	while(current != trg_user){
		userData *crntUserData = (userData*)current->data;
		sendPositionPack(_p, crntUserData->clientID);
		
		current = current->next;
	}
}

void Network::relayPacketToAll(OrientationPack *_p){
	listNode *trg_user = userList->findNode(_p->uuid);
	listNode *current = trg_user->next;

	while(current != trg_user){
		userData *crntUserData = (userData*)current->data;
		sendOrientationPack(_p, crntUserData->clientID);
		
		current = current->next;
	}
}

void Network::sendIDLEtoAllInRange(char uuid[]){
	listNode *trg_user = userList->findNode(uuid);
	listNode *current = trg_user->next;

	while(current != trg_user){
		userData *crntUserData = (userData*)current->data;
		sendIDLEPacket(uuid, crntUserData->clientID);
		current = current->next;
	}
}

// send all the user descriptions in the list to the target user...
void Network::SendInitUserDescriptions(SystemAddress targetClient){
	listNode *trgUser = userList->findNode(targetClient);
	listNode *current = trgUser->next;
	
	while(current != trgUser){
		// get current user data
		userData *currentUserData = (userData*)current->data;
		// end send the description packet to the target user
		sendUserDescriptionPack(currentUserData, targetClient);

		// get the next user in the list
		current = current->next;
	}
}

// sends a user destruction packet, for user uuid, to all users in range
void Network::SendUserDestructionToAll(char uuid[UUID_LEN]){

	listNode *trg_user = userList->findNode(uuid);
	listNode *current = trg_user->next;

	while(current != trg_user){
		userData *crntUserData = (userData*)current->data;
		if(crntUserData->keepAlive)
		{
			bool rsl = sendUserDestructionPack(uuid, crntUserData->clientID);
		
			if(rsl)
			{			
				crntUserData->clientID.ToString(1, clientIp);	// debug
				std::cout<<"  * "<<uuid<<" destruction packet has been sent to "<< clientIp <<"\n";		
			}
		}
		current = current->next;
	}	
}	

// checks in the list who is alive, and for those who are not, sends a destroyUserPacket to all the rest clients
void Network::CheckWhoIsAlive(){

	listNode *current = userList->Head;

	// list is not empty...
	if(current != NULL)
	{
		do{
			userData *currentUser = userList->getUnitData(current);
			if(!currentUser->keepAlive && !currentUser->DestructionPacketSent)
			{
				char uuid[4];
				strcpy(uuid, currentUser->uuid.c_str());
				SendUserDestructionToAll(uuid);
				currentUser->DestructionPacketSent = true;				
			}

			current = current->next;
		}while(current != userList->Head);
	}
}