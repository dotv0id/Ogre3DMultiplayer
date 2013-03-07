/* ------------------------------------------------------------------------------------ *
 * Impementation of the userCharacter class												*
 * Filename: userCharacter.cpp															*
 * ------------------------------------------------------------------------------------ *
 */


#include "userCharacter.h"

userCharacter::userCharacter(Ogre::String name, Ogre::String model, Ogre::SceneManager *sn, Ogre::Camera* cam, Vector3 initPosition, float initOrientation){

	mName = name;
	mSceneMgr = sn;
	mCamera = cam;
	modelName = model; modelName.append(".mesh");

	// Initialize Position & Orientation to 0 [exception safe...]
	charLastPos = charCurrentPos = Vector3(0,0,0);
	charCurrentOrientation = charLastOrientation = Ogre::Quaternion::ZERO;

	charLastPos = charCurrentPos = initPosition;			// initially current position and last position are the same
	
	Radian rot = Degree(initOrientation);
	charCurrentOrientation = charLastOrientation = Quaternion(rot, Vector3::UNIT_Y);
	
	WisDown = SisDown = AisDown = DisDown = false;
	jump = sidekick = attack = false;

	initFacing = Vector3(0, 0, -1);						// facing out front

	mEnt=mSceneMgr->createEntity(mName, "ninja.mesh");
	mEnt->setCastShadows(true);

	mCharNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(mName);
	cameraTargetNode = mCharNode->createChildSceneNode("cameraTargetNode");
	cameraPitchNode = cameraTargetNode->createChildSceneNode("cameraPitchNode");
	mCameraNode = cameraPitchNode->createChildSceneNode("cameraNode");
	
	mCharNode->attachObject(mEnt);
	mCharNode->scale(0.05, 0.05, 0.05);					//0.05 ninja
	mCharNode->setPosition(charCurrentPos);
	mCharNode->setOrientation(charCurrentOrientation);
	
	cameraTargetNode->setPosition(Vector3(0, 165, 0));	//170
	cameraTargetNode->scale(Vector3(0.5, 0.5, 0.5));

	mCameraNode->setPosition(Vector3(0, 0, 650));		// 650
	mCameraNode->attachObject(mCamera);
	mCameraNode->setOrientation(mCharNode->getOrientation());

	mCameraNode->setAutoTracking(true, cameraTargetNode);
	mCameraNode->setFixedYawAxis(true);

	CharCaption = new MovableText("caption", mName);
	CharCaption->setTextAlignment(MovableText::H_CENTER, MovableText::V_ABOVE);
	CharCaption->setCharacterHeight(20);
	
	String __cap = "caption_";__cap.append(mName);
	CaptionNode = mCharNode->createChildSceneNode(__cap);
	CaptionNode->setPosition(Vector3(0, 210, -15));
	//CaptionNode->rotate(90);
	CaptionNode->attachObject(CharCaption);

	mRaySceneQuery = mSceneMgr->createRayQuery(Ray());
	std::cout<<"\ncharacter was created at: x: "<<charCurrentPos.x<<", z: "<<charCurrentPos.z<<"\n\n";

}

userCharacter::~userCharacter(){
	delete CharCaption;
	mSceneMgr->destroyQuery(mRaySceneQuery);	
}

void userCharacter::showBoundingBoxes(bool state){
	mCharNode->showBoundingBox(state);
	CaptionNode->showBoundingBox(state);
}

Vector3 userCharacter::getCurrentPosition(){	
	return charCurrentPos;
}

Quaternion userCharacter::getCurrentOrientation(){
	//return mCharNode->getOrientation();
	return charCurrentOrientation;
}
bool userCharacter::CheckPosition(){

	charCurrentPos = mCharNode->getPosition();

	if(charLastPos.x == charCurrentPos.x && charLastPos.z == charCurrentPos.z)
		return false;

	charLastPos = charCurrentPos;

	return true;
}

// check if the character's orientation has changed since the last time this function was called
bool userCharacter::CheckOrientation(){
	
	charCurrentOrientation = mCharNode->getOrientation();
	if(charCurrentOrientation.getYaw(true) == charLastOrientation.getYaw(true))
		return false;

	charLastOrientation = charCurrentOrientation;

	return true;
}

