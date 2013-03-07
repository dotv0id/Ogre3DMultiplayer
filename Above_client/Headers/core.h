/* ------------------------------------------------------------------------------------ *
 * Core class																			*
 * Filename: core.h																		*
 *																						*
 * ------------------------------------------------------------------------------------ *
 * Description:																			*
 * ------------------------------------------------------------------------------------ *
 * The Core class of the game. Everything passes through here...						*
 *																						*
 * ------------------------------------------------------------------------------------ *
 */

#ifndef _CORE_H
#define _CORE_H


#include "cFrameListener.h"
//#include "OgreOde_Core.h"

class coreApp{
private:
	GlobalVariables		*c_glVar;			// Pointer to GlobalVariables Class	
	Network				*gNet;				// Pointer to Network class
	cFrameListener		*myListener;		// Pointer to the FrameListener class	
	GameWorld			*_world;			// Pointer to GameWorld Class
	userCharacter		*userChar;			// Pointer to the userCharacter class
	LinkedList			*unitLL;			// Pointer to the 3rd user linked list

	Root				*mRoot;				// Ogre3D root...

	//OgreOde::World		*wld;

    OIS::Keyboard		*mKeyboard;
	OIS::Mouse			*mMouse;
    OIS::InputManager	*mInputManager;

	String secName, typeName, archName;		// need to parse cfg files
    ConfigFile cf;							// define cfg file here

	size_t				windowHnd;
	std::ostringstream	windowHndStr;
	OIS::ParamList		pl;
	
	RenderWindow		*win;
	SceneManager		*mgr;
	Camera				*cam;
	Viewport			*vp;

	// CEGUI Variables
	CEGUI::OgreCEGUIRenderer	*mRenderer;
	CEGUI::System				*mSystem;
	CEGUI::Window				*sheet;
	CEGUI::WindowManager		*wmgr;
	
	// pozition debug window
	CEGUI::Window *dbg_x, *dbg_y, *dbg_z, *dbg_packNo, *dbg_orientation;

	// Debug strings
	CEGUI::String dbgStr;
	Ogre::String temp;
	
	// Check & Flow variables
	bool networkState;						// True if the network has been initilized
	bool iu_timeout;						// True if initial update packet hasn't arrived in time	
	bool userCharSet;						// true if the user character pointer has been set for this class	

public:
	coreApp();
	~coreApp();

	void createRoot();
    void defineResources();
    void setupRenderSystem();
	void getClientConfiguration();
    void createRenderWindow();
    void initializeResourceGroups();	
	void startNetwork();
    void setupScene();
	void setupInputSystem();
    void setupCEGUI();
    void createFrameListener();
    void startRenderLoop();
	void initializeDataStructures();
	
	void go();

	// Variable set & get functions
	bool getNetworkState(){return networkState;};	
	void setGlobalVariablesPtr(GlobalVariables *gv){c_glVar = gv;};	

	// Timer Function (Timed callback every POPackInterval)
	void timerCoreFunction();

	// CEGUI Event Handlers
	bool handleQuit(const CEGUI::EventArgs &e);
	bool handleLogin(const CEGUI::EventArgs &e);	
};

#endif