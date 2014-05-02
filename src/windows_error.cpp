#include "windows_error.hpp"
#include <memory>
#include <string>
#include <Windows.h>

using namespace std;

namespace {

HMODULE ntdll_base = NULL;

string format_message(DWORD error_code, bool allow_nt_status = false, bool reenter = false)
{
	if (allow_nt_status && !ntdll_base) {
		ntdll_base = GetModuleHandleA("ntdll.dll");
	}

	char *message = NULL;
	DWORD ret;
	
	if (allow_nt_status) {
		ret = FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_FROM_HMODULE |
			FORMAT_MESSAGE_IGNORE_INSERTS |
			FORMAT_MESSAGE_MAX_WIDTH_MASK,
			ntdll_base,
			error_code,
			LANG_USER_DEFAULT,
			(LPSTR)&message,
			0,
			NULL);
	} else {
		ret = FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS |
			FORMAT_MESSAGE_MAX_WIDTH_MASK,
			NULL,
			error_code,
			LANG_USER_DEFAULT,
			(LPSTR)&message,
			0,
			NULL);
	}

	if (!ret) {
		if (reenter)
			return "unknown_error";
		else
			return "format_message " + format_message(GetLastError(), allow_nt_status, true);
	}
	
	auto buffer_holder = shared_ptr<void>(message,
		[](void *buffer) {
			LocalFree(buffer);
		});

	return string(message, message + ret);
}

}

windows_error::windows_error(uint32_t errval)
	: runtime_error(format_message(errval))
{}

winnt_error::winnt_error(uint32_t errval)
	: runtime_error(format_message(errval, true))
{}