// Update the character's position and orientation
void userCharacter::update(Ogre::Real elpTime){	

	if(WisDown && SisDown){
		setCharAnimation("Walk", elpTime, false, glVar->walkAnimSpeed, true);
		setCharAnimation("Idle1", elpTime, true, glVar->idleAnimSpeed, true);
		moving = false;
	}else{
		if(WisDown && allowMotion){
			mCharNode->translate(mCharNode->getOrientation()*Vector3(0,0,-glVar->moveSpeed*elpTime));
		
			setCharAnimation("Idle1", elpTime, false, glVar->idleAnimSpeed, true);
			setCharAnimation("Walk", elpTime, true, glVar->walkAnimSpeed, true);

			moving = true;
		}		
		if(SisDown && allowMotion){
			mCharNode->translate(mCharNode->getOrientation()*Vector3(0,0,glVar->moveSpeed*elpTime));
			
			setCharAnimation("Idle1", elpTime, false, glVar->idleAnimSpeed, true);
			setCharAnimation("Walk", elpTime, true, glVar->walkAnimSpeed, true);

			moving = true;
		}
	}

	if(AisDown){
		moving = true;
		if(!WisDown && !SisDown){
			setCharAnimation("Idle1", elpTime, false, glVar->idleAnimSpeed, true);
			setCharAnimation("Walk", elpTime, true, glVar->walkAnimSpeed, true);
			//moving = false;
		}else{
			//moving = true;
		}
		mCharNode->yaw(Radian (2*elpTime), Node::TS_LOCAL);		
	}

	if(DisDown){
		moving = true;
		if(!WisDown && !SisDown){
			setCharAnimation("Idle1", elpTime, false, glVar->idleAnimSpeed, true);
			setCharAnimation("Walk", elpTime, true, glVar->walkAnimSpeed, true);
			//moving = false;
		}else{
			//moving = true;
		}

		mCharNode->yaw(Radian (-2*elpTime), Node::TS_LOCAL);		
	}
	// if no key is pressed set animation to idle
	if(!WisDown && !SisDown && !AisDown && !DisDown){
		setCharAnimation("Walk", elpTime, false, glVar->walkAnimSpeed, true);
		setCharAnimation("Idle1", elpTime, true, glVar->idleAnimSpeed, true);
		moving = false;
	}
	// if A and D are down, check i. if either w or s are down, ii. else set animation to idle
	if(AisDown && DisDown){
		if(!WisDown && !SisDown){
			setCharAnimation("Walk", elpTime, false, glVar->walkAnimSpeed, true);
			setCharAnimation("Idle1", elpTime, true, glVar->idleAnimSpeed, true);
			moving = false;
		}
	}
	// execute actions for character (jum, kick...)
	if(jump){
		setCharAnimation("Jump", elpTime, true, 0.75, false);
		if(mAnimationState->hasEnded())
			jump = false;
	}else{
		setCharAnimation("Jump", elpTime, false, glVar->initSpeed, false);
		mAnimationState->setTimePosition(0);
	}
	
	if(attack){
		// tell the character to stop moving, while the attack animation is in progress
		allowMotion = false;

		setCharAnimation("Attack3", elpTime, true, glVar->initSpeed, false);
		if(mAnimationState->hasEnded())
			attack = false;		
	}else{
		if(!sidekick)
			allowMotion = true;

		setCharAnimation("Attack3", elpTime, false, glVar->initSpeed, false);
		mAnimationState->setTimePosition(0);
	}

	if(sidekick){
		// tell the character to stop moving, while the sidekick animation is in progress
		allowMotion = false;

		setCharAnimation("SideKick", elpTime, true, glVar->initSpeed, false);
		if(mAnimationState->hasEnded())
			sidekick = false;		
	}else{
		if(!attack)
			allowMotion = true;

		setCharAnimation("SideKick", elpTime, false, glVar->initSpeed, false);
		mAnimationState->setTimePosition(0);
	}
	// End of actions

	// Execute a raySceneQuery so that our character is always
	// maintained above the ground

	charCurrentPos = mCharNode->getPosition();
	//charCurrentOrientation = mCharNode->getOrientation();

	// Collision detection
	Ray characterRay(Vector3(charCurrentPos.x, 5000.0f, charCurrentPos.z),Vector3::NEGATIVE_UNIT_Y);
	
	mRaySceneQuery->setRay(characterRay);		
	mRaySceneQuery->setSortByDistance(false);
		
	RaySceneQueryResult &result=mRaySceneQuery->execute();
	RaySceneQueryResult::iterator itr;

	for(itr = result.begin(); itr != result.end(); itr++){			
		if(itr->worldFragment){				
			Real terrainHeight = itr->worldFragment->singleIntersection.y;				
			if(terrainHeight!=charCurrentPos.y){				
				mCharNode->setPosition(Vector3(charCurrentPos.x, terrainHeight, charCurrentPos.z));															
				break;
			}
		}
	}
	// end collision detection

}

