#include <typeinfo>

#include "pingpong.hpp"

namespace zerobot {

PlugInPingPong::PlugInPingPong(int _priority) : PlugIn(_priority, "pingpong") {
}

std::auto_ptr< PlugInResult > PlugInPingPong::onConnect(state_t _state) {
	return std::auto_ptr< PlugInResult > (NULL);
}

std::auto_ptr< PlugInResult > PlugInPingPong::onPacket(state_t _state, IRC::Message const &_message) {
	try {
		IRC::MessagePing const &ping = dynamic_cast< IRC::MessagePing const & >(_message);
		std::auto_ptr< PlugInResult > result(new PlugInResult);
		result->messages.push_back(new IRC::MessagePong(ping.getServerName()));
	}
	catch(std::bad_cast e) {
	}
	return std::auto_ptr< PlugInResult > (NULL);
}

std::auto_ptr< PlugInResult > PlugInPingPong::onDisconnect(state_t _state) {
	return std::auto_ptr< PlugInResult > (NULL);
}

}

