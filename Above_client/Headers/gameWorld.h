/* ------------------------------------------------------------------------------------ *
 * GameWorld class																		*
 * Filename: gameWorld.h																*
 *																						*
 * ------------------------------------------------------------------------------------ *
 * Description:																			*
 * ------------------------------------------------------------------------------------ *
 * This Class contains the Elements that construct the world of the game.				*
 * It creates the classes: SceneLoader (loads the scene::TODO),							*
 * userCharracter (creates and updates the users character), thirdCharacters			*
 * (it creates and updates the characters of the other users, based on server info).	*
 *																						*
 * The pointer to this class is being passed, through the core class, to the classes,	*
 * that change the elements of the game world, such as CFrameListener, and Network		*
 * classes...																			*
 *																						*
 * ------------------------------------------------------------------------------------ *
 */


#ifndef _GAMEWORLD_H
#define _GAMEWORLD_H

#include "globalVars.h"

class GameWorld{
private:
	GlobalVariables		*glVars;
	SceneManager		*mgr;	
	

public:
	GameWorld(SceneManager *m);
	~GameWorld();

	void createWorld();
	void setGlobalVarsPtr(GlobalVariables *_gVars){glVars = _gVars;};

};

#endif

