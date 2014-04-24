#include <winc/exception.hpp>
#include <windows.h>

using namespace winc;

win32_exception::win32_exception(uint32_t error_code)
	: error_code_(error_code)
{}

const char *win32_exception::what() const
{
	// TODO: FormatMessage()
	return "win32_exception";
}

uint32_t win32_exception::error_code() const
{
	return error_code_;
}