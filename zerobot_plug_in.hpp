#ifndef ZEROBOT_PLUG_IN_HPP
#define ZEROBOT_PLUG_IN_HPP

#include <string>

namespace zerobot {

class ZeroBotPlugIn {
	public:
		ZeroBotPlugIn(unsigned int _priority, std::string const &_name);

	protected:
		const unsigned int priority;
		const std::string name;
};

}

#endif

