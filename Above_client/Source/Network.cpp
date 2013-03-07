/* ------------------------------------------------------------------------------------ *
 * Impementation of the Network class													*
 * Filename: Network.cpp																*
 * ------------------------------------------------------------------------------------ *
 */


#include "Network.h"
#include "RakSleep.h"


Network::Network(){
	networkState = false;
	connected = false;
	
	idleWasSent = false;
}

Network::~Network(){
}

// Initialize the network class, and connect to server
bool Network::initializeNetwork(){
	peer = RakNetworkFactory::GetRakPeerInterface();

	networkState = peer->Startup(1, n_glVar->poPackInterval, &SocketDescriptor(n_glVar->client_port, peer->GetLocalIP(0), false), 1);
	return networkState;
}

bool Network::connectToServer(const char *serverIP, int serverPort){
	printErrorMessage("Connecting to the server...");

	serverSAd.SetBinaryAddress(n_glVar->server_IP);
	serverSAd.port = n_glVar->server_port;

	bool rslt = peer->Connect(serverIP, serverPort, 0, 0);

	if(rslt){
		std::cout<<"\n!   Connected to: "<<n_glVar->server_IP<<":"<<n_glVar->server_port<<"\n";
		connected = true;		
	}else{
		std::cout<<"\n!   Error Connecting to server...\n";
		connected = false;
	}

	return connected;
}

void Network::disconnectFromServer(bool sendNotification){
	peer->CloseConnection(serverSAd, sendNotification, 0);
	std::cout<<"disconnected...\n";

	connected = false;
}

// Read packets from the server, and call processPacket function to process the packet data
void Network::readPacket(){
		
	packet = peer->Receive();

	while(packet){
		switch (packet->data[0]){
		case ID_CONNECTION_ATTEMPT_FAILED:
			// Wrong IP/port combination
			std::cout<<"Could not connect to server...\n";
			printErrorMessage("Error Connecting to the server...");
			disconnectFromServer(false);

			break;
		case ID_REMOTE_DISCONNECTION_NOTIFICATION:
			printf("Another client has disconnected.\n");
			break;

		case ID_REMOTE_CONNECTION_LOST:
			printf("Another client has lost the connection.\n");
			break;

		case ID_REMOTE_NEW_INCOMING_CONNECTION:
			printf("Another client has connected.\n");
			break;

		case ID_CONNECTION_REQUEST_ACCEPTED:
			printf("Our connection request has been accepted.\n");
			// send the login package
			sendLoginPacket();
			break;

		case ID_NEW_INCOMING_CONNECTION:
			printf("A connection is incoming.\n");
			break;

		case ID_NO_FREE_INCOMING_CONNECTIONS:
			printf("The server is full.\n");
			break;

		case ID_DISCONNECTION_NOTIFICATION:
			printf("We have been disconnected.\n");
			break;

		case ID_CONNECTION_LOST:
			printf("Connection lost.\n");
			break;

		case ID_INIT_UPDATE:
			// The Server just send us the initial Update Packet,
			// which contains our character's last saved position
			proccessInitUpdate(packet);
			std::cout<<"\n%>   Initial Update Packet received & proccessed...\n";
			break;

		case ID_ERROR:
			proccessErrorPacket(packet);
			break;

		case ID_CHAR_DESCRIPTION:
			// The server just send us a packet containing character description
			proccessCharDescription(packet);			
			break;

		case ID_POSITION_PACK:
			proccessPositionPacket(packet);
			break;

		case ID_ORIENTATION_PACK:
			proccessOrientationPacket(packet);
			break;

		case ID_IDLE:
			proccessIDLEPacket(packet);
			break;

		case ID_DESTROY_USER:
			std::cout<<"Destroy User Packet Received...\n";
			proccessUserDestructionpacket(packet);
			break;

		default:
			// Unknown packet
			printf("Message with identifier %i has arrived.\n", packet->data[0]);
			break;
		}
		peer->DeallocatePacket(packet);
		packet = peer->Receive();
	}	
}

/* Send the characters position to the server, 
 * this function is called every [xx] milliseconds, checks if the position of the user
 * character has changed, and if yes, it sends it to the server */
bool Network::sendPosition(Vector3 currentPosition){
	TempPosPack = new PositionPack();						// allocate memory for the TEMPorary Position Orientation PACKet	
	TempPosPack->typeID = ID_POSITION_PACK;				// Set the packet's type as position orientation packet

	strcpy(TempPosPack->uuid, mUserChar->uuid);

	TempPosPack->position.x = currentPosition.x;	// Set the position value...
	TempPosPack->position.y = currentPosition.y;
	TempPosPack->position.z = currentPosition.z;

	bool rslt = peer->Send((char*)TempPosPack, sizeof(*TempPosPack)+1, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0, serverSAd, 0);
		
	n_glVar->pktCnt++;	// debug... increase packets send by one

	delete TempPosPack;
	
	idleWasSent = false;

	return rslt;
}

