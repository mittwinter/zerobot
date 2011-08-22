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

#include <iostream> // TODO: remove after debugging
#include <memory>
#include <sstream>
#include <time.h> // for nanosleep()

#include "zerobot.hpp"

namespace zerobot {

ZeroBot::ZeroBot(std::string const &_serverName, int _serverPort) : socket(_serverName, _serverPort, AF_UNSPEC, SOCK_STREAM, getprotobyname("tcp")->p_proto, false), parser(false) {
	serverName = _serverName;
	serverPort = _serverPort;
	setState(STATE_CONNECTING);
}

ZeroBot::~ZeroBot() {
	for(data::PriorityQueue< int, PlugIn * >::iterator it = plugIns.begin(); it != plugIns.end(); it++) {
		delete it->second;
	}
	plugIns.clear();
}

void ZeroBot::registerPlugIn(PlugIn *_plugIn) {
	plugIns.insert(_plugIn->getPriority(), _plugIn);
}

bool ZeroBot::unregisterPlugIn(std::string const &_name) {
	for(data::PriorityQueue< int, PlugIn * >::iterator it = plugIns.begin(); it != plugIns.end(); it++) {
		if((it->second)->getName() == _name) {
			delete it->second;
			plugIns.erase(it);
			return true;
		}
	}
	return false;
}

void ZeroBot::run() {
	while(isConnected()) {
		// Run onConnect() triggers of plug-ins if appropiate:
		if(getState() == STATE_CONNECTING) {
			for(data::PriorityQueue< int, PlugIn * >::iterator it = plugIns.begin(); it != plugIns.end(); it++) {
				processResult(it->second->onConnect(getState()));
			}
		}
		// Receive messages:
		if(isConnected()) {
			receiveMessages();
		}
		// Time-trigger plug-ins:
		if(isConnected()) {
			timeTriggerPlugins();
		}
		// Run onDisconnect() triggers of plug-ins if appropiate:
		if(getState() == STATE_DISCONNECTING) {
			for(data::PriorityQueue< int, PlugIn * >::reverse_iterator it = plugIns.rbegin(); it != plugIns.rend(); it++) {
				processResult(it->second->onDisconnect(getState()));
			}
		}
		if(isConnected()) {
			sleep();
		}
	}
	// We were disconnected, so run onDisconnect() triggers one last time with this state:
	if(getState() == STATE_DISCONNECTED) {
		for(data::PriorityQueue< int, PlugIn * >::iterator it = plugIns.begin(); it != plugIns.end(); it++) {
			processResult(it->second->onDisconnect(getState()));
		}
	}
}

void ZeroBot::receiveMessages() {
	std::string receivedMessage;
	do {
		receivedMessage = receiveMessage();
		if(receivedMessage.size() > 0) {
			processMessage(receivedMessage);
		}
	}
	while(receivedMessage.size() > 0);
}

void ZeroBot::timeTriggerPlugins() {
	for(data::PriorityQueue< int, PlugIn * >::iterator it = plugIns.begin(); it != plugIns.end(); it++) {
		processResult(it->second->onTimeTrigger(getState()));
	}
}

void ZeroBot::sleep() const {
	struct timespec sleepTime;
	sleepTime.tv_sec = 0;
	sleepTime.tv_nsec = 500000000;
	nanosleep(&sleepTime, NULL);
}

std::string ZeroBot::receiveMessage() {
	std::string receivedData;
	try {
		while((receivedData = socket.receive()).size() > 0) {
			buffer.append(receivedData);
		}
	}
	catch(std::exception const &e) { // Socket was closed or some other error occured:
		std::cerr << "ZeroBot: Socket operation receive() failed, assuming it's disconnected: " << e.what() << std::endl;
		setState(STATE_DISCONNECTED);
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
		return std::string();
	}
}

void ZeroBot::sendMessage(IRC::Message const &message) {
	std::stringstream sstrMessage;
	sstrMessage << message;
	std::cout << ">>> " << sstrMessage.str() << std::flush;
	try {
		socket.send(sstrMessage.str());
	}
	catch(std::exception e) { // Socket was closed or some other error occured:
		std::cerr << "ZeroBot: Socket operation receive() failed, assuming it's disconnected: " << e.what() << std::endl;
		setState(STATE_DISCONNECTED);
	}
}

void ZeroBot::processResult(std::auto_ptr< PlugInResult > _result) {
	// Check if result was actually provided that needs to be processed here:
	if(_result.get() != NULL) {
		// Send messages that were possibly returned for sending:
		// Careful! Do this only as lon as we are connected! (send() operation may fail.)
		for(std::list< IRC::Message * >::iterator it = _result->messages.begin(); isConnected() && it != _result->messages.end(); it++) {
			sendMessage(*(*it));
			delete *it;
			*it = NULL;
		}
		_result->messages.clear();
		// Has the bot state changed?
		if(_result->newState != STATE_NOP) {
			setState(_result->newState);
		}
		for(std::list< PlugIn * >::iterator it = _result->registerPlugIns.begin(); it != _result->registerPlugIns.end(); it++) {
			registerPlugIn(*it);
		}
		for(std::list< std::string >::iterator it = _result->unregisterPlugins.begin(); it != _result->unregisterPlugins.end(); it++) {
			unregisterPlugIn(*it);
		}
	}
}

void ZeroBot::processMessage(std::string const &_message) {
	// Try to parse message:
	try {
		std::auto_ptr< IRC::Message > message = parser.parseMessage(_message);
		// Should not happen, but check for it here, so that plug-ins do not necessarily need to check:
		if(message.get() != NULL) {
			// Process message with plug-ins:
			for(data::PriorityQueue< int, PlugIn * >::iterator it = plugIns.begin(); it != plugIns.end(); it++) {
				processResult(it->second->onPacket(getState(), *message));
			}
		}
	}
	catch(std::runtime_error e) {
		std::cerr << e.what() << std::endl;
	}
}

}

