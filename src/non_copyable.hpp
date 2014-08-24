#ifndef NON_COPYABLE_HPP
#define NON_COPYABLE_HPP

namespace winc {

class non_copyable {
public:
	non_copyable() {}
private:
	non_copyable(const non_copyable &);
	non_copyable &operator =(const non_copyable &);
};

}

#endif