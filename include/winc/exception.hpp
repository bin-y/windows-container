#ifndef _EXCEPTION_HPP
#define _EXCEPTION_HPP

#include <stdexcept>
#include <cstdint>

namespace winc {

class win32_exception : public std::exception {
public:
	explicit win32_exception(uint32_t error_code);
	const char *what() const;
	uint32_t error_code() const;

private:
	uint32_t error_code_;
};

}

#endif