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

#include <cctype>
#include <sstream>
#include <typeinfo>

#include "../lib/rand.hpp"
#include "../lib/version.hpp"
#include "join.hpp"

#include "admin.hpp"

namespace zerobot {

std::vector< std::string > PlugInAdmin::partPhrases;

PlugInAdmin::PlugInAdmin(int _priority, std::string const &_adminNickname): PlugIn(_priority, "admin") {
	adminNickname = _adminNickname;
	if(partPhrases.size() == 0) {
		partPhrases.push_back("Ok, I'll leave you guys alone ... but behave while I'm away!");
		partPhrases.push_back("I was zerobot...ot....ot.....ot......ot...");
		partPhrases.push_back("cya");
	}
}

std::auto_ptr< PlugInResult > PlugInAdmin::onConnect(state_t _state) {
	return std::auto_ptr< PlugInResult >(NULL);
}

std::auto_ptr< PlugInResult > PlugInAdmin::onPacket(state_t _state, IRC::Message const &_message) {
	std::auto_ptr< PlugInResult > result(NULL);
	try {
		IRC::MessagePrivMsg const &privMessage = dynamic_cast< IRC::MessagePrivMsg const & >(_message);
		result = std::auto_ptr< PlugInResult >(new PlugInResult);
		// Admin commands:
		if(checkAdminNickname(privMessage.getPrefix()->getNick())) {
			if(trim(privMessage.getMessage()) == (getCommandPrefix() + "quit")) {
				result->newState = STATE_DISCONNECTING;
			}
			else {
				std::stringstream sstrWordParser;
				sstrWordParser.str(privMessage.getMessage());
				std::string command;
				sstrWordParser >> command;
				if(command == (getCommandPrefix() + "join")) {
					std::string channel;
					sstrWordParser >> channel;
					PlugIn *plugIn = new PlugInJoin(-5, channel);
					result->registerPlugIns.push_back(plugIn);
				}
				else if(command == (getCommandPrefix() + "leave")) {
					std::string channel;
					sstrWordParser >> channel;
					// Check for given channel, otherwise if command was given in channel:
					if((channel.size() == 0 || channel.at(0) != '#') && (privMessage.getReceiver().size() > 1 && privMessage.getReceiver().at(0) == '#')) {
						channel = privMessage.getReceiver();
					}
					if(channel.size() > 0) {
						result->messages.push_back(new IRC::MessagePart(privMessage.getReceiver(), partPhrases.at(util::rand(partPhrases.size() - 1))));
						result->unregisterPlugins.push_back("join_" + channel);
					}
				}
			}
		}
		// Other commands:
		else if(trim(privMessage.getMessage()) == (getCommandPrefix() + "version")) {
			result = std::auto_ptr< PlugInResult >(new PlugInResult);
			result->messages.push_back(new IRC::MessagePrivMsg(privMessage.getPrefix()->getNick(), zerobot::versionString));
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

bool PlugInAdmin::checkAdminNickname(std::string const &_nickname) const {
	return (adminNickname.size() > 0 && adminNickname == _nickname);
}

}
