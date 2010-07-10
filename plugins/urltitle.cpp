#include <typeinfo>

#include "urltitle.hpp"

namespace zerobot {

PlugInURLTitle::PlugInURLTitle(int _priority) : PlugIn(_priority, "urltitle") {
}

std::auto_ptr< PlugInResult > PlugInURLTitle::onConnect(state_t _state) {
	return std::auto_ptr< PlugInResult >(NULL);
}

std::auto_ptr< PlugInResult > PlugInURLTitle::onDisconnect(state_t _state) {
	return std::auto_ptr< PlugInResult >(NULL);
}

std::auto_ptr< PlugInResult > PlugInURLTitle::onPacket(state_t _state, IRC::Message const &_message) {
	std::auto_ptr< PlugInResult > result(NULL);
	try {
		IRC::MessagePrivMsg const &privMessage __attribute__((unused)) = dynamic_cast< IRC::MessagePrivMsg const &>(_message);
	}
	catch(std::bad_cast) {}
	return result;
}

std::auto_ptr< PlugInResult > PlugInURLTitle::onTimeTrigger(state_t _state) {
	return std::auto_ptr< PlugInResult >(NULL);
}

}