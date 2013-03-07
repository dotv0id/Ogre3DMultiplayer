#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "vld.h"

#include "core.h"

Core aboveSrv;
GlobalVariables	globalVar;

/* Callback function for the timer:
 * This function is called every poPackInterval [look in GlobalVariables class],
 * independently of the game flow, since a queue timer runs in a different thread.
 */
void CALLBACK timerProc(PVOID lpParam, BOOLEAN TimerOrWaitFired){
/*	
	if(aboveSrv.getNetworkState()){
		aboveSrv.timerCoreFunction();
		if(!(globalVar.TimeSinceLastBackup == globalVar.userBackupInterval))
			globalVar.TimeSinceLastBackup += globalVar.TickInterval;
		
		aboveSrv.timerCoreFunction();
	}
	*/
}

int main(void){
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	cout<<"+-----------------------------------------------------------+\n";
	cout<<"               Above Server v0.1 alpha\n";
	cout<<"+-----------------------------------------------------------+\n\n";
	
	HANDLE qTimer;				// queue timer Handle
	HANDLE hTimerQueue;			// timer queue handle
	int _arg = 1;

	hTimerQueue = CreateTimerQueue();
	
	if(hTimerQueue == NULL)
		std::cout<<"creating timer queue failed: "<<GetLastError()<<"n";

	// create the timer	
	if(!CreateTimerQueueTimer(&qTimer, hTimerQueue, (WAITORTIMERCALLBACK)timerProc, &_arg, 0, globalVar.TickInterval, WT_EXECUTEINTIMERTHREAD))
		MessageBoxA(NULL, "Timer Failed", "You just got pwned!", MB_OK | MB_ICONERROR | MB_TASKMODAL);

	// Pass global Variables pointer...
	aboveSrv.setGlobalVariablesPtr(&globalVar);
	aboveSrv.getDatabasePtr()->setGlobalVariablesPtr(&globalVar);
	aboveSrv.getNetworkPtr()->setGlobalVariablesPtr(&globalVar);
	
	aboveSrv.go();					// Start the server
	DeleteTimerQueue(hTimerQueue);	// Delete timer & queue
	aboveSrv.terminate();			// Clean up
	
	system("pause");

	return 0;
}