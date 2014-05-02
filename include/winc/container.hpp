#ifndef WINC_CONTAINER_HPP
#define WINC_CONTAINER_HPP

#include <memory>

namespace winc {

class container {
public:
	container();

	struct impl;
private:
	std::shared_ptr<impl> _impl;
};

}

#endif