#include <system_error>
#include <string>
#include <Windows.h>
#include "window_station.hpp"

using namespace std;
using namespace winc;

namespace {

HANDLE get_process_window_station()
{
	HANDLE result = GetProcessWindowStation();
	if (!result) {
		throw system_error(GetLastError(), system_category());
	}
	return result;
}

}

window_station::window_station()
	: user_object(get_process_window_station()), _got_name(false)
{}

std::string window_station::name()
{
	if (!_got_name) {
		_name = user_object::name();
		_got_name = true;
	}

	return _name;
}