/* ------------------------------------------------------------------------------------ *
 * Impementation of the FrameListener class												*
 * Filename: cFrameListener.cpp															*
 * ------------------------------------------------------------------------------------ *
 */

#include "cFrameListener.h"

cFrameListener::cFrameListener(OIS::Keyboard* keyboard, OIS::Mouse* mouse, RenderWindow *win){//, CEGUI::Renderer *GUIr

	// Set pointers...
	mKeyboard = keyboard; 
	mMouse = mouse;
	mWin = win;	
	mGUISystem = CEGUI::System::getSingletonPtr();

	mKeyboard->setEventCallback(this);
	mMouse->setEventCallback(this);
	
	// (L)eft & (R)ight mouse buttons
	mLMouseDown = false;
	mRMouseDown = false;
	
	userCharSet = false;
	mContinue = true;

	// get CEGUI dialogs (fps dialogs)
	currentFps = CEGUI::WindowManager::getSingletonPtr()->getWindow((CEGUI::utf8*)"stats/current_fps");
	averageFps = CEGUI::WindowManager::getSingletonPtr()->getWindow((CEGUI::utf8*)"stats/avg_fps");
	bestFps = CEGUI::WindowManager::getSingletonPtr()->getWindow((CEGUI::utf8*)"stats/best_fps");
	worstFps = CEGUI::WindowManager::getSingletonPtr()->getWindow((CEGUI::utf8*)"stats/worst_fps");

	showCharBox = false;

	// dbg
	prevTime = RakNet::GetTime();
	lastDbgPoz = Vector3(0,0,0);
}

cFrameListener::~cFrameListener(){
}

/*
 * Do stuff at the beggining of the frame
 */
bool cFrameListener::frameStarted(const FrameEvent &evt){
			
	// update the input devices
	mKeyboard->capture();
	mMouse->capture();

	// Update the user character
	// we have passed keyboard & mouse events an now we call it,
	// to process those events, and update the character accordingly
	
	// 1. if network up, receive & proccess packets
	if(cNet->getNetworkState())
		cNet->readPacket();

	// 2. if the user's character exists, update & render him
	if(glVar->mainCharExists){	// The main user character has been created, we can start updating and rendering it
		if(userCharSet){
			// dbg
			if(RakNet::GetTime() >= prevTime + 1000)
			{
				Vector3 currentPoz = mChar->mCharNode->getPosition();
				std::cout<<"space walked in 1 sec x: "<<currentPoz.x-lastDbgPoz.x<<", z: " << currentPoz.z-lastDbgPoz.z<<"\n";
				prevTime = RakNet::GetTime();
				lastDbgPoz = currentPoz;
			}
				
			mChar->update(evt.timeSinceLastFrame);
			if(!mChar->moving)
			{
				//std::cout<<"character not moving\n";
				if(!cNet->idleWasSent)
					cNet->sendIDLE();
			}else
			{
				//std::cout<<"moving\n";
			}
		}else{
			// if the user character pointer has not been set for this class,
			// then set it...
			mChar = cNet->getUserCharPtr();
			userCharSet = true;
		}
	}

	// 3. if the list with other characters exists, render them
	if(glVar->UnitListExists && !unitListPtr->stopRendering()){
		unitListPtr->RenderAll(evt.timeSinceLastFrame);
		unitListPtr->CheckWhoShouldBeIdle(RakNet::GetTime());
	}

	return mContinue;
}

bool cFrameListener::frameEnded(const FrameEvent &evt){
	updateStats();
	return true;
}

// Do actions, Key is pressed
bool cFrameListener::keyPressed(const OIS::KeyEvent &e){
	
	Vector3 campoz;// debug

	switch(e.key){
		// if the user hits ESC, then exit game
		case OIS::KC_ESCAPE:
			mContinue = false;
			break;
		case OIS::KC_SLASH:			
			toogleDialogVisibility("debug");
			break;
		case OIS::KC_PERIOD:
			toogleDialogVisibility("stats");
			break;
	}
	if(glVar->mainCharExists){
		// Proccess those keys only if the main character has been created
		switch(e.key){
			// pass key events to our userCharacter class
			case OIS::KC_W:
				if(!mChar->getAttack_state())
					mChar->setW_state(true);
				break;
			case OIS::KC_S:
				mChar->setS_state(true);
				break;
			case OIS::KC_A:
				mChar->setA_state(true);
				break;
			case OIS::KC_D:
				mChar->setD_state(true);
				break;
			case OIS::KC_SPACE:
				mChar->charSetActionState(JUMP, true);
				break;
			case OIS::KC_F:
				mChar->charSetActionState(SIDE, true);
				break;
			case OIS::KC_MINUS:
				this->glVar->moveSpeed -= 5;
				this->glVar->walkAnimSpeed = glVar->moveSpeed/10;
				break;
			case OIS::KC_ADD:
				glVar->moveSpeed+= 5;
				glVar->walkAnimSpeed = glVar->moveSpeed/10;
				break;
			case OIS::KC_0:
				if(!mChar->getStealth_state()){
					mChar->setStealth_state(true);
					mChar->setMoveSpeed(5);
				}else{
					mChar->setStealth_state(false);
					mChar->setMoveSpeed(10);
				}
				break;
				
			case OIS::KC_B:
				showCharBox = !showCharBox;
				mChar->showBoundingBoxes(showCharBox);
				break;
		}
	}

	// Inject keyboard events to CEGUI (key pressed)
	CEGUI::System::getSingletonPtr()->injectKeyDown(e.key);
	CEGUI::System::getSingletonPtr()->injectChar(e.text);

	return true;
}

