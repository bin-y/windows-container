#ifndef USER_TOKEN_HPP
#define USER_TOKEN_HPP

#include <string>
#include <vector>
#include <Windows.h>

namespace winc {

class user_token {
public:
	user_token(const std::string &username, const std::string &password);
	~user_token();
	HANDLE handle();
	std::vector<char> sid();

private:
	HANDLE _handle;
};

}

#endif