#include <memory>
#include <set>
#include <mutex>
#include <atomic>
#include <process.h>
#include <Windows.h>
#include "timer_queue.hpp"

using namespace std;
using namespace winc;

namespace {

ULARGE_INTEGER g_prev_tick = {0};
mutex g_prev_tick_mutex;
atomic_uint64_t g_sequence_id = 0;

uint64_t get_tick_count_64()
{
	DWORD tick_lo = GetTickCount();

	lock_guard<mutex> lock(g_prev_tick_mutex);
	if (tick_lo < g_prev_tick.LowPart)
		++g_prev_tick.HighPart;
	g_prev_tick.LowPart = tick_lo;
	return (uint64_t)g_prev_tick.QuadPart;
}

}

timer_queue::extra::extra(
	uint32_t delay_ms)
	: _sequence_id(++g_sequence_id)
	, _expire_at(get_tick_count_64() + delay_ms)
{}

timer_queue::timer_queue()
	: _event(CreateEventW(NULL, FALSE, FALSE, NULL),
		[](HANDLE event_handle) { CloseHandle(event_handle); })
	, _exiting(false)
	, _thread(&timer_queue::_thread_entry, this)
{}

timer_queue::~timer_queue()
{
	_exiting = true;
	SetEvent(_event.get());
	_thread.join();
}

void timer_queue::cancel(const context_ptr &timer)
{
	lock_guard<mutex> lock(_mutex);
	_context_set.erase(timer);
}

void timer_queue::_thread_entry()
{
	DWORD wait_time = INFINITE;

	for (;;) {
		DWORD ret = WaitForSingleObject(_event.get(), wait_time);
		if (ret == WAIT_FAILED)
			abort();

		for (;;) {
			context_ptr context;
			{
				lock_guard<mutex> lock(_mutex);
				if (_context_set.empty()) {
					if (_exiting)
						return;
					wait_time = INFINITE;
					break;
				}
				auto iter = _context_set.begin();
				uint64_t expire_at = (*iter)->expire_at();
				uint64_t now = get_tick_count_64();
				if (expire_at > now) {
					wait_time = (DWORD)(expire_at - now);
					break;
				}
				context = *iter;
				_context_set.erase(iter);
			}
			context->invoke();
		}
	}
}