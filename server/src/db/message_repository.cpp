#include "messenger/db/message_repository.hpp"

namespace messenger::db {

namespace {

// ISO 8601 in UTC with milliseconds; JavaScript's Date parses it as is.
constexpr const char* kSentAt = "to_char(sent_at AT TIME ZONE 'UTC', 'YYYY-MM-DD\"T\"HH24:MI:SS.MS\"Z\"')";

Message messageFromRow(const Result& result, int row) {
    return Message{result.int64(row, 0), result.int64(row, 1), result.int64(row, 2), result.text(row, 3),
                   result.text(row, 4)};
}

}  // namespace

Message MessageRepository::insert(UserId from, UserId to, const std::string& body) {
    const Result result =
        connection_.exec(std::string("INSERT INTO messages (sender_id, receiver_id, body) VALUES ($1, $2, $3) "
                                     "RETURNING id, sender_id, receiver_id, body, ") +
                             kSentAt,
                         {std::to_string(from), std::to_string(to), body});
    return messageFromRow(result, 0);
}

std::vector<Message> MessageRepository::conversation(UserId userId, UserId peerId, std::optional<MessageId> beforeId,
                                                     int limit) {
    // LEAST/GREATEST match the messages_conversation_idx index.
    const Result result = connection_.exec(
        std::string("SELECT id, sender_id, receiver_id, body, ") + kSentAt +
            " FROM messages "
            "WHERE LEAST(sender_id, receiver_id) = LEAST($1::bigint, $2::bigint) "
            "AND GREATEST(sender_id, receiver_id) = GREATEST($1::bigint, $2::bigint) "
            "AND ($3::bigint IS NULL OR id < $3::bigint) "
            "ORDER BY id DESC LIMIT $4",
        {std::to_string(userId), std::to_string(peerId),
         beforeId ? Param(std::to_string(*beforeId)) : Param(std::nullopt), std::to_string(limit)});

    std::vector<Message> messages;
    messages.reserve(static_cast<std::size_t>(result.rows()));
    for (int row = 0; row < result.rows(); ++row) {
        messages.push_back(messageFromRow(result, row));
    }
    return messages;
}

}  // namespace messenger::db
