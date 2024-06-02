#pragma once

#include"CommonConst.h"

#include <nlohmann/json.hpp>

#include <string>
#include <libpq-fe.h>
#include <memory>
#include <set>

// RAII for PGconn*
class DBConnection {
private:
    PGconn* connection_;

public:

    DBConnection();

    ~DBConnection();

    operator PGconn* () const
    {
        return connection_;
    }

};

using json = nlohmann::json;

class UserTable;
class MessageTable;

// the class is not created directly, but is created through the getDatabase() method
class Database {
private:
    static Database* instance_;
    static std::unique_ptr<Database> createInstance();

    UserTable* users_table_;
    MessageTable* message_table_;

    Database();
public:
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;
    ~Database();

    static Database* getDatabase();

    UserTable* getUserTable() const;
    MessageTable* getMessageTable() const;
};

// using this class only through the Database class, direct interaction with this class is prohibited
class UserTable {
private:
    DBConnection connection_;
    UserTable() = default;

    friend Database;
public:

    std::string GetPasswordByEmail(const std::string& email) const;
    uint64_t GetUserIdByEmail(const std::string& email) const;
    std::string GetUsernameById(const uint64_t user_id) const;
    json GetAllUsersNamesInJson() const;
    std::set<int> GetFriendIdsByUserId(const uint64_t user_id) const;

    void InsertUsers(const std::string& user_name, const std::string& email, const std::string& password) const;
    
    void AddFriendForUser(const uint64_t user_id, const uint64_t friend_id) const;

    bool CheckUserIdExists(const uint64_t user_id) const;
    bool CheckEmailExists(const std::string& email) const;

    std::string FindUserByEmail(const std::string& email) const;
    json FindUserByName(const std::string& name);
};

// using this class only through the Database class, direct interaction with this class is prohibited
class MessageTable {
private:
    DBConnection connection_;
    MessageTable() = default;

    friend Database;
public:

    void InsertMessage(const uint64_t sender_id, const uint64_t receiver_id, const std::string& message_text) const;
    std::string InsertAndGetSentAt(const uint64_t sender_id, const uint64_t receiver_id, const std::string& message_text) const;

    json PrintClientsMessages(const uint64_t sender_id, const uint64_t receiver_id) const;
};