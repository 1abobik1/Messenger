#pragma once

#include"CommonConst.h"

#include <string>
#include <libpq-fe.h>
#include <memory>

//RAII for PGconn*
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

class Database {
private:
    static Database* instance_;
    DBConnection connection_;

    Database() = default;

public:
    static Database* getInstance();

    void InsertUsers(const std::string& user_name, const std::string& email, const std::string& password) const;

    bool CheckEmailExists(const std::string& email) const;

	std::string GetPasswordByEmail(const std::string& email) const;

};
