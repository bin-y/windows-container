#include "safe_handle.hpp"

using namespace winc;

safe_handle::safe_handle()
	: _handle(NULL)
{}

void safe_handle::_close()
{
	if (_handle != NULL)
		CloseHandle(_handle);
}

safe_handle::~safe_handle()
{
	_close();
}

void safe_handle::set_handle(HANDLE handle)
{
	_close();
	_handle = handle;
}

HANDLE safe_handle::handle()
{
	return _handle;
}