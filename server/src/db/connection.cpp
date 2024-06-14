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
    std::vector<const char*> values;
    values.reserve(params.size());
    for (const Param& param : params) {
        values.push_back(param ? param->c_str() : nullptr);
    }
    const std::string query(sql);

    // A dropped connection (e.g. PostgreSQL restarted) is only noticed when a query fails:
    // reconnect and try once more.
    for (int attempt = 0;; ++attempt) {
        if (PQstatus(conn_.get()) != CONNECTION_OK) {
            PQreset(conn_.get());
            if (PQstatus(conn_.get()) != CONNECTION_OK) {
                connect();
            }
        }

        PGresult* raw = PQexecParams(conn_.get(), query.c_str(), static_cast<int>(values.size()), nullptr,
                                     values.data(), nullptr, nullptr, 0);
        const ExecStatusType status = raw != nullptr ? PQresultStatus(raw) : PGRES_FATAL_ERROR;
        if (status == PGRES_COMMAND_OK || status == PGRES_TUPLES_OK) {
            return Result(raw);
        }

        const bool connectionLost = PQstatus(conn_.get()) == CONNECTION_BAD;
        if (connectionLost && attempt == 0) {
            PQclear(raw);
            continue;
        }

        const char* state = raw != nullptr ? PQresultErrorField(raw, PG_DIAG_SQLSTATE) : nullptr;
        const std::string message = raw != nullptr ? PQresultErrorMessage(raw) : PQerrorMessage(conn_.get());
        PQclear(raw);
        throw DbError("query failed: " + message, state != nullptr ? state : "");
    }
}

}  // namespace messenger::db
