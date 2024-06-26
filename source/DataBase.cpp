#include"../header/DataBase.h"

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

Database::Database() : users_table_(new UserTable()), message_table_(new MessageTable()) {}

Database::~Database() {
    delete users_table_;
    delete message_table_;
}

Database* Database::getDatabase() {
    if (!instance_) {
        std::lock_guard<std::mutex> lock(mtx);
        if (!instance_) {
            instance_ = createInstance().release();
        }
    }
    return instance_;
}


UserTable* Database::getUserTable() const
{
    return users_table_;
}

MessageTable* Database::getMessageTable() const
{
    return message_table_;
}



std::string UserTable::GetPasswordByEmail(const std::string& email) const
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

uint64_t UserTable::GetUserIdByEmail(const std::string& email) const
{
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

std::string UserTable::GetUsernameById(const uint64_t user_id) const
{
    const std::string query = "SELECT username FROM users WHERE id = $1";
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
        throw std::runtime_error("Failed to fetch username by user ID");
    }

    std::string username = PQgetvalue(res, 0, 0);
    PQclear(res);

    return username;
}

json UserTable::GetAllUsersNamesInJson() const
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

std::set<int> UserTable::GetFriendIdsByUserId(const uint64_t user_id) const
{
    const std::string query = "SELECT friend_ids FROM users WHERE id = $1";
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
        throw std::runtime_error("Failed to retrieve friend ids");
    }

    if (PQntuples(res) == 0) {
        PQclear(res);
        return json(); // Return empty JSON if no friend ids found
    }

    std::string friend_ids_str = PQgetvalue(res, 0, 0);
    std::set<int> friends;
    std::string current_number;

    for (const char& ch : friend_ids_str) {
        if (ch == '{' || ch == '}') {
            continue;
        }
        if (ch == ',') {
            if (!current_number.empty()) {
                friends.insert(stoi(current_number)); // convert str to int
                current_number.clear();
            }
        }
        else {
            current_number += ch;
        }
    }

    if (!current_number.empty()) {
        friends.insert(stoi(current_number)); //last number
    }

    PQclear(res);

    return friends;
}

void UserTable::InsertUsers(const std::string& user_name, const std::string& email, const std::string& password) const
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

void UserTable::AddFriendForUser(const uint64_t user_id, const uint64_t friend_id) const
{
    // Execute SQL query to update friend_ids for the user
    const std::string query = "UPDATE users SET friend_ids = ARRAY_APPEND(users.friend_ids, $1) WHERE id = $2";
    const std::string friend_id_str = std::to_string(friend_id);
    const std::string user_id_str = std::to_string(user_id);
    const char* param_values[2] = { friend_id_str.c_str(), user_id_str.c_str() };

    PGresult* res = PQexecParams(connection_,
        query.c_str(),
        2,
        NULL,
        param_values,
        NULL, NULL,
        0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        PQclear(res);
        throw std::runtime_error("Failed to add friend for user");
    }

    PQclear(res);
}

bool UserTable::CheckUserIdExists(const uint64_t user_id) const
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

bool UserTable::CheckEmailExists(const std::string& email) const
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

std::string UserTable::FindUserByEmail(const std::string& email) const
{
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

json UserTable::FindUserByName(const std::string& name)
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



void MessageTable::InsertMessage(const uint64_t sender_id, const uint64_t receiver_id, const std::string& message_text) const
{
    PQsetClientEncoding(connection_, "UTF8");

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

std::string MessageTable::InsertAndGetSentAt(const uint64_t sender_id, const uint64_t receiver_id, const std::string& message_text) const
{
    PQsetClientEncoding(connection_, "UTF8");

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

json MessageTable::PrintClientsMessages(const uint64_t sender_id, const uint64_t receiver_id) const
{
    PQsetClientEncoding(connection_, "UTF8");

    const std::string query = "SELECT sender_id, receiver_id, message_text, sent_at FROM messages WHERE (sender_id = $1 AND receiver_id = $2) OR (sender_id = $2 AND receiver_id = $1) ORDER BY sent_at DESC";

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
        message["sent_at"] = PQgetvalue(res, i, 3);

        messages.push_back(message); // Add the message JSON object to the array
    }

    PQclear(res);

    return messages;
}