/* ------------------------------------------------------------------------------------ *
 * Impementation of the Core class														*
 * Filename: core.cpp																	*
 * ------------------------------------------------------------------------------------ *
 */


#include "core.h"

coreApp::coreApp(){
	// set initial network state to 0. (Network not initialized yet)
	networkState = false;	
	iu_timeout = false;

	userCharSet = false;
}

void coreApp::go(){
	// 1. read client configuration file
	c_glVar->loadConfigFile("client.cfg");

	// 2. initiliaze graphic engine & load resources
	createRoot();
	defineResources();
	setupRenderSystem();
	createRenderWindow();
	initializeResourceGroups();

	startNetwork();					// create the network class
	setupScene();					// create the game world class
	setupInputSystem();				// setup the input system
	setupCEGUI();					// setup CEGUI and CEGUI event handlers
	createFrameListener();			// setup framelistener to "listen" for events
	initializeDataStructures();		// initialize the list(s)

	// 3. start render loop
	startRenderLoop();	
}

/*
 * Creates the Root object, which is a neccessity in order to use
 * Ogre3D...
 */
void coreApp::createRoot(){
	mRoot = new Root();
}

/*
 * Define the files that contain the resources for the game (materials, models, textures...).
 */
void coreApp::defineResources(){
	cf.load("resources.cfg");
	ConfigFile::SectionIterator seci = cf.getSectionIterator();

	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		ConfigFile::SettingsMultiMap *settings = seci.getNext();
		ConfigFile::SettingsMultiMap::iterator i;

		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName = i->first;
			archName = i->second;
			ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
		}
	}
}

/*
 * Setup the render system. If there is no ogre.cfg file, with the previous rendering configuration,
 * throw a config dialog...
 */
void coreApp::setupRenderSystem(){
	if (!mRoot->restoreConfig() && !mRoot->showConfigDialog())
		throw Exception(52, "User canceled the config dialog!", "Application::setupRenderSystem()");
}

void coreApp::getClientConfiguration(){
	// 1.	open the config.abv
	// 2.	read the server IP and Port
	// 3.	connect
	// 4a.	on success, continue
	// 4b.	on failure, display failure message and exit
}

/*
 * Creates the rendering window... win32 programming, Ogre3D side...
 */
void coreApp::createRenderWindow(){
	mRoot->initialise(true, "Above Client v 0.1 - preAlpha");
}

/*
 * Load the resources for our game...
 */
void coreApp::initializeResourceGroups(){
	TextureManager::getSingleton().setDefaultNumMipmaps(5);
	ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

/*
 * Setup the scene, aka Create the game world
 * The game world should be created, ONLY after the client has received a packet from the server,
 * passing the initial data, such as username, model name of the character, last saved position,
 * last saved orientation...
 */
void coreApp::setupScene(){
	mgr = mRoot->createSceneManager(ST_EXTERIOR_CLOSE);		// Setup the scene manager
	mgr->setWorldGeometry("terrain.cfg");
	c_glVar->setSceneManagerPtr(mgr);						// pass its pointer to global variables class
	
	cam = mgr->createCamera("Camera");						// Create the camera
	cam->setNearClipDistance(5);
	c_glVar->setCameraPtr(cam);								// pass its pointer to global variables class
	
	vp = mRoot->getAutoCreatedWindow()->addViewport(cam);	// And the viewport

	_world = new GameWorld(mgr);							// Initialize the game world class
	_world->setGlobalVarsPtr(c_glVar);						// Pass the GlobalVariables pointer
	
	gNet->setWorldPtr(_world);								// pass the world pointer to network
}

/*
 * Setup the input system, neccesary for the user to be able to interact with
 * the game world... and his character :D
 * We use OIS as input system (comes with Ogre3D).
 */
void coreApp::setupInputSystem(){
	windowHnd = 0;
	win = mRoot->getAutoCreatedWindow();

	win->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << windowHnd;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
	mInputManager = OIS::InputManager::createInputSystem(pl);

	try
	{
		mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject(OIS::OISKeyboard, true));
		mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject(OIS::OISMouse, true));		
	}catch(const OIS::Exception &e){
		throw Exception(42, e.eText, "Application::setupInputSystem");
	}
}

/*
 * Setup the 2D interface... CEGUI in this case...
 */
