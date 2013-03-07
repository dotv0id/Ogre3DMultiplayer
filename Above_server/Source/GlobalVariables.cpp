#include "GlobalVariables.h"

// initialize the Global Variables
GlobalVariables::GlobalVariables(){
/*
	// Network
	MaxConnections	= 100;					// max users connected to server simultaneously
	ServerPort		= 46000;				// default server port

	// Database
	db				= "above_mmo";			// schema
	tableName		= "above_users";
	dbServerIP		= "192.168.1.4";
	dbServerIP		= "127.0.0.1";
	dbServerPort	= 3306;
	dbUser			= "v0id";
	dbPwd			= "n4f4";

	// Intervals & Rates
	userBackupInterval	= 20*MILLISEC;		// in second	[every 20 sec]
	
	TickInterval			= 30;			// 33 ticks per second
	ThinkInterval			= 40;			// 25 times per second
*/
}

void GlobalVariables::removeCharacterFromString(std::string *_str, char *rmv){
	int i = _str->find(rmv);

	while(i > -1){
		_str->replace(i, 1, "");			
		i = _str->find("\t");
	}
}

bool GlobalVariables::loadConfigFile(char config[128]){
	ifstream cf(config, ios::in);

	if(!cf){
		cout<<"  ! Error while opening "<<config<<"\n";
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
								
				if(_name == "server_Port")
					ServerPort = atoi(_value.c_str());

				if(_name == "max_connections")
					MaxConnections = atoi(_value.c_str());

				if(_name == "tick_interval")
					TickInterval = atoi(_value.c_str());

				if(_name == "think_interval")
					ThinkInterval = atoi(_value.c_str());

				if(_name == "user_backup_interval")
					userBackupInterval = atoi(_value.c_str());

				if(_name == "mysql_server_IP")
					strcpy(dbServerIP, _value.c_str());

				if(_name == "mysql_server_port")
					dbServerPort = atoi(_value.c_str());

				if(_name == "mysql_username")
					strcpy(dbUser, _value.c_str());

				if(_name == "mysql_password")
					strcpy(dbPwd, _value.c_str());

				if(_name == "mysql_schema")
					strcpy(db, _value.c_str());

				if(_name == "mysql_table_name")
					strcpy(tableName, _value.c_str());
			}
		}
	}

	return true;
}

GlobalVariables::~GlobalVariables(){
}