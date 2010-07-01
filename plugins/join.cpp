#include "join.hpp"

namespace zerobot {

PlugInJoin::PlugInJoin(int _priority, std::string const &_channelName) : PlugIn(_priority, "join") {
	channelName = _channelName;
}

std::auto_ptr< PlugInResult > PlugInJoin::onConnect(state_t _state) {
	return std::auto_ptr< PlugInResult >(NULL);
}

std::auto_ptr< PlugInResult > PlugInJoin::onPacket(state_t _state, IRC::Message const &_message) {
	return std::auto_ptr< PlugInResult >(NULL);
}

std::auto_ptr< PlugInResult > PlugInJoin::onTimeTrigger(state_t _state) {
	return std::auto_ptr< PlugInResult >(NULL);
}

std::auto_ptr< PlugInResult > PlugInJoin::onDisconnect(state_t _state) {
	return std::auto_ptr< PlugInResult >(NULL);
}

}

