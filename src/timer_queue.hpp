#ifndef TIMER_QUEUE_HPP
#define TIMER_QUEUE_HPP

#include <set>
#include <memory>
#include <functional>
#include <cstdint>
#include <stlsoft/synch/lock_scope.hpp>
#include <winstl/synch/thread_mutex.hpp>
#include <winstl/synch/event.hpp>
#include <Windows.h>
#include "action.hpp"

namespace winc {

class timer_queue : public std::enable_shared_from_this<timer_queue> {
private:
	class extra;
	typedef action<extra> context;

public:
	typedef std::shared_ptr<context> context_ptr;

public:
	timer_queue();
	~timer_queue();

	template <typename Callback>
	context_ptr queue(const Callback &callback, uint32_t delay_ms);

	void cancel(const context_ptr &timer);

private:
	void _guard_worker_thread();
	static unsigned __stdcall _thread_entry(void *param);

private:
	class extra {
	public:
		extra(const std::shared_ptr<timer_queue> &timer_queue,
			uint32_t delay_ms);
		uint64_t sequence_id() { return _sequence_id; }
		uint64_t expire_at() { return _expire_at; }

	private:
		std::shared_ptr<timer_queue> _timer_queue;
		uint64_t _sequence_id;
		uint64_t _expire_at;
	};

	struct context_pred : public std::binary_function<
		timer_queue::context_ptr, timer_queue::context_ptr, bool> {
		bool operator()(const context_ptr &a, const context_ptr &b)
		{
			return (a->expire_at() < b->expire_at()) ||
				((a->expire_at() == b->expire_at()) &&
				(a->sequence_id() < b->sequence_id()));
		}
	};

private:
	std::set<context_ptr, context_pred> _context_set;
	winstl::thread_mutex _context_set_lock;
	std::shared_ptr<winstl::event> _notify_event;
	winstl::thread_mutex _worker_thread_lock;
	HANDLE _worker_thread;
};

template <typename Callback>
timer_queue::context_ptr timer_queue::queue(
	const Callback &callback, uint32_t delay_ms)
{
	context_ptr ctx(new action_impl<extra, Callback>(
		extra(shared_from_this(), delay_ms), callback));
	{
		stlsoft::lock_scope<winstl::thread_mutex> lock(_context_set_lock);
		_context_set.insert(ctx);
	}
	_notify_event->set();
	_guard_worker_thread();
	return ctx;
}

}

#endif