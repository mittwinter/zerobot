#include "pingpong.hpp"

namespace zerobot {

PlugInPingPong::PlugInPingPong(int _priority) : PlugIn(_priority, "pingpong") {
}

std::auto_ptr< PlugInResult > PlugInPingPong::onConnect(IRC::Message const &_message) {
	return onPacket(_message);
}

std::auto_ptr< PlugInResult > PlugInPingPong::onPacket(IRC::Message const &_message) {
	return std::auto_ptr< PlugInResult > (NULL);
}

std::auto_ptr< PlugInResult > PlugInPingPong::onDisconnect(IRC::Message const &_message) {
	return onPacket(_message);
}

}

