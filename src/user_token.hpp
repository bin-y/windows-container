#ifndef USER_TOKEN_HPP
#define USER_TOKEN_HPP

#include <string>
#include <vector>
#include <Windows.h>
#include "safe_handle.hpp"

namespace winc {

class user_token : protected safe_handle {
public:
	user_token(const std::string &username, const std::string &password);
	std::vector<char> sid();
};

}

#endif