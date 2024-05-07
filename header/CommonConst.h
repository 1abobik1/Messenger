#pragma once

#include <string>
#include <string_view>

namespace JsonChat
{
	const std::string COMMAND = "command";

	const std::string MESSAGE = "message";

	const std::string PRIVATE_MSG = "private_msg";

	const std::string PUBLIC_MSG = "public_msg";

	const std::string RECEIVER_ID = "receiver_id"; // получатель 

	const std::string SENDER_ID = "sender_id"; // отправитель

	const std::string SET_NAME = "set_name";

	const std::string NAME = "new_name";

	const std::string STATUS = "status";

	const std::string ONLINE = "online";

	const std::string SENT_AT = "sent_at";
}

namespace PathDB
{
	// path to the DB
	constexpr std::string_view CONNECTION_DB = "host=localhost dbname=Messenger_DB user=postgres password=Zopa_kek12";
}