void coreApp::setupCEGUI(){
	mRenderer = new CEGUI::OgreCEGUIRenderer(win, Ogre::RENDER_QUEUE_OVERLAY, false, 3000, mgr);
	mSystem = new CEGUI::System(mRenderer);

	// Setup CEGUI Skin
	CEGUI::SchemeManager::getSingleton().loadScheme((CEGUI::utf8*)"AquaLookSkin.scheme");
	mSystem->setDefaultMouseCursor((CEGUI::utf8*)"AquaLook", (CEGUI::utf8*)"MouseArrow");
	CEGUI::MouseCursor::getSingleton().setImage("AquaLook","MouseArrow");
	
	// show login dialog
	CEGUI::Window *w = CEGUI::WindowManager::getSingleton().loadWindowLayout("aboveclient.layout");
	mSystem->setGUISheet(w);

	// subscribe CEGUI events
	wmgr = CEGUI::WindowManager::getSingletonPtr();

	wmgr->getWindow((CEGUI::utf8*)"system/exitBtn")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&coreApp::handleQuit, this));
	wmgr->getWindow((CEGUI::utf8*)"loginDlg/login")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&coreApp::handleLogin, this));

	// debug info
	dbg_x = wmgr->getWindow((CEGUI::utf8*)"debug/x");
	dbg_y = wmgr->getWindow((CEGUI::utf8*)"debug/y");
	dbg_z = wmgr->getWindow((CEGUI::utf8*)"debug/z");
	dbg_orientation = wmgr->getWindow((CEGUI::utf8*)"debug/Ry");
	dbg_packNo = wmgr->getWindow((CEGUI::utf8*)"debug/packno");
	
	// TODO: Add mask "*" to the password editbox		... dunno how.
}

bool coreApp::handleQuit(const CEGUI::EventArgs &e){
	myListener->requestShutdown();
	return true;
}

/* 
 * Once called, it will connect to the server, send username and password,
 * and receive our character's last saved pozition in the world, along with the
 * positions of other players that are within our range.
 */
bool coreApp::handleLogin(const CEGUI::EventArgs &e){
	//CEGUI::Window *server_ip_inp = wmgr->getWindow((CEGUI::utf8*)"system/serverIp");
	//CEGUI::Window *server_port_inp = wmgr->getWindow((CEGUI::utf8*)"system/serverPort");

	CEGUI::Window *loginDialog = wmgr->getWindow((CEGUI::utf8*)"loginDlg");
	CEGUI::Window *username = wmgr->getWindow((CEGUI::utf8*)"loginDlg/uname");
	CEGUI::Window *pwd = wmgr->getWindow((CEGUI::utf8*)"loginDlg/pwd");

	//CEGUI::String server_inp = server_ip_inp->getText();	// get the server IP from the iser
	//c_glVar->server_IP = server_inp.c_str();				// server IP, user entered	
	//server_inp.clear();

	// Server IP and Port saved... get username and password and proceed
	// TODO -get- & ->send username & password

	CEGUI::String sUname = username->getText();
	CEGUI::String sPwd = pwd->getText();

	std::cout<<"username: "<<sUname.c_str()<<" | password: "<<sPwd.c_str()<<"\n";

	// Initialize the network
	// Once initialized, the framelistener will start receiving packets (framestarted)
	if(gNet->getNetworkState())
	{
		std::cout<<"---* Network has been initialized...\n";

	}else{
		// network Not initialized, initilize it
		gNet->initializeNetwork();
	}

	if(!gNet->isConnected())
	{
		// connect to server
		gNet->connectToServer(c_glVar->server_IP, c_glVar->server_port);
		gNet->setupLoginPacket(sUname.c_str(), sPwd.c_str());		
	}else{
		std::cout<<"Already connected...\n";
	}

	sUname.clear();
	sPwd.clear();	

	networkState = gNet->getNetworkState();

	return true;
}

/*
 * Create the games Frame Listener. The frame listener "listens" for events
 * and updates the correct elements in the world, according to those events...
 */
void coreApp::createFrameListener(){
	myListener = new cFrameListener(mKeyboard, mMouse, win);//, mRenderer
		
	myListener->setCEGUI_RendererPtr(mRenderer);	// pass the CEGUI Renderer pointer
	myListener->setGlobalVariablesPtr(c_glVar);		// pass the global variables pointer
	myListener->setNetworkPtr(gNet);				// pass the network Pointer
	
	mRoot->addFrameListener(myListener);
}
 
/*
 * Start the render loop, aka Begin the game...
 */
void coreApp::startRenderLoop(){
		mRoot->startRendering();
}

/*
 * Create a new Network class, and connect to the world server.
 */
void coreApp::startNetwork(){	
	gNet = new Network();	
	
	gNet->setGlobalVariablesPtr(c_glVar);			// pass the global variables pointer
}

