#ifndef PROCESS_HPP
#define PROCESS_HPP

#include <cstdint>
#include <Windows.h>
#include "safe_handle.hpp"

namespace winc {

class process : protected safe_handle {
public:
	explicit process(HANDLE handle);
	void terminate(int32_t exit_code);
	uint32_t id();
	uint32_t cpu_time_ms();
	uint32_t peak_memory_usage_kb();
	uint32_t virtual_protect(void *address, size_t length, uint32_t new_protect);
	void read_memory(const void *address, void *buffer, size_t length);
	void write_memory(void *address, const void *buffer, size_t length);
};

}

#endif