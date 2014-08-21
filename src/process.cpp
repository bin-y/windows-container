#define _USRDLL

#include <cstdint>
#include <vector>
#include <algorithm>
#include <windows.h>
extern "C" {
	#include <extypes.h>
	#include <exfuncs.h>
}
#include <psapi.h>
#include "process.hpp"
#include "windows_error.hpp"

using namespace std;
using namespace winc;

namespace {

uint32_t g_processor_count = 0;

uint32_t get_processor_count()
{
	if (g_processor_count != 0)
		return g_processor_count;

	SYSTEM_INFO system_info;
	GetSystemInfo(&system_info);
	uint32_t result = static_cast<uint32_t>(system_info.dwNumberOfProcessors);
	g_processor_count = result;
	return result;
}

uint64_t get_idle_time()
{
	const size_t buffer_length = 328;
	vector<char> buffer(buffer_length);
	PSYSTEM_PERFORMANCE_INFORMATION spi = reinterpret_cast<PSYSTEM_PERFORMANCE_INFORMATION>(&*buffer.begin());

	NTSTATUS status = NtQuerySystemInformation(SystemPerformanceInformation, spi, buffer_length, NULL);
	if (!NT_SUCCESS(status)) {
		throw winnt_error(status);
	}

	return (uint64_t)spi->IdleProcessTime.QuadPart;
}

}

process::process(HANDLE handle)
	: _handle(handle)
	, _timer_started(false)
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

void process::start_timer()
{
	_initial_process_time = _process_time();
	_initial_idle_time = get_idle_time();
	_timer_started = true;
}

uint64_t process::_process_time()
{
	ULARGE_INTEGER creation_time, exit_time, kernel_time, user_time;
	BOOL result = GetProcessTimes(_handle,
		(LPFILETIME)&creation_time, (LPFILETIME)&exit_time,
		(LPFILETIME)&kernel_time, (LPFILETIME)&user_time);
	if (!result) {
		throw windows_error(GetLastError());
	}
	
	return (uint64_t)(kernel_time.QuadPart + user_time.QuadPart);
}

uint32_t process::alive_time_ms()
{
	if (!_timer_started) {
		return 0;
	} else {
		return static_cast<uint32_t>(
			max(_process_time() - _initial_process_time,
				(get_idle_time() - _initial_idle_time) / get_processor_count()
				) / 10000);
	}
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