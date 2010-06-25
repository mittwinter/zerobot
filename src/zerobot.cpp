#include <iostream> // TODO: remove after debugging
#include <memory>
#include <sstream>

#include "zerobot.hpp"

namespace zerobot {

ZeroBot::ZeroBot(std::string const &_serverName, int _serverPort) : socket(_serverName, _serverPort), parser(false) {
	state = STATE_CONNECTING;
}

ZeroBot::~ZeroBot() {
	for(data::PriorityQueue< int, PlugIn & >::iterator it = plugIns.begin(); it != plugIns.end(); it++) {
		delete &(it->second);
	}
	plugIns.clear();
}

void ZeroBot::registerPlugIn(PlugIn &_plugIn) {
	plugIns.insert(_plugIn.getPriority(), _plugIn);
}

bool ZeroBot::unregisterPlugIn(std::string const &_name) {
	for(data::PriorityQueue< int, PlugIn & >::iterator it = plugIns.begin(); it != plugIns.end(); it++) {
		if((it->second).getName() == _name) {
			delete &(it->second);
			plugIns.erase(it);
			return true;
		}
	}
	return false;
}

void ZeroBot::run() {
	while(state != STATE_DISCONNECTED) {
		switch(state) {
			case STATE_CONNECTING:
			case STATE_CONNECTED:
				for(data::PriorityQueue< int, PlugIn & >::iterator it = plugIns.begin(); it != plugIns.end(); it++) {
					(it->second).onConnect(state);
				}
				break;
			case STATE_DISCONNECTING:
			case STATE_DISCONNECTED:
				for(data::PriorityQueue< int, PlugIn & >::iterator it = plugIns.begin(); it != plugIns.end(); it++) {
					(it->second).onDisconnect(state);
				}
				break;
			default:
				break;
		}
		processMessage(receiveMessage());
	}
}

std::string ZeroBot::receiveMessage() {
	std::string receivedData;
	while((receivedData = socket.receive()) != "") {
		buffer.append(receivedData);
		std::string::size_type messageEnd = buffer.find('\n');
		if(messageEnd != std::string::npos) {
			// we have a complete message in our buffer,
			// so remove it from the buffer and return it:
			std::string receivedMessage = buffer.substr(0, messageEnd + 1);
			buffer.erase(0, messageEnd + 1);
			std::cout << receivedMessage << std::flush;
			return receivedMessage;
		}
	}
	return "";
}

void ZeroBot::processMessage(std::string _message) {
	// Try to parse message:
	try {
		std::auto_ptr< IRC::Message > message = parser.parseMessage(_message);
		// Should not happen, but check for it here, so that plug-ins do not need to check:
		if(message.get() != NULL) {
			// Process message with plug-ins:
			for(data::PriorityQueue< int, PlugIn & >::iterator it = plugIns.begin(); it != plugIns.end(); it++) {
				std::auto_ptr< PlugInResult > result = (it->second).onPacket(state, *message);
				// Check if plug-in provided result that needs to be processed here:
				if(result.get() != NULL) {
					// Has the bot state changed?
					if(result->newState != STATE_NOP) {
						state = result->newState;
					}
					// Send messages the plug-in possibly returned for sending:
					for(std::list< IRC::Message * >::iterator it = result->messages.begin(); it != result->messages.end(); it++) {
						std::stringstream sstrMessage;
						sstrMessage << (*it);
						socket.send(sstrMessage.str());
						delete (*it);
						*it = NULL;
					}
					result->messages.clear();
				}
			}
		}
	}
	catch(std::runtime_error e) {
		std::cerr << e.what() << std::endl;
	}
}

}