// initialize the Unit list
void coreApp::initializeDataStructures(){	

	unitLL = new LinkedList();
	unitLL->setSceneMgrPtr(mgr);
	std::cout<<"Linked list has been initialized (loginhandle)\n";

	gNet->setUnitPtr(unitLL);			// Pass the Unit List pointer to the network
	myListener->setUnitListPtr(unitLL);	// Pass the Unit List pointer to the FrameListener

	c_glVar->UnitListExists = true;		// Now that the list has been created, inform the game.
}

/* 
 * This function is being called everytime the Queue timer goes off
 */
void coreApp::timerCoreFunction(){
	
	// call the world class function, checkUserPOChange, if true
	// get the vector 3, and pass it to the network fuction
	// sendPOpacket

	// Check for how long have we been waiting for the
	// Initial update packet
	if(!iu_timeout && !c_glVar->init_char_update){		

		c_glVar->elapsed_iu += c_glVar->poPackInterval;
		if((c_glVar->elapsed_iu / 1000)>c_glVar->iu_timeout)
			iu_timeout = true;
	}

	/* Start checking character position and sending packets when neccessary
	 * --! Start the checking only if user's character has been created !--
	 * if the position of the character has change between those n? milliseconds,
	 * get his current position and tell network to send the new position to the server */	
	if(c_glVar->mainCharExists){
		if(!userCharSet)
		{
			userChar = gNet->getUserCharPtr();		// if coreApp class doesn't have a valid pointer to user character,
			userCharSet = true;						// then set it...
		}else{

			// 1.Check if the user's position has changed
			if(userChar->CheckPosition())
			{
				Vector3 _poz = userChar->getCurrentPosition();
				
				if(!gNet->sendPosition(_poz))
					std::cout<<"Error sending position packet.\n";
			
				// setup debug string for current pozition
				temp.append("x: ");temp.append(Ogre::StringConverter::toString(_poz.x));
				dbgStr = temp.c_str();dbg_x->setText(dbgStr);
				temp.clear();dbgStr.clear();
				
				temp.append("y: ");temp.append(Ogre::StringConverter::toString(_poz.y));
				dbgStr = temp.c_str();dbg_y->setText(dbgStr);
				temp.clear();dbgStr.clear();
				
				temp.append("z: ");temp.append(Ogre::StringConverter::toString(_poz.z));
				dbgStr = temp.c_str();dbg_z->setText(dbgStr);
				temp.clear();dbgStr.clear();				

				temp.append("PO: ");temp.append(Ogre::StringConverter::toString(c_glVar->pktCnt));
				dbgStr = temp.c_str();dbg_packNo->setText(dbgStr);
				temp.clear();dbgStr.clear();			
				// end of debug...							
			}else{
				//if(!gNet->idleWasSent)
					//gNet->sendIDLE();
			}
			
			// 2. Check if the user's orientation has changed
			if(userChar->CheckOrientation())
			{
				// TODO; orientation_y ; use float
				Quaternion q = userChar->getCurrentOrientation();
				Radian r = q.getYaw(true);
				float current_y_rot = r.valueDegrees();

				if(!gNet->sendOrientation(current_y_rot))
					std::cout<<"Error sending orientation packet.\n";
	
				temp.append(", Ry: ");temp.append(Ogre::StringConverter::toString(r));//?
				dbgStr = temp.c_str();dbg_orientation->setText(dbgStr);
				temp.clear();dbgStr.clear();
			}else{
			//	if(!gNet->idleWasSent)
					//gNet->sendIDLE();
			}

			// TODO:
			// linked list: if no pozpacket has been received for xx milliseconds, set character to idle
			// unit-> last_poz_packet_received	:: timestamp raknet
			// unit-> last_ornt_packet_rcvd		:: timestamp raknet
		}// end of [user exists & ptr is set in core class]
	}
}

coreApp::~coreApp(){

	// set NetworkState false, so that the timerProc won't send packets anymore
	networkState = false;
	std::cout<<"***  Network State has been set inactive...\n";
		
	// destroy input OIS objects
	mInputManager->destroyInputObject(mMouse);
	mInputManager->destroyInputObject(mKeyboard);
	OIS::InputManager::destroyInputSystem(mInputManager);

	// destroy CEGUI objects
	delete mSystem;
	delete mRenderer;	
	
	// Destroy the linked list... if ever created
	delete unitLL;
	std::cout<<"***  Unit linked list has been deleted... \n";
	
	// destroy the user character.. if ever created
	if(c_glVar->mainCharExists){
		c_glVar->mainCharExists = false;
		delete userChar;
		std::cout<<"***  Main user character deleted...\n";
	}
	
	delete _world;
	std::cout<<"The world has been destroyed...\n";

	delete myListener;
	
	// delete the network...
	gNet->terminateNetwork();
	delete gNet;	

	ResourceGroupManager::getSingleton().shutdownAll();
	delete mRoot;
}