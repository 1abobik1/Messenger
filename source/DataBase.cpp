#include"../header/DataBase.h"

#include <stdexcept>


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

Database* Database::getInstance()
{
    if (!instance_) {
        instance_ = new Database();
    }
    return instance_;
}

void Database::insert_user(const std::string& user_name, const std::string& email, const std::string& password) const
{
    // Составление SQL-запроса
    const std::string query = "INSERT INTO users (username, email, password) VALUES ($1, $2, $3)";
    const char* param_values[3] = { user_name.c_str(), email.c_str(), password.c_str() };
    PGresult* res = PQexecParams(connection_,
        query.c_str(),
        3,       // cnt param_values
        NULL,
        param_values,
        NULL,   
        NULL,    // форматы параметров
        0);      // результат в текстовом формате
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        PQclear(res);
        throw std::runtime_error(PQerrorMessage(connection_));
    }
    PQclear(res);
}
