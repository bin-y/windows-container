#include <string>
#include <vector>
#include <Windows.h>
#include "user_token.hpp"
#include "windows_error.hpp"

using namespace std;
using namespace winc;

namespace {

HANDLE logon_user(const string &username, const string &password)
{
	HANDLE result;
	BOOL success = LogonUserA(username.c_str(), ".", password.c_str(),
		LOGON32_LOGON_INTERACTIVE, LOGON32_PROVIDER_DEFAULT, &result);
	if (!success) {
		throw windows_error(GetLastError());
	}
	return result;
}

}

user_token::user_token(const std::string &username, const std::string &password)
{
	_handle.set(logon_user(username, password));
}

vector<char> user_token::sid() const
{
	vector<char> buffer;
	PTOKEN_GROUPS ptg;
	DWORD length;
	DWORD index;

	if (!GetTokenInformation(_handle.get(), TokenGroups, NULL, NULL, &length)) {
		DWORD error_code = GetLastError();
		if (error_code != ERROR_INSUFFICIENT_BUFFER) {
			throw windows_error(error_code);
		}
	} else {
		length = 0;
	}

	if (!length) {
		throw runtime_error("empty_token");
	}

	buffer.resize(length);
	ptg = reinterpret_cast<PTOKEN_GROUPS>(buffer.data());
	if (!GetTokenInformation(_handle.get(), TokenGroups, ptg, length, &length)) {
		throw windows_error(GetLastError());
	}

	for (index = 0; index < ptg->GroupCount; ++index) {
		if ((ptg->Groups[index].Attributes & SE_GROUP_LOGON_ID) == SE_GROUP_LOGON_ID) {
			length = GetLengthSid(ptg->Groups[index].Sid);
			if (!length) {
				throw runtime_error("empty_sid");
			}
			vector<char> sid(length);
			if (!CopySid(length, sid.data(), ptg->Groups[index].Sid)) {
				throw windows_error(GetLastError());
			}
			return sid;
		}
	}

	// token group not found
	throw runtime_error("token_group_not_found");
}