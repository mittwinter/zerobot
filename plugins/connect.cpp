#include <bits/local_lim.h> // for HOST_NAME_MAX (POSIX limit)
#include <typeinfo>
#include <unistd.h>

#include "connect.hpp"

namespace zerobot {

PlugInConnect::PlugInConnect(int _priority, std::string _botNickname) : PlugIn(_priority, "connect") {
	botNickname = _botNickname;
	connectState = STATE_CONNECT_NOP;
}

std::auto_ptr< PlugInResult > PlugInConnect::onConnect(state_t _state) {
	std::auto_ptr< PlugInResult > result(NULL);
	if(_state == STATE_CONNECTING) {
		switch(connectState) {
			case STATE_CONNECT_NOP:
				result = std::auto_ptr< PlugInResult >(new PlugInResult);
				result->messages.push_back(new IRC::MessageNick(botNickname));
				char hostnameBuffer[HOST_NAME_MAX];
				gethostname(hostnameBuffer, HOST_NAME_MAX);
				result->messages.push_back(new IRC::MessageUser(botNickname, hostnameBuffer, "foobar", botNickname));
				connectState = STATE_CONNECT_NICK_USER_SENT;
				break;
			default:
				break;
		}
	}
	return result;
}

std::auto_ptr< PlugInResult > PlugInConnect::onPacket(state_t _state, IRC::Message const &_message) {
	std::auto_ptr< PlugInResult > result(NULL);
	try {
		IRC::MessageNumericReply const &reply = dynamic_cast< IRC::MessageNumericReply const & >(_message);
		if(connectState == STATE_CONNECT_NICK_USER_SENT
		    && (reply.getReplyCode() == IRC::ERR_NONICKNAMEGIVEN 
		        || reply.getReplyCode() == IRC::ERR_ERRONEUSNICKNAME
		        || reply.getReplyCode() == IRC::ERR_NICKNAMEINUSE
		        || reply.getReplyCode() == IRC::ERR_NICKCOLLISION
		        || reply.getReplyCode() == IRC::ERR_ALREADYREGISTERED)) {
			std::cerr << "PlugInConnect: " << reply << std::flush;
			result = std::auto_ptr< PlugInResult >(new PlugInResult);
			result->newState = STATE_DISCONNECTING;
		}
	}
	catch(std::bad_cast) {
	}
	return result;
}

std::auto_ptr< PlugInResult > PlugInConnect::onTimeTrigger(state_t _state) {
	return std::auto_ptr< PlugInResult >(NULL);
}

std::auto_ptr< PlugInResult > PlugInConnect::onDisconnect(state_t _state) {
	return std::auto_ptr< PlugInResult >(NULL);
}

}

