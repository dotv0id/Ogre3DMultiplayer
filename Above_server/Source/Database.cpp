#include "Database.h"


Database::Database(){
}

Database::~Database(){
	delete dbConn;
}

// connect to the database
bool Database::initializeDatabase(){	
	dbConn = new mysqlpp::Connection(false);
	bool rslt = dbConn->connect(gVar->db, gVar->dbServerIP, gVar->dbUser, gVar->dbPwd, gVar->dbServerPort);
	
	return rslt;
}

// Checks if the pair ussername-password exists in the user database
// if it exists returns true, else... false
bool Database::CheckUserLogin(std::string uname, std::string pwd, userData* uData){

	cout<< "  % username: " << uname << ", password: " << pwd << "\n";

	// Write the query to execute	
	mysqlpp::Query query = dbConn->query();
	query << "SELECT * FROM " << gVar->tableName << " WHERE username = " << mysqlpp::quote << uname << " AND password = " << mysqlpp::quote << pwd;	

	mysqlpp::StoreQueryResult queryRslt = query.store();	// store the query result

	// user exists in the database & the password is correct
	if(!queryRslt.empty()){
		//cout<<"\n numOfRows: "<<queryRslt.num_rows()<<"\n";
		
		// Save the data for this user, in a userData struct
		// TODO: multiple characters for one user
		for(int i = 0; i< queryRslt.num_rows(); i++){
			
			uData->uuid			= queryRslt[i]["uuid"];
			uData->username		= queryRslt[i]["username"];
			uData->charModel	= queryRslt[i]["modelname"];

			uData->range		= queryRslt[i]["range"];
			uData->moveSpeed	= queryRslt[i]["movespeed"];
			
			// Last saved position of our character
			uData->userPos.x	= queryRslt[i]["world_x"];
			uData->userPos.y	= queryRslt[i]["world_y"];
			uData->userPos.z	= queryRslt[i]["world_z"];
			uData->orientation	= queryRslt[i]["world_orientation_y"];
		}

		return true;
	}
	
	return false;
}

userData* Database::getUserDataFromDB(std::string uname, std::string pwd){
	userData *tempUser;	
	return tempUser;
}

void Database::UpdatePosition(char uuid[UUID_LEN], myPosition *newPos){
	
	std::string searchUUID(uuid);

	// Write the query to execute
	mysqlpp::Query query = dbConn->query();
	query << "UPDATE " << gVar->tableName << " SET world_x = " << newPos->x << ", world_z = " << newPos->z <<" WHERE uuid = " << mysqlpp::quote << searchUUID;

	mysqlpp::StoreQueryResult queryRslt = query.store();	// store the query result

	std::cout<< "  [DB] " << uuid << ": pozition updated\n";
}

void Database::UpdateOrientation(char uuid[UUID_LEN], float newOrientation){
	std::string searchUUID(uuid);

	// Write the query to execute
	mysqlpp::Query query = dbConn->query();
	query << "UPDATE " << gVar->tableName << " SET world_orientation_y = " << newOrientation <<" WHERE uuid = " << mysqlpp::quote << searchUUID;

	mysqlpp::StoreQueryResult queryRslt = query.store();	// store the query result

	std::cout<< "  [DB] " << uuid << ": orientation updated\n";
}