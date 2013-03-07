#ifndef _GLOBALVARIABLES_H
#define _GLOBALVARIABLES_H

#include "DataTypes.h"
//#include <string.h>
#include <iostream>
#include <fstream>

using namespace std;

#define MILLISEC 1000;

class GlobalVariables{

public:

	// Network
	unsigned int	MaxConnections;
	unsigned int	ServerPort;
	const char		*LocalIP;

	// Database	
	char			dbServerIP[32];
	unsigned int	dbServerPort;
	char			db[64];
	char			tableName[64];
	char			dbUser[64];
	char			dbPwd[64];	

	// Intervals & rates
	int userBackupInterval;		// Every x millisecond, save the users' position in the database
	int SnapshotInterval;			// 
	int TickInterval;				// every x millisec
	int ThinkInterval;
	int worldBackupInterval;		// how frequent to save the world state

	int TimeSinceLastBackup;


public:
	GlobalVariables();
	~GlobalVariables();
	
	bool loadConfigFile(char config[128]);
	void removeCharacterFromString(std::string *_str, char *rmv);
};

#endif

