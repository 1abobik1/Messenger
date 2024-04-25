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

void Database::InsertMessage(const int sender_id, const int receiver_id, const std::string& message_text) const {

    const std::string query = "INSERT INTO messages (sender_id, receiver_id, message_text, sent_at) VALUES ($1, $2, $3, NOW())";

    const std::string sender_id_str = std::to_string(sender_id);
    const std::string receiver_id_str = std::to_string(receiver_id);

    const char* param_values[3] = { sender_id_str.c_str(), receiver_id_str.c_str(), message_text.c_str() };

    PGresult* res = PQexecParams(connection_,
        query.c_str(),
        3,
        NULL,
        param_values,
        NULL, NULL,
        0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        PQclear(res);
        throw std::runtime_error("Failed to insert message");
    }

    PQclear(res);
}

std::string Database::GetSentAt(const int message_id) const {
    const std::string query = "SELECT sent_at FROM messages WHERE message_id = $1";

    const std::string message_id_str = std::to_string(message_id);

    const char* param_values[1] = {
        message_id_str.c_str()
    };

    PGresult* res = PQexecParams(connection_,
        query.c_str(),
        1,
        NULL,
        param_values,
        NULL, NULL,
        0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        PQclear(res);
        throw std::runtime_error("Failed to fetch sent time");
    }

    if (PQntuples(res) == 0) {
        PQclear(res);
        throw std::runtime_error("No matching message found");
    }

    std::string sent_at = PQgetvalue(res, 0, 0);
    PQclear(res);

    return sent_at;
}
