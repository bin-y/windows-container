#ifndef _WINDOW_STATION_HPP
#define _WINDOW_STATION_HPP

#include <string>
#include <Windows.h>
#include "user_object.hpp"

namespace winc {

class window_station : public user_object {
public:
	window_station();
	/* override */ std::string name();

private:
	bool _got_name;
	std::string _name;
};

}

#endif