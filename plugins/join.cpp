#include <iostream>
#include <typeinfo>

#include "join.hpp"

namespace zerobot {

PlugInJoin::PlugInJoin(int _priority, std::string const &_channelName) : PlugIn(_priority, "join") {
	channelName = _channelName;
	joinState = STATE_JOIN_NOP;
}

std::auto_ptr< PlugInResult > PlugInJoin::onConnect(state_t _state) {
	return std::auto_ptr< PlugInResult >(NULL);
}

std::auto_ptr< PlugInResult > PlugInJoin::onPacket(state_t _state, IRC::Message const &_message) {
	std::auto_ptr< PlugInResult > result(NULL);
	if(joinState == STATE_JOIN_SENT) {
		try {
			IRC::MessageNumericReply const &reply = dynamic_cast< IRC::MessageNumericReply const & >(_message);
			switch(reply.getReplyCode()) {
				case IRC::RPL_TOPIC:
				case IRC::RPL_NOTOPIC:
				case IRC::RPL_NAMEREPLY:
				case IRC::RPL_ENDOFNAMES:
					joinState = STATE_JOIN_JOINED;
					std::cout << "PlugInJoin: Joined to channel " << channelName << "." << std::endl;
					break;
				case IRC::ERR_NOSUCHCHANNEL:
				case IRC::ERR_TOOMANYCHANNELS:
				case IRC::ERR_CHANNELISFULL:
				case IRC::ERR_INVITEONLYCHAN:
				case IRC::ERR_BANNEDFROMCHAN:
				case IRC::ERR_BADCHANNELKEY:
					std::cerr << "PlugInJoin: " << reply << std::flush;
					result = std::auto_ptr< PlugInResult >(new PlugInResult);
					result->newState = STATE_DISCONNECTING;
					break;
				default:
					break;
			}
		}
		catch(std::bad_cast) {
		}
	}
	return result;
}

std::auto_ptr< PlugInResult > PlugInJoin::onTimeTrigger(state_t _state) {
	std::auto_ptr< PlugInResult > result(NULL);
	if(_state == STATE_CONNECTED && joinState == STATE_JOIN_NOP) {
		result = std::auto_ptr< PlugInResult >(new PlugInResult);
		result->messages.push_back(new IRC::MessageJoin(channelName));
		joinState = STATE_JOIN_SENT;
	}
	return result;
}

std::auto_ptr< PlugInResult > PlugInJoin::onDisconnect(state_t _state) {
	return std::auto_ptr< PlugInResult >(NULL);
}

}