bool Network::sendIDLE(){
	OnlyUuidPack *idlePack = new OnlyUuidPack();

	idlePack->typeID = ID_IDLE;
	strcpy(idlePack->uuid, mUserChar->uuid);

	bool result = peer->Send((char*)idlePack, sizeof(*idlePack)+1, HIGH_PRIORITY, RELIABLE_SEQUENCED,0, serverSAd, 0);
	
	idleWasSent = true;
	
	delete idlePack;

	return result;
}

bool Network::sendOrientation(float yRot){
	OrientationPack *currentOrPack = new OrientationPack();
	
	currentOrPack->typeID = ID_ORIENTATION_PACK;
	strcpy(currentOrPack->uuid, mUserChar->uuid);
	currentOrPack->Orientation = yRot;

	bool rslt = peer->Send((char*)currentOrPack, sizeof(*currentOrPack)+1, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0, serverSAd, 0);

	n_glVar->pktCnt++;	
	idleWasSent = false;
	
	delete currentOrPack;

	return rslt;
}

void Network::setupLoginPacket(std::string username, std::string password){
	loginData = new LoginPack();

	loginData->typeID = ID_LOGIN;
	strcpy(loginData->loginName, username.c_str());
	strcpy(loginData->pwd_hash, password.c_str());

	//delete the pack after sending
}

void Network::sendLoginPacket(){

	bool rsld = peer->Send((char*)loginData, sizeof(*loginData)+1, HIGH_PRIORITY, RELIABLE_ORDERED, 0, serverSAd, 0);

	std::cout<<"login Packet Send...\n";
	delete loginData;
}

/* Request character Description Packet. If ID is ID_INIT_CHAR_DESCRIPTION, send the initial characters
 * (all the characters in the world who are near my character)
 */
bool Network::RequestCharDescription(char uuid[UUID_LEN]){

	OnlyUuidPack tempCharDescPack;

	tempCharDescPack.typeID = ID_CHAR_DESCRIPTION;
	strcpy(tempCharDescPack.uuid, uuid);
	
	return peer->Send((char*)&tempCharDescPack, sizeof(tempCharDescPack)+1, HIGH_PRIORITY, RELIABLE_ORDERED, 0, serverSAd, 0);	
}

// Request the init char descriptions to be send to this client
// (all the users in the database that are in range of my user)...
bool Network::RequestCharDescription(){
	
	OnlyUuidPack tempCharDescPack;
	tempCharDescPack.typeID = ID_INIT_CHAR_DESCRIPTION;
	
	return peer->Send((char*)&tempCharDescPack, sizeof(tempCharDescPack)+1, HIGH_PRIORITY, RELIABLE_ORDERED, 0, serverSAd, 0);
}

void Network::proccessInitUpdate(Packet *p){
	// Hide login Dialog & disable server ip/port user input
	CEGUI::WindowManager *wmgr		= CEGUI::WindowManager::getSingletonPtr();
	CEGUI::Window *server_ip_inp	= wmgr->getWindow((CEGUI::utf8*)"system/serverIp");
	CEGUI::Window *server_port_inp	= wmgr->getWindow((CEGUI::utf8*)"system/serverPort");	
	CEGUI::Window *loginDialog		= wmgr->getWindow((CEGUI::utf8*)"loginDlg");
	CEGUI::Window *dbg_win			= wmgr->getWindow((CEGUI::utf8*)"messageDlg");
	
	server_ip_inp->setEnabled(false);	// disable the ip input box
	server_port_inp->setEnabled(false);	// disable the port input box
	loginDialog->hide();				// Hide the login dialog	
	dbg_win->hide();

	// proccess init update packet

	initUpdate *iu = (initUpdate*)p->data;							// cast the data in the packet to the iuUpdate struct

	std::cout<<"\n%>   My init position x: "<<iu->position.x<<", z: "<<iu->position.z<<" | "; // debug message
	std::cout<<sizeof(*iu)<<" | "<<p->length<<" bytes\n";

	// setup the initial position vector, for Ogre
	Vector3 _tPoz;
	_tPoz.x = iu->position.x;
	_tPoz.y = iu->position.y;
	_tPoz.z = iu->position.z;
	
	// get the strings...
	std::string uuid(iu->uuid);
	std::string uName(iu->username);
	std::string model(iu->modelName);	

	// Start the game...
	// 1. Create the game world
	gWorldPtr->createWorld();
	std::cout<<"world has been created\n";

	// 2. Create the user's main character
	mUserChar = new userCharacter(uName, model, n_glVar->getSceneManager(), n_glVar->getCamera(), _tPoz, iu->orientation);
	mUserChar->setGlobalVariablesPtr(n_glVar);
	strcpy(mUserChar->uuid, iu->uuid);
	
	n_glVar->mainCharExists = true;						// the main character has been created
	std::cout<<"---* The user's Character has been created...\n";		
	
	n_glVar->init_char_update = true;	
	
	// 3. Send Initial Character description request to the server	
	RequestCharDescription();

	std::cout<<"request char description send\n";	
}

