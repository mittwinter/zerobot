#include "base.hpp"

namespace zerobot {

PlugInResult::PlugInResult() {
	newState = STATE_NOP;
}

PlugIn::PlugIn(unsigned int _priority, std::string const &_name) : priority(_priority), name(_name) {
}

}
