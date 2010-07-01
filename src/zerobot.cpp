#include <iostream> // TODO: remove after debugging
#include <memory>
#include <sstream>
#include <time.h> // for nanosleep()

#include "zerobot.hpp"

namespace zerobot {

ZeroBot::ZeroBot(std::string const &_serverName, int _serverPort) : socket(_serverName, _serverPort), parser(false) {
	serverName = _serverName;
	serverPort = _serverPort;
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
		std::auto_ptr< PlugInResult > result(NULL);
		switch(state) {
			case STATE_CONNECTING:
			case STATE_CONNECTED:
				for(data::PriorityQueue< int, PlugIn & >::iterator it = plugIns.begin(); it != plugIns.end(); it++) {
					processResult((it->second).onConnect(state));
				}
				break;
			case STATE_DISCONNECTING:
			case STATE_DISCONNECTED:
				for(data::PriorityQueue< int, PlugIn & >::iterator it = plugIns.begin(); it != plugIns.end(); it++) {
					processResult((it->second).onDisconnect(state));
				}
				break;
			default:
				break;
		}
		std::string receivedMessage;
		do {
			receivedMessage = receiveMessage();
			if(receivedMessage.size() > 0) {
				processMessage(receivedMessage);
			}
		}
		while(receivedMessage.size() > 0);
		// time-trigger plug-ins:
		for(data::PriorityQueue< int, PlugIn & >::iterator it = plugIns.begin(); it != plugIns.end(); it++) {
			processResult((it->second).onTimeTrigger(state));
		}
		struct timespec sleepTime;
		sleepTime.tv_sec = 0;
		sleepTime.tv_nsec = 500000000;
		nanosleep(&sleepTime, NULL);
	}
}

void ZeroBot::processResult(std::auto_ptr< PlugInResult > _result) {
	// Check if result was actually provided that needs to be processed here:
	if(_result.get() != NULL) {
		// Has the bot state changed?
		if(_result->newState != STATE_NOP) {
			state = _result->newState;
		}
		// Send messages that were possibly returned for sending:
		for(std::list< IRC::Message * >::iterator it = _result->messages.begin(); it != _result->messages.end(); it++) {
			std::stringstream sstrMessage;
			sstrMessage << *(*it);
			socket.send(sstrMessage.str());
			delete (*it);
			*it = NULL;
		}
		_result->messages.clear();
	}
}

std::string ZeroBot::receiveMessage() {
	std::string receivedData;
	while((receivedData = socket.receive()) != "") {
		buffer.append(receivedData);
	}
	std::string::size_type messageEnd = buffer.find('\n');
	if(messageEnd != std::string::npos) {
		// we have a complete message in our buffer,
		// so remove it from the buffer and return it:
		std::string receivedMessage = buffer.substr(0, messageEnd + 1);
		buffer.erase(0, messageEnd + 1);
		std::cout << "<<< " << receivedMessage << std::flush; // TODO: remove this debug output
		return receivedMessage;
	}
	else {
		return "";
	}
}

void ZeroBot::processMessage(std::string const &_message) {
	// Try to parse message:
	try {
		std::auto_ptr< IRC::Message > message = parser.parseMessage(_message);
		// Should not happen, but check for it here, so that plug-ins do not necessarily need to check:
		if(message.get() != NULL) {
			// Process message with plug-ins:
			for(data::PriorityQueue< int, PlugIn & >::iterator it = plugIns.begin(); it != plugIns.end(); it++) {
				processResult((it->second).onPacket(state, *message));
			}
		}
	}
	catch(std::runtime_error e) {
		std::cerr << e.what() << std::endl;
	}
}

}