void Network::proccessCharDescription(Packet *p){

	CharDescPack *cdpack;
	cdpack = (CharDescPack*)p->data;				// cast the data in the packet to the CharDescPack struct	

	std::cout<<"sizeof cdpack: "<<sizeof(*cdpack)<<"\n";
	std::cout<<"sizeof packet: "<<sizeof(*p)<<"\n";
		
	std::string username(cdpack->username);
	std::string modelname(cdpack->modelName);
	std::string uuid(cdpack->uuid);

	Vector3 tpoz;
	tpoz.x = cdpack->position.x;
	tpoz.y = cdpack->position.y;
	tpoz.z = cdpack->position.z;

	nUserUnit->newUserUnit(uuid, username, modelname, tpoz, cdpack->orientation);						// Pass the cdpointer to the newUserUnit function,
}

void Network::proccessUserDestructionpacket(Packet *p){
	OnlyUuidPack *_destro = (OnlyUuidPack*)p->data;

	std::string uuid(_destro->uuid);
	nUserUnit->deleteUserUnit(uuid);
}

void Network::proccessErrorPacket(Packet *p){
	ErrorPack *_err = (ErrorPack*)p->data;

	switch(_err->ErrorID)
	{
		case WRONG_LOGIN_DATA:
			printErrorMessage("Wrong username or password...");
			disconnectFromServer(true);
			break;
		case ALREADY_LOGGED_IN:
			printErrorMessage("This user is already online...");
			disconnectFromServer(true);
			break;
	}
}

// pozition packet from server... it is the pozition of another user, that is in range of mine...
void Network::proccessPositionPacket(Packet *p){
	PositionPack *posPack = (PositionPack*)p->data;

	Unit *crntUser = nUserUnit->findUnit(posPack->uuid);
	
	if(crntUser != NULL)
	{
		// This user exists in my client's list
		// update hiz position
		Vector3 newPosition(posPack->position.x, posPack->position.y, posPack->position.z);
		crntUser->setCharcaterPosition(newPosition);
		crntUser->last_poz_pack_received = RakNet::GetTime();
		crntUser->isMoving = true;
	}else{
		// the user doesn't exist in my list,
		// request a character Description packet
		std::cout<<"no such user\n";
		RequestCharDescription(posPack->uuid);
	}		
}

void Network::proccessOrientationPacket(Packet *p){
	OrientationPack *orntPack = (OrientationPack*)p->data;

	Unit *crntUser = nUserUnit->findUnit(orntPack->uuid);

	if(crntUser != NULL)
	{
		// this user exists in my client's list
		// update his orientation
		crntUser->setCharacterOrientation(orntPack->Orientation);
		crntUser->last_ornt_pack_received = RakNet::GetTime();
		crntUser->isMoving = true;
	}else{
		std::cout<<"no such user\n";
		RequestCharDescription(orntPack->uuid);
	}
}

void Network::proccessIDLEPacket(Packet *p){
	OnlyUuidPack *idlePack = (OnlyUuidPack*)p->data;

	Unit *crntUser = nUserUnit->findUnit(idlePack->uuid);

	if(crntUser != NULL)
	{
		// set this user to IDLE state
		crntUser->isMoving = false;
	}else{
		std::cout<<"no such user\n";
		RequestCharDescription(idlePack->uuid);
	}
}

void Network::printErrorMessage(const char *_errMsg){
	CEGUI::WindowManager *wmgr = CEGUI::WindowManager::getSingletonPtr();
	CEGUI::Window *_erDlg = wmgr->getWindow((CEGUI::utf8*)"messageDlg/message");

	CEGUI::String msg(_errMsg);

	_erDlg->setText(msg);
}

bool Network::terminateNetwork(){
	peer->Shutdown(500, 0);
	RakNetworkFactory::DestroyRakPeerInterface(peer);

	return 1;
}

