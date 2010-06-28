#include "../src/rand.hpp"

#include "disconnect.hpp"

namespace zerobot {

std::vector< std::string > PlugInDisconnect::quitMessages;

PlugInDisconnect::PlugInDisconnect(int _priority) : PlugIn(_priority, "disconnect") {
	if(quitMessages.size() == 0) {
		quitMessages.push_back("I'll die alone ... and disconnected.");
		quitMessages.push_back("PADI! - Fight for your right!");
	}
}

std::auto_ptr< PlugInResult > PlugInDisconnect::onConnect(state_t _state) {
	return std::auto_ptr< PlugInResult >(NULL);
}

std::auto_ptr< PlugInResult > PlugInDisconnect::onPacket(state_t _state, IRC::Message const &_message) {
	return std::auto_ptr< PlugInResult >(NULL);
}

std::auto_ptr< PlugInResult > PlugInDisconnect::onTimeTrigger(state_t _state) {
	return std::auto_ptr< PlugInResult >(NULL);
}

std::auto_ptr< PlugInResult > PlugInDisconnect::onDisconnect(state_t _state) {
	std::auto_ptr< PlugInResult > result(NULL);
	switch(_state) {
		case STATE_DISCONNECTING:
			result = std::auto_ptr< PlugInResult >(new PlugInResult);
			result->newState = STATE_DISCONNECTED;
			result->messages.push_back(new IRC::MessageQuit(quitMessages.at(util::rand(quitMessages.size()))));
			break;
		default:
			break;
	}
	return result;
}

}

