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
#include "Database.h"

#include "GetTime.h"

struct listNode{
	void *data;					// Pointer to the structure that contains the Unit data (3rd user...)| cast to void and then back to the needed type
	
	bool _destroy;				// if True: this node must be destroyed
	listNode *prev;				// Pointer to the previous node of the list
	listNode *next;				// Pointer to the next node of the list
};

// ----------------------------------------------------------
class LinkedList{
public:
	listNode *Head;				// First Node of the L.List
	listNode *Tail;				// Last Node of the L.List

	int numberOfNodes;

	// use them to calculate how long a function needs to be executed
	unsigned long after, before;
		
public:
	LinkedList();
	~LinkedList();

	// Core Linked List Functions
	void addNode(userData *udata);
	void destroyNode(listNode *trg);
	void destroyAll();

	bool removeUnitFromList(SystemAddress clientID);
	bool removeUnitFromList(char uuid[UUID_LEN]);

	listNode* findNode(char uuid[UUID_LEN]);
	listNode* findNode(SystemAddress clientID);
	
	userData* findUnit(char uuid[UUID_LEN]);
	userData* findUnit(SystemAddress clientID);

	userData* getUnitData(listNode *fromNode);	

	// Backup functions
	void SaveUsersToDb(Database *dbPtr);
	bool isUserOnline(char uuid[UUID_LEN]);

	// sync try #1
	void calculate_pozition();

	// set functions	
};


