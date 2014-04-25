#ifndef LOGIN_IMPL_HPP
#define LOGIN_IMPL_HPP

#include <winc/login.hpp>
#include <memory>
#include <string>
#include "user_token.hpp"
#include "desktop.hpp"
#include "window_station.hpp"

struct winc::login::impl {
	std::string username;
	user_token user_token;
	desktop desktop;
	window_station window_station;

	impl(const std::string &username, const std::string &password);
};

#endif