#include <system_error>
#include <string>
#include <Windows.h>
#include "desktop.hpp"

using namespace std;
using namespace winc;

namespace {

HANDLE create_desktop(const string &name)
{
	HANDLE result = ::CreateDesktopA(name.c_str(), NULL, NULL, 0,
		DESKTOP_READOBJECTS | DESKTOP_CREATEWINDOW | DESKTOP_WRITEOBJECTS |
		READ_CONTROL | WRITE_DAC | DESKTOP_SWITCHDESKTOP, 0);
	if (!result) {
		throw system_error(::GetLastError(), system_category());
	}
	return result;
}

}

desktop::desktop(const string &name)
	: user_object(create_desktop(name)), _name(name)
{}

desktop::~desktop()
{
	::CloseDesktop(reinterpret_cast<HDESK>(handle()));
}

std::string desktop::name()
{
	return _name;
}