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
	//std::cout<<"  * LinkedList initialized!\n";
}

void LinkedList::addNode(userData *udata){

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

	numberOfNodes++;				// increase the number of nodes
	std::cout<<"\n  [LL]"<<" List node has been added ("<<numberOfNodes<<")\n";
}

// ... where trg the node to be destroyed
// you must call a function like find, to get the pointer to the node u want to delete
void LinkedList::destroyNode(listNode *trg){

	if(numberOfNodes > 0){
		// There are still elements in the list
		if(Head == Tail){
			// This is the last Node in the list
			Head = Tail = NULL;			
			std::cout<<"\n  [LL] last node deleted\n";			
		}else{
			// There are at least 2 Nodes in the list
			trg->prev->next = trg->next;
			trg->next->prev = trg->prev;
			if(trg == Head){
				Head = trg->next;	// set new Head
				std::cout<<"\n  * New Head has been set\n";
			}
			if(trg == Tail){
				Tail = trg->prev;	// set new Tail
				std::cout<<"\n  * New Tail Has been set\n";
			}			
		}	

		userData *dbg = getUnitData(trg);

		delete dbg;	// de-allocates the memory we allocated for the data, outside of the LinkedList class
		delete trg;			// de-allocates the momory used by the list node		

		std::cout<<"  ["<<numberOfNodes<<"] List node has been destroyed\n";
		numberOfNodes--;		
	}	
}

listNode* LinkedList::findNode(SystemAddress clientID){
	listNode *current = Head;

	if(current != NULL){
		do{
			userData *cData = getUnitData(current);		

			if(clientID == cData->clientID){
				/*
				char thisIp[50];
				cData->clientID.ToString(true, thisIp);
				std::cout<<"[.] node found: "<<thisIp<<"\n";
				*/

				return current;
			}
			current = current->next;		
		}while(current != Head);
	}

	return NULL;
}

listNode* LinkedList::findNode(char uuid[UUID_LEN]){
	listNode *current = Head;			// get the first node of the list
	
	// if list not empty, search
	if(Head != NULL)
	{
		do{
			userData *cData = getUnitData(current);
		
			std::string searchUUID(uuid);		
			if(searchUUID == cData->uuid)	// node found
				return current;
		
			current = current->next;
		}while(current != Head);
	}

	return NULL;						// return NULL if the node hasn't been found
}

userData* LinkedList::findUnit(SystemAddress clientID){
	listNode *temp = findNode(clientID);

	if(temp != NULL)
		return (userData*)temp->data;

	return NULL;
}

userData* LinkedList::findUnit(char uuid[UUID_LEN]){

	listNode *temp = findNode(uuid);

	if(temp != NULL)
		return (userData*)temp->data;

	std::cout<<"  [!] Node not found\n";
	return NULL;			/// unit could not be found
}

bool LinkedList::removeUnitFromList(SystemAddress clientID){

	listNode *trgNode = findNode(clientID);

	if(trgNode != NULL){
		destroyNode(trgNode);
		return true;
	}

	std::cout<<"  [!] Node not found\n";
	return false;
}

bool LinkedList::removeUnitFromList(char uuid[UUID_LEN]){

	listNode *trgNode = findNode(uuid);

	if(trgNode != NULL){		
		destroyNode(trgNode);
		return true;
	}

	std::cout<<"  [!] Node not found\n";
	return false;
}

userData* LinkedList::getUnitData(listNode *fromNode){
	return (userData*)fromNode->data;
}

bool LinkedList::isUserOnline(char uuid[UUID_LEN]){
	listNode *_t = findNode(uuid);
	
	// user is not Online
	if(_t == NULL)
		return false;		

	return true;
}

void LinkedList::calculate_pozition(){
	listNode *current = Head;

	if(current != NULL)
	{
		do{
			userData* thisUser = getUnitData(current);
		
			if(thisUser->isMoving){
				thisUser->userPos.x += 0.44;
			}

			current = current->next;		
		}while(current != Head);
	}
}

// saves all changed users to the db
// and performs minimal cleanup (if keepAlive = false, delete user...)
void LinkedList::SaveUsersToDb(Database *dbPtr){
	before = RakNet::GetTime();

	listNode *current = Head;

	for(int i = 0; i<numberOfNodes; i++)
	{
		userData* _user = getUnitData(current);
		// get current user's uuid 
		char uuid[4];
		strcpy(uuid, _user->uuid.c_str());

		if(!_user->savedInDb)
		{				
			dbPtr->UpdatePosition(uuid, &_user->userPos);
			dbPtr->UpdateOrientation(uuid, _user->orientation);
			_user->savedInDb = true;
			std::cout<<"  > user " << uuid <<" has been saved to DB...\n";
		}

		// check if the current user, should be in the list
		if(!_user->keepAlive){
			removeUnitFromList(uuid);
			// send destroyUnitPacket to all
		}

		// get the next element of the list
		current = current->next;
	}

	// how many ms the system needs to run this?
	after = RakNet::GetTime();
	if((after-before) > 0)
		std::cout<<"  > SaveUsersToDb(*) was executed in "<<after-before<<" MS\n\n";
}

void LinkedList::destroyAll(){
	while(Head != NULL){
		destroyNode(Tail);
	}
}

LinkedList::~LinkedList(){	
	destroyAll();
}