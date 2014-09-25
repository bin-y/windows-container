#ifndef USER_TOKEN_HPP
#define USER_TOKEN_HPP

#include <string>
#include <vector>
#include <Windows.h>
#include "non_copyable.hpp"
#include "safe_handle.hpp"

namespace winc {

class user_token : non_copyable {
public:
	user_token(const std::string &username, const std::string &password);
	std::vector<char> sid() const;
private:
	safe_handle _handle;
};

}

#endif