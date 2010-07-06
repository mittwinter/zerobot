#include "log.hpp"

namespace zerobot {

std::auto_ptr< PlugInResult > PlugInLog::onConnect(state_t _state) {
	return std::auto_ptr< PlugInResult >(NULL);
}

std::auto_ptr< PlugInResult > PlugInLog::onPacket(state_t _state, IRC::Message const &_message) {
	return std::auto_ptr< PlugInResult >(NULL);
}

std::auto_ptr< PlugInResult > PlugInLog::onTimeTrigger(state_t _state) {
	return std::auto_ptr< PlugInResult >(NULL);
}

std::auto_ptr< PlugInResult > PlugInLog::onDisconnect(state_t _state){
	return std::auto_ptr< PlugInResult >(NULL);
}

}
