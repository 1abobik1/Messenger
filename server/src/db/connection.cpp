#include "messenger/db/connection.hpp"

namespace messenger::db {

Connection::Connection(std::string conninfo) : conninfo_(std::move(conninfo)), conn_(nullptr, &PQfinish) {
    connect();
}

void Connection::connect() {
    conn_.reset(PQconnectdb(conninfo_.c_str()));
    if (PQstatus(conn_.get()) != CONNECTION_OK) {
        throw DbError("cannot connect to PostgreSQL: " + std::string(PQerrorMessage(conn_.get())), {});
    }
    PQsetClientEncoding(conn_.get(), "UTF8");
}

Result Connection::exec(std::string_view sql, const std::vector<Param>& params) {
    if (PQstatus(conn_.get()) != CONNECTION_OK) {
        PQreset(conn_.get());
        if (PQstatus(conn_.get()) != CONNECTION_OK) {
            connect();
        }
    }

    std::vector<const char*> values;
    values.reserve(params.size());
    for (const Param& param : params) {
        values.push_back(param ? param->c_str() : nullptr);
    }

    const std::string query(sql);
    PGresult* raw = PQexecParams(conn_.get(), query.c_str(), static_cast<int>(values.size()), nullptr,
                                 values.data(), nullptr, nullptr, 0);
    if (raw == nullptr) {
        throw DbError("query failed: " + std::string(PQerrorMessage(conn_.get())), {});
    }

    const ExecStatusType status = PQresultStatus(raw);
    if (status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK) {
        const char* state = PQresultErrorField(raw, PG_DIAG_SQLSTATE);
        DbError error("query failed: " + std::string(PQresultErrorMessage(raw)),
                      state != nullptr ? state : "");
        PQclear(raw);
        throw error;
    }
    return Result(raw);
}

}  // namespace messenger::db
