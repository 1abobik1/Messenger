#pragma once

#include <string>
#include <string_view>

namespace JsonObj
{
	const std::string COMMAND = "command";
	// ����ҧ�֧ߧڧ�
	const std::string MESSAGE = "message";
	// �ݧڧ�ߧ��� ����ҧ�֧ߧڧ�
	const std::string PRIVATE_MSG = "private_msg";
	// ���ҧݧڧ�ߧ�� ����ҧ�֧ߧڧ�
	const std::string PUBLIC_MSG = "public_msg";

	// ���ݧ��ѧ�֧ݧ� ����ҧ�֧ߧڧ�
	const std::string RECEIVER_ID = "receiver_id";
	// �ܧ�� �����ѧӧڧ� ����ҧ�֧ߧڧ�
	const std::string USER_ID_FROM = "user_id_from";

	// �ڧާ� ���ݧ�٧�ӧѧ�֧ݧ�
	const std::string SET_NAME = "set_name";
	const std::string NEW_NAME = "new_name";
}

namespace PathHTML
{
	// ����� �� ��ѧۧݧ� HTML-�ܧݧڧ֧ߧ��
	constexpr std::string_view CLIENT_HTML_FILE_PATH = "client.htm";
}

namespace ConnDB
{
	// ����� �� �ҧ�
	constexpr std::string_view CONNECTION_DB = "host=localhost dbname=Messenger_DB user=postgres password=Zopa_kek12";
}
