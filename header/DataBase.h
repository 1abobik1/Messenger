#pragma once

#include"CommonConst.h"

#include <string>
#include <libpq-fe.h>
#include <memory>

//RAII
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

    void insert_user(const std::string& user_name, const std::string& email, const std::string& password) const;
    
};
