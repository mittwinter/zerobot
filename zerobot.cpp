#include <iostream> // TODO: remove after debugging
#include <memory>

#include "zerobot.hpp"

namespace zerobot {

ZeroBot::ZeroBot(std::string const &_serverName, int _serverPort) : socket(_serverName, _serverPort) {
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
	// parse message:
	try {
		std::auto_ptr< IRC::Message > message = parser.parseMessage(_message);
		// process message with plug-ins:
		for(data::PriorityQueue< int, PlugIn & >::iterator it = plugIns.begin(); it != plugIns.end(); it++) {
			PlugInResult result;
			result.newState = STATE_NOP;
			switch(state) {
				case STATE_CONNECTING:
					result = (it->second).onConnect(*message);
					break;
				case STATE_CONNECTED:
				case STATE_RUNNING:
					result = (it->second).onPacket(*message);
					break;
				case STATE_DISCONNECTING:
					result = (it->second).onDisconnect(*message);
					break;
				case STATE_DISCONNECTED:
				case STATE_NOP:
				default:
					break;
			}
			if(result.newState != STATE_NOP) {
				state = result.newState;
			}
			for(std::list< std::string >::const_iterator it = result.messages.begin(); it != result.messages.end(); it++) {
				socket.send(*it);
			}
		}
	}
	catch(std::runtime_error e) {
		std::cerr << e.what() << std::endl;
	}
}

}

