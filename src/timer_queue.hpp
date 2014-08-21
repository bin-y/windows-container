#ifndef TIMER_QUEUE_HPP
#define TIMER_QUEUE_HPP

#include <set>
#include <memory>
#include <functional>
#include <cstdint>
#include <mutex>
#include <thread>
#include <Windows.h>
#include "action.hpp"

namespace winc {

class timer_queue {
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
	void _thread_entry();

private:
	timer_queue(const timer_queue &);
	timer_queue &operator=(const timer_queue &);

private:
	class extra {
	public:
		extra(uint32_t delay_ms);
		uint64_t sequence_id() { return _sequence_id; }
		uint64_t expire_at() { return _expire_at; }

	private:
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
	std::mutex _mutex;
	std::set<context_ptr, context_pred> _context_set;
	std::shared_ptr<void> _event;
	volatile bool _exiting;
	std::thread _thread;
};

template <typename Callback>
timer_queue::context_ptr timer_queue::queue(
	const Callback &callback, uint32_t delay_ms)
{
	context_ptr ctx(new action_impl<extra, Callback>(
		extra(delay_ms), callback));
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_context_set.insert(ctx);
	}
	SetEvent(_event.get());
	return ctx;
}

}

#endif