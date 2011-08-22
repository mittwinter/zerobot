/*	This file is part of zerobot.
	Copyright (C) 2010-2011 Martin Wegner

	zerobot is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	zerobot is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with zerobot.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <typeinfo>

#include "join.hpp"

namespace zerobot {

PlugInJoin::PlugInJoin(int _priority, std::string const &_channelName) : PlugIn(_priority, "join_" + _channelName), channelName(_channelName), joinState(STATE_JOIN_NOP) {
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
				case IRC::RPL_NAMREPLY:
				case IRC::RPL_ENDOFNAMES:
					for(std::vector< std::string >::const_iterator it = reply.getParamaters().begin(); it != reply.getParamaters().end(); it++) {
						if(*it == channelName) {
							joinState = STATE_JOIN_JOINED;
							std::clog << "PlugInJoin: Joined to channel " << channelName << "." << std::endl;
							break;
						}
					}
					break;
				case IRC::ERR_NOSUCHCHANNEL:
				case IRC::ERR_TOOMANYCHANNELS:
				case IRC::ERR_CHANNELISFULL:
				case IRC::ERR_INVITEONLYCHAN:
				case IRC::ERR_BANNEDFROMCHAN:
				case IRC::ERR_BADCHANNELKEY:
					std::cerr << "PlugInJoin: " << reply << std::flush;
					//result = std::auto_ptr< PlugInResult >(new PlugInResult);
					//result->newState = STATE_DISCONNECTING;
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

