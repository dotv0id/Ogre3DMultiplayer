/* ------------------------------------------------------------------------------------ *
 * Impementation of the LinkedList class												*
 * Filename: LinkedList.cpp																*
 * ------------------------------------------------------------------------------------ *
 */


#include "LinkedList.h"


LinkedList::LinkedList(){
	Head = NULL;					// Initialize head and tail nodes,
	Tail = NULL;					// to point to noware.
	numberOfNodes = 0;
	std::cout<<"LinkedList initialized!\n";
	PauseRender = true;
}

void LinkedList::addNode(Unit *udata){

	// Create a new Node
	// Remember to call delete when destroying the node...
	listNode *newNode = new listNode();
	
	// Check if the list is empty or not...
	// And do the linking
	if(Head == NULL){
		// the list is empty. Add the first node
		Head = Tail = newNode;
		Head->prev = Head;
		Head->next = Head;		
	}else if(Head == Tail){
		// We have 1 node in the list.
		// Add this node as Tail
		Tail = newNode;
		Tail->prev = Tail->next = Head;
		Head->prev = Head->next = Tail;
	}else{
		// The list has at least 2 Nodes.
		// Add this node as Tail
		newNode->next = Tail->next;
		newNode->prev = Tail;
		Tail->next = newNode;
		Tail = newNode;
	}	

	Tail->_destroy = false;			// Set the _destroy value to false (this node has just been created... guess is it's needed)

	/* Pass the data structure pointer to the Node struct
	 * From now on, the data structure is the lists problem,
	 * which means that when the list destroys the Node, it has to destroy the data too (call delete 2 times) */
	newNode->data = (void*)udata;	// cast the incoming data ptr to v0id ptr

	std::cout<<"\n$> "<<numberOfNodes<<": List node has been added\n";
	numberOfNodes++;				// increase the number of nodes
}

// ... where trg the node to be destroyed
// you must call a function like find, to get the pointer to the node u want to delete
void LinkedList::destroyNode(listNode *trg){

	if(numberOfNodes > 0){
		// There are still elements in the list
		if(Head == Tail){
			// This is the last Node in the list
			Head = Tail = NULL;			
			std::cout<<"\nlast node deleted\n";			
		}else{
			// There are at least 2 Nodes in the list
			trg->prev->next = trg->next;
			trg->next->prev = trg->prev;
			if(trg == Head){
				Head = trg->next;	// set new Head
				std::cout<<"\n* New Head has been set\n";
			}
			if(trg == Tail){
				Tail = trg->prev;	// set new Tail
				std::cout<<"\n* New Tail Has been set\n";
			}			
		}	

		Unit *dbg = (Unit*)trg->data;

		delete dbg;		// de-allocates the memory we allocated for the data, outside of the LinkedList class
		delete trg;		// de-allocates the momory used by the list node		

		std::cout<<": ["<<numberOfNodes<<"] List node has been destroyed\n";
		numberOfNodes--;		
	}	
}

listNode* LinkedList::findNode(char uuid[UUID_LEN]){
	listNode *current = Head;			// get the first node of the list
	
	do{
		Unit *cData = getUnitData(current);
		
		std::string searchUUID(uuid);		
		if(searchUUID == cData->uuid)	// node found
			return current;
		
		current = current->next;
	}while(current != Head);

	return NULL;						// return NULL if the node hasn't been found

}

Unit* LinkedList::findUnit(char uuid[UUID_LEN]){
	listNode *temp = findNode(uuid);

	if(temp != NULL)
		return (Unit*)temp->data;

	std::cout<<"[!] Node not found\n";
	return NULL;			/// unit could not be found
}

Unit* LinkedList::getUnitData(listNode *fromNode){
	return (Unit*)fromNode->data;
}
// Adds a new user character in the list
// with the data received from the server
// it is called every time a ID_CHAR_DESCRIPTION packet arrives
void LinkedList::newUserUnit(std::string id, std::string username, std::string modelname, Vector3 initPos, float orientation){
	// allocate memory for a new userUnitData data structure, and pass the pointer to the list
	// & set up the Unit according to the server info
	PauseRender = true;

	Unit *tempUnitPtr = new Unit(id, username, modelname, initPos, uMgr);
	tempUnitPtr->setCharacterOrientation(orientation);
	tempUnitPtr->setMoveSpeed(20);

	addNode(tempUnitPtr);	// Pass the pointer to the linked list & add a list node

	std::cout<<"New char has been added to list\n\n";

	PauseRender = false;
	// the 3rd user is ready, we can start updating and rendering him in the scene	
}

void LinkedList::deleteUserUnit(std::string uuid){
	char __uuid[4];
	strcpy(__uuid, uuid.c_str());

	listNode *targetNode = findNode(__uuid);
	
	// Node found, will be destroyed
	if(targetNode != NULL)
		this->destroyNode(targetNode);		
}

void LinkedList::RenderAll(Real elapsedTime){
	listNode *current = Head;
	
	if(current == NULL){
		// list Empty: nothing to render
	}else{		
		do{
			Unit *currentUser = (Unit*)current->data;
			if(currentUser->KeepUnitAlive)
			{
				current = current->next;
				currentUser->RenderMe(elapsedTime);
			}else
			{
				current = current->next;
				destroyNode(current);			
			}
		}while(current != Head);
	}
}

void LinkedList::CheckWhoIsMoving(){
}

void LinkedList::CheckWhoShouldBeIdle(unsigned long current_time){
	/* NO NEED NOW
	listNode *current = Head;

	if(current != NULL)
	{
		do{
			Unit *currentUser = getUnitData(current);
			currentUser->checkIfShouldBeIDLE(current_time);			
			current = current->next;
		}while(current != Head);
	}
	*/
}

void LinkedList::destroyAll(){
	while(Head != NULL){
		destroyNode(Tail);
	}
}

LinkedList::~LinkedList(){	
	destroyAll();
}