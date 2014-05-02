#ifndef WINDOWS_ERROR_HPP
#define WINDOWS_ERROR_HPP

#include <stdexcept>
#include <cstdint>

class windows_error : public std::runtime_error {
public:
	windows_error(uint32_t errval);
};

class winnt_error : public std::runtime_error {
public:
	winnt_error(uint32_t errval);
};

#endif