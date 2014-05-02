#ifndef CONTAINER_IMPL_HPP
#define CONTAINER_IMPL_HPP

#include <winc/container.hpp>
#include <cstdint>

struct winc::container::impl {
	uint32_t processor_count;

	impl();
};

#endif