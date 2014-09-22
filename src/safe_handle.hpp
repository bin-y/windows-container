#ifndef SAFE_HANDLE_HPP
#define SAFE_HANDLE_HPP

#include <Windows.h>
#include "non_copyable.hpp"

namespace winc {
	 
class safe_handle : non_copyable {
public:
	safe_handle();
	~safe_handle();
	void set_handle(HANDLE handle);
	HANDLE handle();

private:
	void _close();

private:
	HANDLE _handle;
};

}

#endif