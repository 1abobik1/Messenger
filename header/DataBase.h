#pragma once

#include"CommonConst.h"

#include <nlohmann/json.hpp>

#include <string>
#include <libpq-fe.h>
#include <memory>

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

class Database {
private:
    static Database* instance_;
    DBConnection connection_;

    Database() = default;

    static std::unique_ptr<Database> createInstance();

public:
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;
    ~Database() = default;

    static Database* getInstance();


    void InsertUsers(const std::string& user_name, const std::string& email, const std::string& password) const;

    bool CheckEmailExists(const std::string& email) const;

	std::string GetPasswordByEmail(const std::string& email) const;

    void InsertMessage(int sender_id, int receiver_id, const std::string& message_text) const;

    std::string GetSentAt(int message_id) const;

    int64_t GetUserIdByEmail(const std::string& email) const;

    json GetAllUsersNamesInJson() const;

    std::string FindUserByEmail(const std::string& name) const;
};
