#pragma once

#include <libpq-fe.h>

#include <cstdint>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace messenger::db {

class DbError : public std::runtime_error {
public:
    DbError(const std::string& message, std::string sqlState)
        : std::runtime_error(message), sqlState_(std::move(sqlState)) {}

    // Five-character SQLSTATE, e.g. "23505" for a unique violation; empty if unknown.
    const std::string& sqlState() const { return sqlState_; }
    bool isUniqueViolation() const { return sqlState_ == "23505"; }
    bool isForeignKeyViolation() const { return sqlState_ == "23503"; }

private:
    std::string sqlState_;
};

// Query parameter: std::nullopt is sent as SQL NULL.
using Param = std::optional<std::string>;

// Owns a PGresult and gives typed access to its cells.
class Result {
public:
    explicit Result(PGresult* result) : result_(result, &PQclear) {}

    int rows() const { return PQntuples(result_.get()); }
    bool isNull(int row, int column) const { return PQgetisnull(result_.get(), row, column) != 0; }
    std::string text(int row, int column) const { return PQgetvalue(result_.get(), row, column); }
    std::int64_t int64(int row, int column) const {
        return std::stoll(PQgetvalue(result_.get(), row, column));
    }

private:
    std::unique_ptr<PGresult, decltype(&PQclear)> result_;
};

// One libpq connection. The server is single-threaded (uWebSockets event loop), so it is shared by all
// repositories.
class Connection {
public:
    explicit Connection(std::string conninfo);

    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;

    // Runs a parameterized query ($1, $2, ...). If the connection was lost, reconnects and retries once.
    // Throws DbError if the query fails.
    Result exec(std::string_view sql, const std::vector<Param>& params = {});

private:
    void connect();

    std::string conninfo_;
    std::unique_ptr<PGconn, decltype(&PQfinish)> conn_;
};

}  // namespace messenger::db
