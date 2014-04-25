#include <system_error>
#include <string>
#include <vector>
#include <Windows.h>
#include "logon_session.hpp"

using namespace std;
using namespace winc;

logon_session::logon_session(const std::string &username, const std::string &password)
	: _handle(_logon_user(username, password))
{}

HANDLE logon_session::_logon_user(const string &username, const string &password)
{
	HANDLE result;
	BOOL success = LogonUserA(username.c_str(), ".", password.c_str(),
		LOGON32_LOGON_INTERACTIVE, LOGON32_PROVIDER_DEFAULT, &result);
	if (!success) {
		throw system_error(::GetLastError(), system_category());
	}
	return result;
}

logon_session::~logon_session()
{
	::CloseHandle(handle());
}

HANDLE logon_session::handle()
{
	return _handle;
}

vector<char> logon_session::sid()
{
	vector<char> buffer;
	PTOKEN_GROUPS ptg;
	DWORD length;
	DWORD index;

	if (!::GetTokenInformation(handle(), TokenGroups, NULL, NULL, &length)) {
		DWORD error_code = ::GetLastError();
		if (error_code != ERROR_INSUFFICIENT_BUFFER) {
			throw system_error(error_code, system_category());
		}
	} else {
		length = 0;
	}

	if (!length) {
		throw runtime_error("empty_token");
	}

	buffer.resize(length);
	ptg = reinterpret_cast<PTOKEN_GROUPS>(buffer.data());
	if (!::GetTokenInformation(handle(), TokenGroups, ptg, length, &length)) {
		throw system_error(::GetLastError(), system_category());
	}

	for (index = 0; index < ptg->GroupCount; ++index) {
		if ((ptg->Groups[index].Attributes & SE_GROUP_LOGON_ID) == SE_GROUP_LOGON_ID) {
			length = ::GetLengthSid(ptg->Groups[index].Sid);
			if (!length) {
				throw runtime_error("empty_sid");
			}
			vector<char> sid(length);
			if (!::CopySid(length, sid.data(), ptg->Groups[index].Sid)) {
				throw system_error(::GetLastError(), system_category());
			}
			return sid;
		}
	}

	// token group not found
	throw runtime_error("token_group_not_found");
}