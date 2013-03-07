/* ------------------------------------------------------------------------------------ *
 * FrameListener class																	*
 * Filename: cFrameListener.h															*
 *																						*
 * ------------------------------------------------------------------------------------ *
 * Description:																			*
 * ------------------------------------------------------------------------------------ *
 * This class, processes the events that occure, at the beggining, or at the end of a	*
 * frame, and respond... (keyboard & mouse events, CEGUI events...)						*
 *																						*
 * Using OIS buffered input																*
 *																						*
 * ------------------------------------------------------------------------------------ *
 */

#ifndef _CFRAMELISTENER_H
#define _CFRAMELISTENER_H


#include <OgreFrameListener.h>
//#include <CEGUI/CEGUI.h>
//#include <CEGUI/CEGUISystem.h>
#include <OgreCEGUIRenderer.h>

#include "Network.h"

#include "OgreStringConverter.h"

// debug purposes iostream **
#include <iostream>


using namespace Ogre;
class cFrameListener : public FrameListener, public OIS::KeyListener, public OIS::MouseListener{

private:
	userCharacter	*mChar;		// Pointer to the userCharacter class
	Network			*cNet;		// Pointer to Network class
	GlobalVariables *glVar;		// Pointer to GlobalVariables class
	LinkedList		*unitListPtr;
	
	// Mouse Buttons state cheching variables
	bool mLMouseDown, mRMouseDown, mRgotReleased; 

	// Render window pointer
	RenderWindow *mWin;

	// Input System Pointers
	OIS::Keyboard *mKeyboard; 
	OIS::Mouse *mMouse;

	// Flow Vars
	bool mContinue;				// true, if to continue rendering
	bool userCharSet;			// true if the user character for this class has been set
	
	// CEGUI
	CEGUI::Renderer *mGUIRenderer;
	CEGUI::System *mGUISystem;

	CEGUI::Window *currentFps, *averageFps, *bestFps, *worstFps, *orientDbg;
	// Character movement variables
	CEGUI::Point mouseLastPoz, mousePozDeb, mousePozRay;

	bool showCharBox;

	void toogleDialogVisibility(char *dialogName);
	void updateStats();

	// time debug
	unsigned long prevTime;
	Vector3 lastDbgPoz;


public: 
	// Constructor -> input variables; [keyboard, mouse], [scenemanager, renderwindow, camera], CEGUI renderer
	cFrameListener(OIS::Keyboard* keyboard, OIS::Mouse* mouse, RenderWindow *win);
	~cFrameListener();

	bool frameStarted(const FrameEvent &evt);	// This gets called before the next frame is beeing rendered
	bool frameEnded(const FrameEvent &evt);		// This gets called at the end of a frame

	// Input Functions
	bool keyPressed(const OIS::KeyEvent &e);
	bool keyReleased(const OIS::KeyEvent &e);	
	bool mouseMoved(const OIS::MouseEvent &e);
	bool mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id);
	void onLeftPressed(const OIS::MouseEvent &arg);
	void onLeftReleased(const OIS::MouseEvent &arg);
	void onRightPressed(const OIS::MouseEvent &arg);
	void onRightReleased(const OIS::MouseEvent &arg);

	void requestShutdown(){mContinue = false;};

	// Set functions
	void setCEGUI_RendererPtr(CEGUI::Renderer *cgr){mGUIRenderer = cgr;};	
	void setCharacterPtr(userCharacter *character){mChar = character;};
	void setGlobalVariablesPtr(GlobalVariables *gv){glVar = gv;};
	void setUnitListPtr(LinkedList *l){unitListPtr = l;};
	void setNetworkPtr(Network *net){cNet = net;};

	// CEGUI functions
	CEGUI::MouseButton convertButton(OIS::MouseButtonID buttonID);
};

#endif

