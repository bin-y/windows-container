#include <stdexcept>
#include <algorithm>
#include <stlsoft/synch/lock_scope.hpp>
#include <winstl/synch/spin_mutex.hpp>
#include <winstl/synch/thread_mutex.hpp>
#include <winstl/shims/conversion/to_uint64.hpp>
#include <process.h>
#include <Windows.h>
#include "timer_queue.hpp"

using namespace std;
using stlsoft::lock_scope;
using winstl::spin_mutex;
using winstl::thread_mutex;
using stlsoft::to_uint64;
using namespace winc;

namespace {

ULARGE_INTEGER g_prev_tick = {0};
spin_mutex g_prev_tick_lock;

uint64_t get_tick_count_64()
{
	DWORD tick_lo = GetTickCount();

	lock_scope<spin_mutex> lock(g_prev_tick_lock);
	if (tick_lo < g_prev_tick.LowPart) {
		++g_prev_tick.HighPart;
	}
	g_prev_tick.LowPart = tick_lo;
	return to_uint64(g_prev_tick);
}

}

timer_queue::extra::extra(uint32_t delay_ms)
{
	_expire_at = get_tick_count_64() + delay_ms;
}

uint64_t timer_queue::extra::expire_at()
{
	return _expire_at;
}

timer_queue::timer_queue()
	: _exiting(false)
	, _notify_event(false, false)
	, _worker_thread(_create_thread())
{}

timer_queue::~timer_queue()
{
	_exiting = true;
	_notify_event.set();
	WaitForSingleObject(_worker_thread, INFINITE);
	CloseHandle(_worker_thread);
}

HANDLE timer_queue::_create_thread()
{
	uintptr_t ret = _beginthreadex(nullptr, 0, _thread_entry, this, 0, nullptr);
	if (ret == 0) {
		throw runtime_error(strerror(errno));
	}
	return (HANDLE)ret;
}

unsigned __stdcall timer_queue::_thread_entry(void *param)
{
	timer_queue *u = (timer_queue *)param;
	DWORD wait_time = INFINITE;

	for (;;) {
		DWORD ret = WaitForSingleObject(u->_notify_event.handle(), wait_time);
		if (ret == WAIT_FAILED) {
			abort();
		}

		for (;;) {
			context_ptr context;
			{
				lock_scope<thread_mutex> lock(u->_context_heap_lock);
				if (u->_context_heap.empty()) {
					if (u->_exiting) {
						return 0;
					}
					wait_time = INFINITE;
					break;
				}
				uint64_t expire_at = u->_context_heap.front()->expire_at();
				uint64_t now = get_tick_count_64();
				if (expire_at > now) {
					wait_time = (DWORD)(expire_at - now);
					break;
				}
				pop_heap(u->_context_heap.begin(), u->_context_heap.end(), context_pred());
				context = move(u->_context_heap.back());
				u->_context_heap.pop_back();
			}
			context->invoke();
		}
	}
}