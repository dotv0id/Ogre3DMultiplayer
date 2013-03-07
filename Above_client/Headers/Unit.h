/* ------------------------------------------------------------------------------------ *
 * Unit class																			*
 * Filename: Unit.h																		*
 *																						*
 * ------------------------------------------------------------------------------------ *
 * Description:																			*
 * ------------------------------------------------------------------------------------ *
 *																						*
 * Creates a linked list of server-depended Units (3rd user characters, npc, etc)		*
 * This class manages the movable & server-dependable units, such as:					*
 * The other user characters (players), NPC characters [Non Playable Characters],		*
 * creatures (aka mobs).																*
 *																						*
 * As input it uses pointers to structs that contain the data of the character.			*
 *																						*
 * ------------------------------------------------------------------------------------ *
 */

#ifndef _UNIT_H
#define _UNIT_H

#include "Ogre.h"
#include "MovableText.h"

using namespace Ogre;

class Unit{
private:
	Ogre::String	username;
	Ogre::String	modelName;

	MovableText		*UnitCaption;
	SceneNode		*CaptionNode;
	
	Vector3			charCurrentPos, NextPosition;
	Vector3			smoothPoz, lastPz;
	
	Real			moveSpeed;

	// animation speeds
	Real animMoveSpeed, animIdleSpeed;
	
	Entity			*charEnt;
	SceneNode		*charNode;
	SceneManager	*mySmPtr;

	AnimationState *mAnimationState;

	bool changeSinceLastRender;		// chech if this unit has been updated since it was last rendered
	
	// collision Detection
	RaySceneQuery *mRaySceneQuery;

	unsigned int IDLE_after;		// in millisecond: set idle if IDLE_after has passed since last poz/or packet

public:
	std::string		uuid;
	bool			KeepUnitAlive;
	bool			isMoving;

	unsigned long	last_poz_pack_received;
	unsigned long	last_ornt_pack_received;

	// public functions
public:
	Unit(std::string id, std::string uname, std::string modelname, Vector3 initPoz, SceneManager *mgr);
	~Unit();

	void RenderMe(Real elpTime);
	bool checkIfMoving();
	bool checkIfShouldBeIDLE(unsigned long currentTime);
	
	// set functions
	void setMoveSpeed(Real ms){moveSpeed = ms; animMoveSpeed = ms/10;};
	void setPosition(Vector3 newPoz){NextPosition = newPoz;};	
	
	void setCharcaterPosition(Vector3 newPos);
	void setCharacterOrientation(float rotY);

	void setCharAnimation(String AnimName, Real elapsedTime, bool AnimState, float AnimSpeed, bool loop);
	
	// get functions
	std::string getUUID(){return uuid;};

};

#endif

