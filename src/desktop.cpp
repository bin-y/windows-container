#include <string>
#include <Windows.h>
#include "desktop.hpp"
#include "windows_error.hpp"

using namespace std;
using namespace winc;

namespace {

HANDLE create_desktop(const string &name)
{
	HANDLE result = CreateDesktopA(name.c_str(), NULL, NULL, 0,
		DESKTOP_READOBJECTS | DESKTOP_CREATEWINDOW | DESKTOP_WRITEOBJECTS |
		READ_CONTROL | WRITE_DAC | DESKTOP_SWITCHDESKTOP, 0);
	if (!result) {
		throw windows_error(GetLastError());
	}
	return result;
}

}

desktop::desktop(const string &name)
	: user_object(create_desktop(name)), _name(name)
{}

desktop::~desktop()
{
	::CloseDesktop(reinterpret_cast<HDESK>(_handle));
}

std::string desktop::name()
{
	return _name;
}