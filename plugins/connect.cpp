#include "connect.hpp"

namespace zerobot {

PlugInConnect::PlugInConnect(int _priority, std::string _botNickname) : PlugIn(_priority, "connect"), botNickname(_botNickname) {
	connectState = STATE_CONNECT_NOP;
}

std::auto_ptr< PlugInResult > PlugInConnect::onConnect(state_t _state) {
	std::auto_ptr< PlugInResult > result(NULL);
	if(_state == STATE_CONNECTING) {
		switch(connectState) {
			case STATE_CONNECT_NOP:
				result = std::auto_ptr< PlugInResult >(new PlugInResult);
				result->messages.push_back(new IRC::MessageNick(botNickname));
				connectState = STATE_CONNECT_NICK_SENT;
				break;
			default:
				break;
		}
	}
	return result;
}

std::auto_ptr< PlugInResult > PlugInConnect::onPacket(state_t _state, IRC::Message const &_message) {
	return std::auto_ptr< PlugInResult >(NULL);
}

std::auto_ptr< PlugInResult > PlugInConnect::onDisconnect(state_t _state) {
	return std::auto_ptr< PlugInResult >(NULL);
}

}

