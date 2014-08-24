#ifndef _USER_OBJECT_HPP
#define _USER_OBJECT_HPP

#include <string>
#include <vector>
#include <Windows.h>
#include "non_copyable.hpp"

namespace winc {

class user_object : non_copyable {
public:
	struct allowed_ace {
		allowed_ace(BYTE flags, ACCESS_MASK mask)
			: flags(flags)
			, mask(mask)
		{}

		BYTE flags;
		ACCESS_MASK mask;
	};

public:
	explicit user_object(HANDLE obj) : _handle(obj) {}
	virtual ~user_object() {}
	HANDLE handle() { return _handle; }
	virtual std::string name();
	void add_allowed_ace(const std::vector<char> &sid, const allowed_ace &ace);
	void remove_ace_by_sid(const std::vector<char> &sid);

protected:
	HANDLE _handle;
};

}

#endif