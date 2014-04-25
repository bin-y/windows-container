#include <winc/login.hpp>
#include <system_error>
#include <memory>
#include <cstdint>
#include <string>
#include <Windows.h>
#include <aclapi.h>
#include "login_impl.hpp"

using namespace std;
using namespace winc;

namespace {

const size_t desktop_name_length = 16;

string random_string(size_t size,
	const string &charset = "abcdefghijklmnopqrstuvwxyz0123456789")
{
	HCRYPTPROV context;
	if (!CryptAcquireContextA(&context, NULL, MS_DEF_PROV_A, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
		throw system_error(GetLastError(), system_category());
	}
	
	auto context_scope = shared_ptr<void>(nullptr,
		[context](void *)
		{ CryptReleaseContext(context, 0); });

	string result;
	while (size--) {
		size_t data;
		if (!::CryptGenRandom(context, sizeof(data), reinterpret_cast<BYTE *>(&data))) {
			throw system_error(GetLastError(), system_category());
		}
		result.push_back(charset[data % charset.size()]);
	}

	return result;
}

}

login::login(const string &username, const string &password)
	: _impl(new impl(username, password))
{}

login::impl::impl(const string &username, const string &password)
	: username(username)
	, user_token(username, password)
	, desktop(random_string(desktop_name_length))
	, window_station()
{
}

void login::grant_access(const string &path)
{
	HANDLE file = CreateFileA(path.c_str(), READ_CONTROL | WRITE_DAC,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS, NULL);
	if (file == INVALID_HANDLE_VALUE) {
		throw system_error(GetLastError(), system_category());
	}

	auto file_scope = shared_ptr<void>(nullptr,
		[file](void *)
		{ CloseHandle(file); });

	PACL old_dacl, new_dacl;
	PSECURITY_DESCRIPTOR sd;
	EXPLICIT_ACCESSA ea = {0};

	DWORD ret;
	ret = GetSecurityInfo(file, SE_FILE_OBJECT, DACL_SECURITY_INFORMATION,
		NULL, NULL, &old_dacl, NULL, &sd);
	if (ret != ERROR_SUCCESS) {
		throw system_error(ret, system_category());
	}

	auto sd_scope = shared_ptr<void>(nullptr,
		[sd](void *)
		{ LocalFree(sd); });

	ea.grfAccessPermissions = FILE_GENERIC_READ | FILE_GENERIC_WRITE | FILE_GENERIC_EXECUTE | DELETE;
	ea.grfAccessMode = GRANT_ACCESS;
	ea.grfInheritance = OBJECT_INHERIT_ACE | CONTAINER_INHERIT_ACE;
	ea.Trustee.TrusteeForm = TRUSTEE_IS_NAME;
	ea.Trustee.TrusteeType = TRUSTEE_IS_USER;
	ea.Trustee.ptstrName = const_cast<LPSTR>(_impl->username.c_str());
	ret = SetEntriesInAclA(1, &ea, old_dacl, &new_dacl);
	if (ret != ERROR_SUCCESS) {
		throw system_error(ret, system_category());
	}

	auto new_dacl_scope = shared_ptr<void>(nullptr,
		[new_dacl](void *)
		{ LocalFree(new_dacl); });

	ret = SetSecurityInfo(file, SE_FILE_OBJECT, DACL_SECURITY_INFORMATION,
		NULL, NULL, new_dacl, NULL);
	if (ret != ERROR_SUCCESS) {
		LocalFree(new_dacl);
		throw system_error(ret, system_category());
	}
}