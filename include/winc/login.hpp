#ifndef WINC_LOGIN_HPP
#define WINC_LOGIN_HPP

#include <string>
#include <memory>

namespace winc {

class login {
public:
	login(const std::string &username, const std::string &password);
	void grant_access(const std::string &path);

	struct impl;
private:
	std::shared_ptr<impl> _impl;
};

}

#endif