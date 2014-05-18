#ifndef ACTION_HPP
#define ACTION_HPP

namespace winc {

template <typename Extra>
class action : public Extra {
public:
	action(const Extra &extra)
		: Extra(extra)
		{}
	virtual ~action() {}
	virtual void invoke() = 0;
};

template <typename Extra, typename Callback>
class action_impl : public action<Extra> {
public:
	action_impl(const Extra & extra,
		const Callback &callback)
		: action(extra)
		{ _callback = callback; }

	/* override */ void invoke()
		{ _callback(); }

private:
	Callback _callback;
};

}

#endif