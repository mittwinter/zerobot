#include <cctype>
#include <typeinfo>

#include "admin.hpp"

namespace zerobot {

PlugInAdmin::PlugInAdmin(int _priority, std::string const &_adminNickname): PlugIn(_priority, "admin") {
	adminNickname = _adminNickname;
}

std::auto_ptr< PlugInResult > PlugInAdmin::onConnect(state_t _state) {
	return std::auto_ptr< PlugInResult >(NULL);
}

std::auto_ptr< PlugInResult > PlugInAdmin::onPacket(state_t _state, IRC::Message const &_message) {
	std::auto_ptr< PlugInResult > result(NULL);
	try {
		IRC::MessagePrivMsg const &privMessage = dynamic_cast< IRC::MessagePrivMsg const & >(_message);
		if(privMessage.getPrefix()->getNick() == adminNickname && trim(privMessage.getMessage()) == (getCommandPrefix() + "quit")) {
			result = std::auto_ptr< PlugInResult >(new PlugInResult);
			result->newState = STATE_DISCONNECTING;
		}
	}
	catch(std::bad_cast) {}
	return result;
}

std::auto_ptr< PlugInResult > PlugInAdmin::onTimeTrigger(state_t _state) {
	return std::auto_ptr< PlugInResult >(NULL);
}

std::auto_ptr< PlugInResult > PlugInAdmin::onDisconnect(state_t _state) {
	return std::auto_ptr< PlugInResult >(NULL);
}

std::string PlugInAdmin::trim(std::string _str) const {
	while(_str.size() > 0 && (isspace(_str.at(0)) || iscntrl(_str.at(0)))) {
		_str.erase(0, 1);
	}
	while(_str.size() > 0 && (isspace(_str.at(_str.size() - 1)) || iscntrl(_str.at(_str.size() - 1)))) {
		_str.erase(_str.size() - 1, 1);
	}
	return _str;
}

}