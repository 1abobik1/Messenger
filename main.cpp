#define _SILENCE_ALL_CXX23_DEPRECATION_ARNINGS 
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <locale.h>

#include <libpq-fe.h>

#include "bcrypt-cpp/include/bcrypt.h"
#include "header/CommonConst.h"
#include "header/Server.h"


int main() {
	setlocale(LC_ALL, "en_US.UTF-8");

	/*std::string password = "top_secret";

	std::string hash = "$2a$12$DT1HSQOsRmu5qfAsD3t2yOKQgX.BvjL8e/KDxWylxPqKCwu4CS.pS";

	std::cout << "\"" << password << "\" : " << bcrypt::validatePassword(password, hash) << std::endl;
*/

	Server server(9000);

	server.run();

	return 0;
}
