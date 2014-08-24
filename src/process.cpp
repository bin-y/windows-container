#define _USRDLL

#include <cstdint>
#include <vector>
#include <algorithm>
#include <Windows.h>
extern "C" {
	#include <extypes.h>
	#include <exfuncs.h>
}
#include <psapi.h>
#include "process.hpp"
#include "windows_error.hpp"

using namespace std;
using namespace winc;

process::process(HANDLE handle)
	: _handle(handle)
{}

process::~process()
{
	::CloseHandle(_handle);
}

void process::terminate(int32_t exit_code)
{
	BOOL result = TerminateProcess(_handle, static_cast<UINT>(exit_code));
	if (!result) {
		throw windows_error(GetLastError());
	}
}

uint32_t process::id()
{
	DWORD result = GetProcessId(_handle);
	return static_cast<uint32_t>(result);
}

uint32_t process::cpu_time_ms()
{
	ULARGE_INTEGER creation_time, exit_time, kernel_time, user_time;
	BOOL result = GetProcessTimes(_handle,
		(LPFILETIME)&creation_time, (LPFILETIME)&exit_time,
		(LPFILETIME)&kernel_time, (LPFILETIME)&user_time);
	if (!result) {
		throw windows_error(GetLastError());
	}
	
	return (uint32_t)((kernel_time.QuadPart + user_time.QuadPart) / 10000);
}

uint32_t process::peak_memory_usage_kb()
{
	PROCESS_MEMORY_COUNTERS pmc;
	pmc.cb = sizeof(pmc);
	BOOL result = GetProcessMemoryInfo(_handle, &pmc, sizeof(pmc));
	if (!result) {
		throw windows_error(GetLastError());
	}

	return static_cast<uint32_t>(pmc.PeakPagefileUsage / 1024);
}

uint32_t process::virtual_protect(void *address, size_t length, uint32_t new_protect)
{
	DWORD old_protect;
	BOOL result = VirtualProtectEx(_handle, address, length, new_protect, &old_protect);
	if (!result) {
		throw windows_error(GetLastError());
	}
	return old_protect;
}

void process::read_memory(const void *address, void *buffer, size_t length)
{
	BOOL result = ReadProcessMemory(_handle, address, buffer, length, NULL);
	if (!result) {
		throw windows_error(GetLastError());
	}
}

void process::write_memory(void *address, const void *buffer, size_t length)
{
	BOOL result = WriteProcessMemory(_handle, address, buffer, length, NULL);
	if (!result) {
		throw windows_error(GetLastError());
	}
}