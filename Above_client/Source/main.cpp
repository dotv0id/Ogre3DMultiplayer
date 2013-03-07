#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

//#include "vld.h"
#define OIS_DYNAMIC_LIB

#include "core.h"

GlobalVariables *globalVar = new GlobalVariables();
coreApp *app = new coreApp();

/* Callback function for the timer:
 * This function is called every poPackInterval [look in GlobalVariables class],
 * independently of the game flow, since a queue timer runs in a different thread.
 */
void CALLBACK timerProc(PVOID lpParam, BOOLEAN TimerOrWaitFired){
	
	/* instead of putting all the pointers here, just
	 * call a function in the core class...
	 * use getNetworkState as guard... if the network has been
	 * initialized (getNetworkState returns true), only then 
	 * start receiving and sending packets */
	if(app->getNetworkState())
		app->timerCoreFunction();
}


// Main -------------------
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	#define WIN32_LEAN_AND_MEAN
	#include "windows.h"
	INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
	int main(int argc, char **argv)
#endif
{
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	
	HANDLE qTimer;			// initialize timer
	HANDLE hTimerQueue;		// timer queue
	int _arg = 1;			// Optional argument for the CALLBACK function of the timer.

	// Create the timer queue.
	hTimerQueue = CreateTimerQueue();
	if(hTimerQueue == NULL)
		std::cout<<"creating timer queue failed: "<<GetLastError()<<"n";

	// create the timer	
	if(!CreateTimerQueueTimer(&qTimer, hTimerQueue, (WAITORTIMERCALLBACK)timerProc, &_arg, 0, globalVar->poPackInterval, WT_EXECUTEDEFAULT))
		MessageBoxA(NULL, "Timer Failed", "You just got pwned!", MB_OK | MB_ICONERROR | MB_TASKMODAL);

	  try{

		// Start the Game
		// The core class of the game		
		app->setGlobalVariablesPtr(globalVar);
		app->go();			

    }catch(Exception& e){

#if OGRE_PLATFORM == PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WIN32
        MessageBoxA(NULL, e.getFullDescription().c_str(), "You just got pwned!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
        fprintf(stderr, "An exception has occurred: %s\n",
            e.getFullDescription().c_str());
#endif
	  }

	  delete app;
	  delete globalVar;
	  
	  DeleteTimerQueue(hTimerQueue);

	  system("pause");
	  
	  return 0;
	}