// Set action state [T/F]
void userCharacter::charSetActionState(int actionName, bool state){
	switch(actionName){
		case ATTA:
			attack = state;
			break;
		case JUMP:
			jump = state;
			break;
		case SIDE:
			sidekick = state;
			break;
	}
}

// update the camera's position and orientation
void userCharacter::updateCamera(const OIS::MouseEvent &arg)
{	
	// Quaternion Based Camera with limits (limits need to be perfected)
	Quaternion qx, qy;
	
	qx.FromAngleAxis(Degree(-arg.state.Y.rel*glVar->mRotateSpeed), Vector3::UNIT_X);
	qy.FromAngleAxis(Degree(-arg.state.X.rel*glVar->mRotateSpeed), Vector3::UNIT_Y);

	qx.normalise();
	qy.normalise();

	cameraTargetNode->rotate(qy, Node::TS_LOCAL);
			
	// The pitch Limit -> Still not perfect (the limits' change is determined by the qx.x value)
	//std::cout<<alpha.x<<" "<<qx.x<<"\n\n";
	if((alpha.x+qx.x) >= -0.69 && (alpha.x+qx.x) <= 0.69)
			cameraPitchNode->rotate(qx, Node::TS_LOCAL);
	
	alpha = cameraPitchNode->getOrientation();
}

// simple camera zoom with limits, using the mouse wheel
void userCharacter::zoomCamera(const OIS::MouseEvent &arg)
{
	camPoz = mCameraNode->getPosition();

	// Use [+] or [-] to invert the scroll
	// You can change the zoomSpeed from GlobalVariables Class
	NextCamZpos = camPoz.z-arg.state.Z.rel*glVar->zoomSpeed;
		
	// camera zoom Limits. 
	if(NextCamZpos >= glVar->zoomLimitClose && NextCamZpos <= glVar->zoomLimitFar){
		mCameraNode->setPosition(Vector3(camPoz.x, camPoz.y, NextCamZpos));
	}
}

// returns the camera to is init position
void userCharacter::resetCamera()
{
	// TODO: Make zoom smooth -> using slerp or squad
		
	// reset Camera Position
	Vector3 currentFacingT = cameraTargetNode->getOrientation()*initFacing;
	Vector3 currentFacingP = cameraPitchNode->getOrientation()*initFacing;

	Quaternion quatT = currentFacingT.getRotationTo(initFacing);
	Quaternion quatP = currentFacingP.getRotationTo(initFacing);

	cameraTargetNode->rotate(quatT, Node::TS_LOCAL);
	cameraPitchNode->rotate(quatP, Node::TS_LOCAL);
						
	// reset zoom
	mCameraNode->setPosition(Vector3(0, 0, 650));
}

// set the animation state of the character entity.
// FALSE to stop animation, TRUE to start it
// we can decrease or increase the animation speed using AnimSpeed (eg 0.5)
void userCharacter::setCharAnimation(Ogre::String AnimName, Ogre::Real elapsedTime, bool AnimState, float AnimSpeed, bool loop)
{
	//Idle1, Walk
	mAnimationState=mEnt->getAnimationState(AnimName);

	// Do we want the animation to loop?
	mAnimationState->setLoop(loop);
		
	mAnimationState->setEnabled(AnimState);
			
	if(AnimState)
		mAnimationState->addTime(elapsedTime*AnimSpeed);
}