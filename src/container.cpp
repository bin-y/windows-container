#include <winc/container.hpp>
#include <Windows.h>
#include "container_impl.hpp"

using namespace winc;

container::container()
	: _impl(new impl)
{}

container::impl::impl()
{
}