#include <winc/container.hpp>
#include <Windows.h>
#include "container_impl.hpp"

using namespace winc;

namespace {

uint32_t get_processor_count()
{
	SYSTEM_INFO system_info;
	GetSystemInfo(&system_info);
	return static_cast<uint32_t>(system_info.dwNumberOfProcessors);
}

}

container::container()
	: _impl(new impl)
{}

container::impl::impl()
{
	processor_count = get_processor_count();
}