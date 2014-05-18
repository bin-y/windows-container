#ifndef TIMER_QUEUE_HPP
#define TIMER_QUEUE_HPP

#include <vector>
#include <memory>
#include <algorithm>
#include <functional>
#include <utility>
#include <stdexcept>
#include <cstdint>
#include <stlsoft/synch/lock_scope.hpp>
#include <winstl/synch/thread_mutex.hpp>
#include <winstl/synch/event.hpp>
#include <Windows.h>
#include "action.hpp"

namespace winc {

class timer_queue {
public:
	timer_queue();
	~timer_queue();

	template <typename Callback>
	void queue(const Callback &callback, uint32_t delay_ms);

private:
	class extra;
	typedef action<extra> context;
	typedef std::unique_ptr<context> context_ptr;
	struct context_pred;

private:
	HANDLE _create_thread();
	static unsigned __stdcall _thread_entry(void *param);

private:
	std::vector<context_ptr> _context_heap;
	volatile bool _exiting;
	winstl::thread_mutex _context_heap_lock;
	winstl::event _notify_event;
	HANDLE _worker_thread;
};

class timer_queue::extra {
public:
	extra(uint32_t delay_ms);
	uint64_t expire_at();

private:
	uint64_t _expire_at;
};

struct timer_queue::context_pred : public std::binary_function<
	timer_queue::context_ptr, timer_queue::context_ptr, bool> {
	bool operator()(const context_ptr &a, const context_ptr &b)
	{
		return a->expire_at() > b->expire_at();
	}
};

template <typename Callback>
void timer_queue::queue(const Callback &callback, uint32_t delay_ms)
{
	context_ptr ctx(new action_impl<extra, Callback>(delay_ms, callback));
	{
		stlsoft::lock_scope<winstl::thread_mutex> lock(_context_heap_lock);
		_context_heap.push_back(move(ctx));
		std::push_heap(_context_heap.begin(), _context_heap.end(), context_pred());
	}
	_notify_event.set();
}

}

#endif