#pragma once


class Server
{
public:
	explicit Server(const uint16_t port): cnt_user_(1),port_(port){}

	void run();

private:

	struct UserData
	{
		uint64_t user_id;
		std::string name = "NO_NAME";
	};

	//id пользователя(атомарный для многопоточности)
	std::atomic_ulong cnt_user_;
	std::uint16_t port_ = 9001;
};

