/* ------------------------------------------------------------------------------------ *
 * User Character class																	*
 * Filename: userCharacter.h															*
 *																						*
 * ------------------------------------------------------------------------------------ *
 * Description:																			*
 * ------------------------------------------------------------------------------------ *
 * Creates the user's character, and updates him when through user, or server input.	*
 * This Class creates ONLY the user's character.										*
 *																						*
 * ------------------------------------------------------------------------------------ *
 */

#ifndef _USERCHARACTER_H
#define _USERCHARACTER_H

#include <OIS/OIS.h>
#include <OgreStringConverter.h>
#include "globalVars.h"

#include "MovableText.h"

#include <CEGUI/CEGUI.h>
#include <CEGUI/CEGUISystem.h>

class userCharacter{

private:
	// globalVariable class pointer
	GlobalVariables *glVar;
	
	String mName;
	String modelName;

	// Scene Nodes needed for character and camera
//	SceneNode	*mCharNode;				// The character Node: The character's model lives here... (Bob Ross :D)
	SceneNode	*mCameraNode;			// Put the camera here
	SceneNode	*cameraTargetNode;		// The camera will look here
	SceneNode	*cameraPitchNode;		// Camera's target rotation ONLY around X axis
	
	MovableText	*CharCaption;
	SceneNode	*CaptionNode;			// Will hold the username (above character's node)

	Vector3		camPoz;
	Vector3		initFacing;

	Vector3		charCurrentPos;			// Current position of the character
	Vector3		charLastPos;			// Last position of the character

	Quaternion	charCurrentOrientation;
	Quaternion	charLastOrientation;

	float		currentOrientation_Y;
	float		lastOrientation_Y;

	Quaternion alpha;

	Entity			*mEnt;				// The Character's model...
	SceneManager	*mSceneMgr;
	Camera			*mCamera;

	RaySceneQuery	*mRaySceneQuery;

	AnimationState *mAnimationState;
	
	bool WisDown, SisDown, AisDown, DisDown, ooStealth, JumpP;

	Real NextCamZpos, elpTime;
	
	// actions
	bool jump, sidekick, attack;

	SceneNode *test;//??	

public:
	// temp
	SceneNode	*mCharNode;				// The character Node: The character's model lives here... (Bob Ross :D)

	char uuid[4];
	bool moving, allowMotion;

// public functions
public:
	userCharacter(Ogre::String name, Ogre::String model, Ogre::SceneManager *sn, Ogre::Camera* cam, Vector3 initPosition, float initOrientation);
	~userCharacter();

	// Functions of userCharacter class
	Vector3 getCurrentPosition();						// Returns the current position of the character
	Quaternion getCurrentOrientation();
	
	bool CheckPosition();							// Returns True if the position has changed
	bool CheckOrientation();						// Returns true if the character's orientation chas changed

	// Update the character's position & orientation in the world (input from user / not network)
	void update(Real elpTime);
	void updateCamera(const OIS::MouseEvent &arg); // Update the camera (input user|mouse)
	void zoomCamera(const OIS::MouseEvent &arg);
	void resetCamera();								// Place camera to initial pozition
	void setCharAnimation(String AnimName, Real elapsedTime, bool AnimState, float AnimSpeed, bool loop);

	// Variable setting Functions
	void setGlobalVariablesPtr(GlobalVariables *gv){glVar = gv;};

	void setW_state(bool state){WisDown = state;};
	void setS_state(bool state){SisDown = state;};
	void setA_state(bool state){AisDown = state;};
	void setD_state(bool state){DisDown = state;};
	void setStealth_state(bool state){ooStealth = state;};
		
	bool getStealth_state(){return ooStealth;};
	bool getAttack_state(){return attack;}
	
	void setMoveSpeed(Real speed){glVar->moveSpeed = speed;};

	void charSetActionState(int actionName, bool state);

	void showBoundingBoxes(bool state);
};

enum actions{
	ATTA = 0, JUMP, SIDE
};
#endif


