#include"../header/DataBase.h"
#include"../header/TimeUtils.h"

#include <mutex>
#include <stdexcept>
#include <iostream>

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

//--- FOR THE USERS TABLE ---//

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

bool Database::CheckUserIdExists(const uint64_t user_id) const
{
    const std::string query = "SELECT EXISTS(SELECT 1 FROM users WHERE id = $1)";
    const std::string user_id_str = std::to_string(user_id);
    const char* param_values[1] = { user_id_str.c_str() };

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

uint64_t Database::GetUserIdByEmail(const std::string& email) const {
    const std::string query = "SELECT id FROM users WHERE email = $1";
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
        throw std::runtime_error("Failed to fetch user ID by email");
    }

    if (PQntuples(res) == 0) {
        PQclear(res);
        throw std::runtime_error("No matching user found for the provided email");
    }

    int64_t user_id = std::stoi(PQgetvalue(res, 0, 0));
    PQclear(res);

    return user_id;
}

json Database::GetAllUsersNamesInJson() const
{
    json usersJson;

    const std::string query = "SELECT username FROM users";
    PGresult* res = PQexec(connection_, query.c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        PQclear(res);
        throw std::runtime_error("Failed to fetch user names");
    }

    for (int i = 0; i < PQntuples(res); ++i) {
        usersJson.push_back(PQgetvalue(res, i, 0));
    }

    PQclear(res);

    return usersJson;
}

std::string Database::FindUserByEmail(const std::string& email) const {
    const std::string query = "SELECT username FROM users WHERE email = $1";
    const char* param_values[1] = { email.c_str() };

    PGresult* res = PQexecParams(connection_,
        query.c_str(),
        1,
        NULL,
        param_values,
        NULL, NULL,
        0);

    if (PQntuples(res) == 0) {
        PQclear(res);
        return "";
    }

    std::string username = PQgetvalue(res, 0, 0);
    PQclear(res);

    return username;
}

json Database::FindUserByName(const std::string& name)
{
    json usersJson;

    const std::string query = "SELECT username FROM users WHERE username = $1";
    const char* param_values[1] = { name.c_str() };

    PGresult* res = PQexecParams(connection_,
        query.c_str(),
        1,
        NULL,
        param_values,
        NULL, NULL,
        0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        PQclear(res);
        return usersJson;
    }

    if (PQntuples(res) == 0) {
        PQclear(res);
        return usersJson;
    }

    for (int i = 0; i < PQntuples(res); ++i) {
        usersJson.push_back(PQgetvalue(res, i, 0));
    }

    PQclear(res);

    return usersJson;
}

//--- FOR THE MESSAGES TABLE---//

void Database::InsertMessage(const uint64_t sender_id, const uint64_t receiver_id, const std::string& message_text) const {

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

std::string Database::InsertAndGetSentAt(const uint64_t sender_id, const uint64_t receiver_id, const std::string& message_text) const {
    const std::string query = "INSERT INTO messages (sender_id, receiver_id, message_text, sent_at) VALUES ($1, $2, $3, NOW()) RETURNING sent_at";

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

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        PQclear(res);
        throw std::runtime_error("Failed to insert message or fetch sent time");
    }

    if (PQntuples(res) == 0) {
        PQclear(res);
        throw std::runtime_error("No sent time returned");
    }

    std::string sent_at = PQgetvalue(res, 0, 0);
    PQclear(res);

    return sent_at;
}

json Database::PrintClientsMessages(const uint64_t sender_id, const uint64_t receiver_id) const {
    const std::string query = "SELECT sender_id, receiver_id, message_text, sent_at FROM messages WHERE sender_id = $1 AND receiver_id = $2 ORDER BY sent_at DESC";

    const std::string sender_id_str = std::to_string(sender_id);
    const std::string receiver_id_str = std::to_string(receiver_id);

    const char* param_values[2] = { sender_id_str.c_str(), receiver_id_str.c_str() };

    PGresult* res = PQexecParams(connection_,
        query.c_str(),
        2,           
        NULL,        
        param_values,
        NULL,     
        NULL,     
        0);       

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        PQclear(res);
        throw std::runtime_error("Failed to retrieve messages");
    }

    const int rows = PQntuples(res);
    json messages = json::array(); // Create a JSON array to store the messages

    for (int i = 0; i < rows; ++i) {
        json message;
        message["sender_id"] = std::stoull(PQgetvalue(res, i, 0));
        message["receiver_id"] = std::stoull(PQgetvalue(res, i, 1));
        message["message_text"] = PQgetvalue(res, i, 2);
        message["sent_at"] = time_utils::ExtractTime(PQgetvalue(res, i, 3));

        messages.push_back(message); // Add the message JSON object to the array
    }

    PQclear(res);

    return messages;
}