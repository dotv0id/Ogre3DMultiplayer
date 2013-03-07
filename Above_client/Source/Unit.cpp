/* ------------------------------------------------------------------------------------ *
 * Impementation of the Unit class														*
 * Filename: Unit.cpp																	*
 * ------------------------------------------------------------------------------------ *
 */


#include "Unit.h"

Unit::Unit(std::string id, std::string uname, std::string modelname, Vector3 initPoz, SceneManager *mgr){
	
	uuid = id;
	username = uname;
	modelName = modelname;

	charCurrentPos = NextPosition = initPoz;
	
	animIdleSpeed = 0.5;
	
	mySmPtr = mgr;
		
	charEnt = mySmPtr->createEntity(username, modelName);
	
	charNode = mySmPtr->getRootSceneNode()->createChildSceneNode(username);
	charNode->attachObject(charEnt);
	
	charNode->scale(0.05, 0.05, 0.05);
	charNode->setPosition(charCurrentPos);
	
	changeSinceLastRender	= false;
	isMoving				= false;

	// username above character
	UnitCaption = new MovableText("caption", username);
	UnitCaption->setTextAlignment(MovableText::H_CENTER, MovableText::V_ABOVE);
	UnitCaption->setCharacterHeight(20);

	String __cap = "caption_";__cap.append(username);
	CaptionNode = charNode->createChildSceneNode(__cap);
	CaptionNode->setPosition(Vector3(0, 210, -15));
	CaptionNode->attachObject(UnitCaption);
		
	// Collision detection
	mRaySceneQuery = mySmPtr->createRayQuery(Ray());

	KeepUnitAlive = true;
	// new user unit is ready

	last_poz_pack_received = 0;
	last_ornt_pack_received = 0;

	IDLE_after = 100;	// millisecond
}

void Unit::setCharcaterPosition(Vector3 newPos){
	//charNode->setPosition(newPos);
	lastPz = smoothPoz;
	charCurrentPos = newPos;
	std::cout<<"x: "<<charCurrentPos.x<<" z: "<<charCurrentPos.z<<"\n";
}

void Unit::setCharacterOrientation(float rotY){
	Radian rot = Degree(rotY);
	charNode->setOrientation(Quaternion(rot, Vector3::UNIT_Y));
}

bool Unit::checkIfMoving(){
	return true;
}

bool Unit::checkIfShouldBeIDLE(unsigned long currentTime){
/*
	unsigned long last_time;
	if(last_poz_pack_received >= last_ornt_pack_received)
		last_time = last_poz_pack_received;
	else
		last_time = last_ornt_pack_received;
	
	if((currentTime - last_time) > IDLE_after)
		isMoving = false;
	else{
		if(!isMoving)
			isMoving = true;
	}
*/
	return true;
}

void Unit::RenderMe(Real elpTime){
	
	charNode->setPosition(charCurrentPos);

	if(!isMoving){
		setCharAnimation("Walk", elpTime, false, animMoveSpeed, true);
		setCharAnimation("Idle1", elpTime, true, animIdleSpeed, true);
		//std::cout<<"not moving\n";
	}else{		
		setCharAnimation("Idle1", elpTime, false, animIdleSpeed, true);
		setCharAnimation("Walk", elpTime, true, animMoveSpeed, true);		
		//std::cout<<"moving\n";
	}

	// collision detection
	charCurrentPos = charNode->getPosition();
	//charCurrentPos = NextPosition;

	Ray characterRay(Vector3(charCurrentPos.x, 5000.0f, charCurrentPos.z),Vector3::NEGATIVE_UNIT_Y);
	
	mRaySceneQuery->setRay(characterRay);		
	mRaySceneQuery->setSortByDistance(false);
		
	RaySceneQueryResult &result=mRaySceneQuery->execute();
	RaySceneQueryResult::iterator itr;

	for(itr = result.begin(); itr != result.end(); itr++){
		if(itr->worldFragment){				
			Real terrainHeight = itr->worldFragment->singleIntersection.y;				
			if(terrainHeight!=charCurrentPos.y){				
				charNode->setPosition(Vector3(charCurrentPos.x, terrainHeight, charCurrentPos.z));															
				break;
			}
		}
	}
	// End of collision detection

	//std::cout<<elpTime<<"\n";
	//std::cout<<uuid<<" poz, x: "<< charCurrentPos.x <<", z: " <<charCurrentPos.x<<"\n";
}

void Unit::setCharAnimation(Ogre::String AnimName, Real elapsedTime, bool AnimState, float AnimSpeed, bool loop){
	//Idle1, Walk
	mAnimationState=charEnt->getAnimationState(AnimName);

	// Do we want the animation to loop?
	mAnimationState->setLoop(loop);		
	mAnimationState->setEnabled(AnimState);
			
	if(AnimState)
		mAnimationState->addTime(elapsedTime*AnimSpeed);
}

Unit::~Unit(){
	charNode->detachAllObjects();						// detach all objects attached

	mySmPtr->destroySceneNode(charNode->getName());		// destroy the Character Node

	delete UnitCaption;									// destroy the caption & its node
	mySmPtr->destroySceneNode(CaptionNode->getName());

	mySmPtr->destroyEntity(charEnt->getName());			// destroy the character

	mySmPtr->destroyQuery(mRaySceneQuery);
	std::cout<<"\n--unit destroyed--\n";
}