#ifndef PROCESS_HPP
#define PROCESS_HPP

#include <cstdint>
#include <Windows.h>
#include "non_copyable.hpp"

namespace winc {

class process : non_copyable {
public:
	explicit process(HANDLE handle);
	~process();
	void terminate(int32_t exit_code);
	uint32_t id();
	void start_timer();
	uint32_t alive_time_ms();
	uint32_t peak_memory_usage_kb();
	uint32_t virtual_protect(void *address, size_t length, uint32_t new_protect);
	void read_memory(const void *address, void *buffer, size_t length);
	void write_memory(void *address, const void *buffer, size_t length);

private:
	uint64_t _process_time();

private:
	HANDLE _handle;
	bool _timer_started;
	uint64_t _initial_process_time;
	uint64_t _initial_idle_time;
};

}

#endif