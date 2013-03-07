/* ------------------------------------------------------------------------------------ *
 * Impementation of the GlobalVariables class											*
 * Filename: globalVars.cpp																*
 * ------------------------------------------------------------------------------------ *
 */


#include "globalVars.h"


GlobalVariables::GlobalVariables(){
	
	mRotateSpeed = 0.22;			// Camera Rotate speed	
	zoomSpeed = 0.4;				// Zoom speed of the camera
	zoomLimitClose = 220;			// How close to the user Character can the camera go
	zoomLimitFar = 2000;			// How far from the user Characrer can tha camera go

	moveSpeed = 20;					// User's character move speed, it can be affected by the use of potions/buffs
	walkAnimSpeed = moveSpeed/10;	// User Character Animation Speeds
	idleAnimSpeed = 0.5;			// Other Characters in the world, can have different speeds, 
	initSpeed = 1;					// due to the use of Potions/buffs...

	// Set the network related variables
	//server_IP = "000.000.000.000";
	
	//server_port = 46000;			// UDP port ...
	//client_port = 46001;			// UDP port ...

	// Set position and orientation packet interval to ... in milliseconds (50ms -> 20 packs per sec.)
	poPackInterval = 33;			// 25 packs per second

	// Has the initial update packet arrived?
	// if yes, then create the user's character and the game world...
	init_char_update = false;
	mainCharExists = false;			// Has the user Character been created?

	createWorld = false;
	createMainCharacter = false;
	UnitListExists = false;

	iu_timeout = 5;					// in Seconds
	elapsed_iu = 0;
 
	// debug
	pktCnt = 0;
}

void GlobalVariables::removeCharacterFromString(std::string *_str, char *rmv){
	int i = _str->find(rmv);

	while(i > -1){
		_str->replace(i, 1, "");			
		i = _str->find("\t");
	}
}

bool GlobalVariables::loadConfigFile(char config[128]){
	std::ifstream cf(config, std::ios::in);

	if(!cf){
		std::cout<<"  ! Error while opening "<<config<<"\n";
		return false;
	}
	
	// while not end of file...
	while(!cf.eof()){
		// read and proccess line
		char buffer[256];
		cf.getline(buffer, sizeof(buffer));

		if(buffer[0] != char(35))
		{
			std::string inp(buffer);

			// remove whitespace from input line
			removeCharacterFromString(&inp, "\t");
			removeCharacterFromString(&inp, " ");

			// find '=' and split the string in name & value			
			int i = inp.find("=");
			std::string _name, _value;
			
			// match _name with global variables, and set values			
			if(i > -1)
			{
				_name = inp.substr(0, i);
				_value = inp.substr(i+1, inp.length());
								
				if(_name == "Server_IP")
					strcpy(server_IP, _value.c_str());

				if(_name == "Server_port")
					server_port = atoi(_value.c_str());

				if(_name == "client_port")
					client_port = atoi(_value.c_str());
			}
		}
	}

	return true;
}

GlobalVariables::~GlobalVariables(){
}