#ifndef _CORE_H
#define _CORE_H

#include <windows.h>

#include <conio.h>
#include <math.h>

#include "OgreMath.h"
#include "OgreVector3.h"

#include "Network.h"
#include "Database.h"

class Core{

private:
	GlobalVariables		*globalVar;
	Network				*Net;
	Database			*dBase;
	LinkedList			*userLinkedList;

	bool NetworkState;
	
	// controll Vars
	RakNetTime lastTimeExec, lastTick, lastThink;
	
// private funcitons
private:

public:
	Core();
	~Core();

	void go();
			
	void startLoop();
	void terminate();

	bool getNetworkState(){return NetworkState;};
	void timerCoreFunction();	

	// get functions
	Database*	getDatabasePtr(){return dBase;};
	Network*	getNetworkPtr(){return Net;};
	LinkedList*	getUserLinkedListPtr(){return userLinkedList;};

	// set functions
	void setGlobalVariablesPtr(GlobalVariables *g){globalVar = g;};
};

#endif

