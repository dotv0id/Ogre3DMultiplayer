/* ------------------------------------------------------------------------------------ *
 * Global Variables class																*
 * Filename: globalVars.h																*
 *																						*
 * ------------------------------------------------------------------------------------ *
 * Description:																			*
 * ------------------------------------------------------------------------------------ *
 *	...																					*
 *																						*
 * ------------------------------------------------------------------------------------ *
 */

#ifndef _GLOBALVARS_H
#define _GLOBALVARS_H

#include "Ogre.h"
#include <fstream>

using namespace Ogre;

// This class contains all the global variables for the game
// all other classes, that use those vars, contain a pointer to this class
// so the can access the global variables
class GlobalVariables{

// public variables (supposedly not good in O.O. design)
public:
	// userChar
	Real moveSpeed, mRotateSpeed, zoomSpeed;
	Real walkAnimSpeed, idleAnimSpeed, initSpeed;
	int zoomLimitClose, zoomLimitFar;

	// Network Related Variabless	
	int		client_port;
	int		server_port;
	char	server_IP[32];

	// if true, then the server has send us the initial data, we need to create our
	// character, so core must proceed by creating the user character with those data	
	Vector3 initPosition;

	// Flow & checking Variables
	int poPackInterval;			// milliseconds: how often to send packages of (p)osition and (o)rientation 

	int iu_timeout;				// How long to wait for the server to send the initial update packet
	int elapsed_iu;				// How much time has passed since we started waiting for the initial paxket
	
	bool init_char_update;		// The initiali character update packet has arrived
	bool mainCharExists;		// user's main character exists (this is used by framelistener & timercallback)
	bool createWorld;
	bool createMainCharacter;
	bool UnitListExists;		// True if the Circular Double Linked List for 3rd Users has been created

	// Scene manager & camera pointers
	SceneManager *sManagerPtr;
	Camera *mCamPtr;

	// debug
	int pktCnt;

// Public Functions
public:	
	GlobalVariables();
	~GlobalVariables();

	bool loadConfigFile(char config[128]);
	void removeCharacterFromString(std::string *_str, char *rmv);
	
	void setSceneManagerPtr(SceneManager *sm){sManagerPtr = sm;};
	void setCameraPtr(Camera *c){mCamPtr = c;};

	SceneManager *getSceneManager(){return sManagerPtr;};
	Camera *getCamera(){return mCamPtr;};
};

#endif

