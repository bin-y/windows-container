#include <memory>
#include <set>
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
volatile uint64_t g_sequence_id = 0;

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

timer_queue::extra::extra(
	const shared_ptr<timer_queue> &timer_queue,
	uint32_t delay_ms)
	: _timer_queue(timer_queue)
	, _sequence_id(InterlockedIncrement(&g_sequence_id))
	, _expire_at(get_tick_count_64() + delay_ms)
{}

timer_queue::timer_queue()
	: _notify_event(make_shared<winstl::event>(false, false))
	, _worker_thread(NULL)
{}

timer_queue::~timer_queue()
{
	_notify_event->set();
	if (_worker_thread != NULL) {
		CloseHandle(_worker_thread);
	}
}

void timer_queue::cancel(const context_ptr &timer)
{
	lock_scope<thread_mutex> lock(_context_set_lock);
	_context_set.erase(timer);
}

void timer_queue::_guard_worker_thread()
{
	if (!_worker_thread) {
		lock_scope<thread_mutex> lock(_worker_thread_lock);
		if (!_worker_thread) {
			uintptr_t ret = _beginthreadex(nullptr, 0, _thread_entry, this, 0, nullptr);
			if (ret == 0) {
				throw runtime_error(strerror(errno));
			}
			_worker_thread = (HANDLE)ret;
		}
	}
}

unsigned __stdcall timer_queue::_thread_entry(void *param)
{
	weak_ptr<timer_queue> w(((timer_queue *)param)->shared_from_this());
	DWORD wait_time = INFINITE;

	try {
		for (;;) {
			auto e = shared_ptr<timer_queue>(w)->_notify_event;
			DWORD ret = WaitForSingleObject(e->handle(), wait_time);
			if (ret == WAIT_FAILED) {
				abort();
			}

			shared_ptr<timer_queue> u(w);
			for (;;) {
				context_ptr context;
				{
					lock_scope<thread_mutex> lock(u->_context_set_lock);
					if (u->_context_set.empty()) {
						wait_time = INFINITE;
						break;
					}
					auto iter = u->_context_set.begin();
					uint64_t expire_at = (*iter)->expire_at();
					uint64_t now = get_tick_count_64();
					if (expire_at > now) {
						wait_time = (DWORD)(expire_at - now);
						break;
					}
					context = *iter;
					u->_context_set.erase(iter);
				}
				context->invoke();
			}
		}
	} catch (const std::bad_weak_ptr &) {
	}

	return 0;
}