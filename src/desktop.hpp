#ifndef _DESKTOP_HPP
#define _DESKTOP_HPP

#include <string>
#include <Windows.h>
#include "user_object.hpp"

namespace winc {

class desktop : public user_object {
public:
	desktop(const std::string &name);
	~desktop();
	/* override */ std::string name();

private:
	std::string _name;
};

}

#endif