// Do actions, key is released
bool cFrameListener::keyReleased(const OIS::KeyEvent &e){

	// Proccess those keys only if the main character has been created
	if(glVar->mainCharExists){
		switch(e.key){
			case OIS::KC_W:
				mChar->setW_state(false);
				break;
			case OIS::KC_S:
				mChar->setS_state(false);
				break;
			case OIS::KC_A:
				mChar->setA_state(false);
				break;
			case OIS::KC_D:
				mChar->setD_state(false);
				break;
		}
	}
	
	// inject keyboard events to CEGUI (key released)
	CEGUI::System::getSingletonPtr()->injectKeyUp(e.key);	
	
	return true;
}

// Do actions, Mouse Moves
bool cFrameListener::mouseMoved(const OIS::MouseEvent &e){
	// update CEGUI mouse pozition
	mGUISystem->injectMouseMove(e.state.X.rel, e.state.Y.rel);	

	if(glVar->mainCharExists){
		// Proccess this events only if the main character has been created
		if(mRMouseDown)
			mChar->updateCamera(e);

		mChar->zoomCamera(e);
	}

	return true;
}

// Do actions, Mouse button Pressed
bool cFrameListener::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id){

	// inject mouse events to CEGUI (mouse down)
	CEGUI::System::getSingletonPtr()->injectMouseButtonDown(convertButton(id));

	if(id == OIS::MB_Left){ //  if left mouse button was pressed...
		onLeftPressed(e);
		mLMouseDown = true;
	}		
	if(id == OIS::MB_Right){ // if right mouse button was pressed...
		onRightPressed(e);
		mRMouseDown = true;
	}	
	
	if(id == OIS::MB_Middle){
		mChar->resetCamera();
	}

	return true;
}

//Do actions, Mouse button Released
bool cFrameListener::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id){
	
	// inject mouse events to CEGUI (mouse up)
	CEGUI::System::getSingletonPtr()->injectMouseButtonUp(convertButton(id));

	if(id == OIS::MB_Left){
		onLeftReleased(e);
		mLMouseDown = false;
	}		
	if(id == OIS::MB_Right){
		onRightReleased(e);
		mRMouseDown = false;
	}	

	return true;
}

// Proccess Right mouse button
void cFrameListener::onRightPressed(const OIS::MouseEvent &arg){
	mouseLastPoz = CEGUI::MouseCursor::getSingleton().getPosition();				
	CEGUI::MouseCursor::getSingleton().hide();	
}

void cFrameListener::onRightReleased(const OIS::MouseEvent &arg){
	// once the mouse button is released, reset the last mouse pozition
	// and so, the mouse cursor stays in the same position, while
	// rotating the camera
	CEGUI::System::getSingletonPtr()->injectMousePosition(mouseLastPoz.d_x ,mouseLastPoz.d_y);
	CEGUI::MouseCursor::getSingleton().show();
}


// Proccess Left Mouse button
void cFrameListener::onLeftPressed(const OIS::MouseEvent &arg){
	// TODO: point & click world navigation
}

void cFrameListener::onLeftReleased(const OIS::MouseEvent &arg){
}

// Convert OIS mouse input to CEGUI mouse input
CEGUI::MouseButton cFrameListener::convertButton(OIS::MouseButtonID buttonID){
    switch (buttonID){
    case OIS::MB_Left:
        return CEGUI::LeftButton;
    case OIS::MB_Right:
        return CEGUI::RightButton;
    case OIS::MB_Middle:
        return CEGUI::MiddleButton;
    default:
        return CEGUI::LeftButton;
    }
}

void cFrameListener::toogleDialogVisibility(char *dialogName){
	CEGUI::Window *dbg_win = CEGUI::WindowManager::getSingletonPtr()->getWindow((CEGUI::utf8*)dialogName);
	
	if(dbg_win->isVisible())
		dbg_win->setVisible(false);
	else
		dbg_win->setVisible(true);	
}

void cFrameListener::updateStats(){
	const RenderTarget::FrameStats& stats = mWin->getStatistics();

	CEGUI::String dbgStr;
	Ogre::String temp;

	// setup debug string for current pozition
	temp.append("Current Fps: ");
	temp.append(Ogre::StringConverter::toString(stats.lastFPS));
	dbgStr = temp.c_str();
	currentFps->setText(dbgStr);
	temp.clear();
	dbgStr.clear();

	temp.append("Average Fps: ");
	temp.append(Ogre::StringConverter::toString(stats.lastFPS));
	dbgStr = temp.c_str();
	averageFps->setText(dbgStr);
	temp.clear();
	dbgStr.clear();

	temp.append("Best Fps: ");
	temp.append(Ogre::StringConverter::toString(stats.lastFPS));
	dbgStr = temp.c_str();
	bestFps->setText(dbgStr);
	temp.clear();
	dbgStr.clear();

	temp.append("Worst Fps:     ");
	temp.append(Ogre::StringConverter::toString(stats.lastFPS));
	dbgStr = temp.c_str();
	worstFps->setText(dbgStr);
	temp.clear();
	dbgStr.clear();
}