#ifndef PROCESS_HPP
#define PROCESS_HPP

#include <cstdint>
#include <Windows.h>
#include "non_copyable.hpp"
#include "safe_handle.hpp"

namespace winc {

class process : non_copyable {
public:
	explicit process(HANDLE handle);
	HANDLE handle() const;
	void terminate(int32_t exit_code);
	uint32_t id() const;
	uint32_t cpu_time_ms() const;
	uint32_t peak_memory_usage_kb() const;
	uint32_t virtual_protect(void *address, size_t length, uint32_t new_protect);
	void read_memory(const void *address, void *buffer, size_t length);
	void write_memory(void *address, const void *buffer, size_t length);
private:
	safe_handle _handle;
};

}

#endif