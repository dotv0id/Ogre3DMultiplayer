#include "core.h"

Core::Core(){	
	// Initialize Core
	Net				= new Network();
	dBase			= new Database();
	userLinkedList	= new LinkedList();
	
	Net->setLinkedListPtr(userLinkedList);
	Net->setDatabasePtr(dBase);

	// Set Network down
	NetworkState = false;	
}

Core::~Core(){
}

void Core::go(){

	// 0. read config file
	globalVar->loadConfigFile("server.cfg");

	// 1. Initialize the Database
	if(dBase->initializeDatabase())
		cout<<"  * Connected to database: "<<globalVar->db<<", as user: "<<globalVar->dbUser<<"\n";
	else
		cout<<"  * Error: Could not connect to the database \n";

	// 2. Initialize the network
	if(NetworkState = Net->initializeNetwork())
		cout<<"  * server initialized on: "<<globalVar->LocalIP<<":"<<globalVar->ServerPort<<" [UDP]\n";
	else
		cout<<"  * Error: server could not be initialized\n";

	// set control variables
	lastTimeExec = lastThink =lastTick = RakNet::GetTime();

	// 3. Start the server Loop
	startLoop();
}

void Core::startLoop(){
	cout<<"  - main loop started. Hit 'ESC' to exit...\n\n";	

	while(true){
		// Do if Network Up...
		if(NetworkState)
		{
			// Proccess packets
			Net->readPacket();

			// # 1. server think rate...
			// the rate the server "Thinks" [proccesses information from clients, and updates them accordingly]
			if(RakNet::GetTime()> lastThink + globalVar->ThinkInterval)
			{
				Net->CheckWhoIsAlive();
				lastThink = RakNet::GetTime();
			}

			// # 2. server tick rate (the rate with which the server sends packets (position / orientation))

			// # 3. Save user pozitions/orientations to Database every [userPosBackupInterval seconds][eg. every 10 seconds]
			if(RakNet::GetTime()> lastTimeExec + globalVar->userBackupInterval)
			{
				userLinkedList->SaveUsersToDb(dBase);			
				lastTimeExec = RakNet::GetTime();
			}
		}

		// if ESC is pressed, exit the server
		if(_kbhit() && (getch() == 27))
			break;
	}
}

void Core::terminate(){
	delete userLinkedList;
	delete dBase;
	
	NetworkState = false;
	Net->terminateNetwork();

	delete Net;	
}

void Core::timerCoreFunction(){
	
}