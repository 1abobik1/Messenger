#pragma once

#include <string>
#include <string_view>

namespace JsonObj
{
	const std::string COMMAND = "command";
	// §ã§à§à§Ò§ë§Ö§ß§Ú§ñ
	const std::string MESSAGE = "message";
	// §Ý§Ú§é§ß§í§í§Ö §ã§à§à§Ò§ë§Ö§ß§Ú§ñ
	const std::string PRIVATE_MSG = "private_msg";
	// §á§å§Ò§Ý§Ú§é§ß§í§Ö §ã§à§à§Ò§ë§Ö§ß§Ú§ñ
	const std::string PUBLIC_MSG = "public_msg";

	// §á§à§Ý§å§é§Ñ§ä§Ö§Ý§î §ã§à§à§Ò§ë§Ö§ß§Ú§ñ
	const std::string RECEIVER_ID = "receiver_id";
	// §Ü§ä§à §à§ä§á§â§Ñ§Ó§Ú§Ý §ã§à§à§Ò§ë§Ö§ß§Ú§Ö
	const std::string USER_ID_FROM = "user_id_from";

	// §Ú§Þ§ñ §á§à§Ý§î§Ù§à§Ó§Ñ§ä§Ö§Ý§ñ
	const std::string SET_NAME = "set_name";
	const std::string NEW_NAME = "new_name";
}

namespace PathHTML
{
	// §á§å§ä§î §Ü §æ§Ñ§Û§Ý§å HTML-§Ü§Ý§Ú§Ö§ß§ä§Ñ
	constexpr std::string_view CLIENT_HTML_FILE_PATH = "client.htm";
}

namespace ConnDB
{
	// §á§å§ä§î §Ü §Ò§Õ
	constexpr std::string_view CONNECTION_DB = "host=localhost dbname=Messenger_DB user=postgres password=Zopa_kek12";
}
