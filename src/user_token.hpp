#ifndef USER_TOKEN_HPP
#define USER_TOKEN_HPP

#include <string>
#include <vector>
#include <Windows.h>
#include "non_copyable.hpp"

namespace winc {

class user_token : non_copyable {
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