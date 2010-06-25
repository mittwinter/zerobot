#include "pingpong.hpp"

namespace zerobot {

PlugInPingPong::PlugInPingPong(int _priority) : PlugIn(_priority, "pingpong") {
}

PlugInResult PlugInPingPong::onConnect(IRC::Message const &_message) {
	return onPacket(_message);
}

PlugInResult PlugInPingPong::onPacket(IRC::Message const &_message) {
	PlugInResult result;
	result.newState = STATE_NOP;
	return result;
}

PlugInResult PlugInPingPong::onDisconnect(IRC::Message const &_message) {
	return onPacket(_message);
}

}

