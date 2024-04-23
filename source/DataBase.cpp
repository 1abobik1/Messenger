#include"../header/DataBase.h"

#include <mutex>
#include <stdexcept>

std::mutex mtx;

DBConnection::DBConnection() : connection_(PQconnectdb(PathDB::CONNECTION_DB.data()))
{
	if (PQstatus(connection_) != CONNECTION_OK) {
		PQfinish(connection_);
		connection_ = nullptr;
		throw std::runtime_error(PQerrorMessage(connection_));
	}
}

DBConnection::~DBConnection()
{
    PQfinish(connection_);
}


//---------------initialization instance_---------------//
Database* Database::instance_ = nullptr;

std::unique_ptr<Database> Database::createInstance() {
    return std::unique_ptr<Database>(new Database());
}

Database* Database::getInstance() {
    if (!instance_) {
        std::lock_guard<std::mutex> lock(mtx);
        if (!instance_) {
            instance_ = createInstance().release();
        }
    }
    return instance_;
}


void Database::InsertUsers(const std::string& user_name, const std::string& email, const std::string& password) const
{

    const std::string query = "INSERT INTO users (username, email, password) VALUES ($1, $2, $3)";
    const char* param_values[3] = { user_name.c_str(), email.c_str(), password.c_str() };
    PGresult* res = PQexecParams(connection_,
        query.c_str(),
        3,      
        NULL,
        param_values,
        NULL,   
        NULL,    
        0);      

    PQclear(res);
}

bool Database::CheckEmailExists(const std::string& email) const
{
    const std::string query = "SELECT EXISTS(SELECT 1 FROM users WHERE email = $1)";
    const char* param_values[1] = { email.c_str() };

    PGresult* res = PQexecParams(connection_,
        query.c_str(),
        1,           
        NULL,         
        param_values,
        NULL, NULL,   
        0);           

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        PQclear(res);
        return false; 
    }

    const bool exists = PQgetvalue(res, 0, 0)[0] == 't'; // 't' == true

    PQclear(res);
    return exists;
}

std::string Database::GetPasswordByEmail(const std::string& email) const
{
    const std::string query = "SELECT password FROM users WHERE email = $1";
    const char* param_values[1] = { email.c_str() };

    PGresult* res = PQexecParams(connection_,
        query.c_str(),
        1,
        NULL,
        param_values,
        NULL, NULL,
        0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        PQclear(res);
        return "";
    }

    std::string password = PQgetvalue(res, 0, 0);

    PQclear(res);
    return password;
}
