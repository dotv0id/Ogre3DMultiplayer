#ifndef _DATABASE_H
#define _DATABASE_H


#include <mysql++.h>
#include "GlobalVariables.h"
#include "DataTypes.h"

class Database{
private:
	GlobalVariables		*gVar;
	mysqlpp::Connection *dbConn;	

public:
	Database();
	~Database();

	bool initializeDatabase();
	bool CheckUserLogin(std::string uname, std::string pwd, userData* uData);
	userData* getUserDataFromDB(std::string uname, std::string pwd);

	void UpdatePosition(char uuid[UUID_LEN], myPosition *newPos);
	void UpdateOrientation(char uuid[UUID_LEN], float newOrientation);

	// set functions
	void setGlobalVariablesPtr(GlobalVariables *g){gVar = g;};

};

#endif

