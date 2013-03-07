/* ------------------------------------------------------------------------------------ *
 * LinkedList class																		*
 * Filename: LinkedList.h																*
 *																						*
 * ------------------------------------------------------------------------------------ *
 * Description:																			*
 * ------------------------------------------------------------------------------------ *
 * This class creates and manages a double linked list.									*
 * 																						*
 * ------------------------------------------------------------------------------------ *
 */

#include "DataTypes.h"

struct listNode{
	void *data;					// Pointer to the structure that contains the Unit data (3rd user...)| cast to void and then back to the needed type
	
	bool _destroy;				// if True: this node must be destroyed
	listNode *prev;				// Pointer to the previous node of the list
	listNode *next;				// Pointer to the next node of the list
};

// ----------------------------------------------------------
class LinkedList{
private:
	listNode *Head;				// First Node of the L.List
	listNode *Tail;				// Last Node of the L.List
	int numberOfNodes;

	SceneManager *uMgr;			// Pointer to the scene Manager	
	
	
		
	bool PauseRender;			// indicates whether the game should render elements of the list (true -> stop rendering)

public:
	LinkedList();
	~LinkedList();

	// Core Linked List Functions
	void addNode(Unit *udata);
	void destroyNode(listNode *trg);
	Unit* findUnit(char uuid[UUID_LEN]);
	listNode* findNode(char uuid[UUID_LEN]);
	Unit* getUnitData(listNode *fromNode);
	void destroyAll();

	// Unit linked list related functions
	void newUserUnit(std::string id, std::string username, std::string modelname, Vector3 initPos, float orientation);
	void deleteUserUnit(std::string uuid);
	
	void RenderUnit(std::string uuid);	
	void RenderAll(Real elapsedTime);
	void CheckWhoIsMoving();
	void CheckWhoShouldBeIdle(unsigned long current_time);

	// set & get functions
	void setSceneMgrPtr(SceneManager *m){uMgr = m;};
	
	bool hasElements(){
		if(numberOfNodes>3)
			return true;
		return false;
	}

	bool stopRendering(){return PauseRender;};	// true, stops rendering of this list's elements

};


