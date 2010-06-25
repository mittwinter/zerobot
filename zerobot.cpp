#include <iostream> // TODO: remove after debugging

#include "zerobot.hpp"

namespace zerobot {

ZeroBot::ZeroBot(std::string const &_serverName, int _serverPort) : socket(_serverName, _serverPort) {
}

void ZeroBot::registerPlugIn(ZeroBotPlugIn &_plugIn) {
	plugIns.insert(_plugIn.getPriority(), _plugIn);
}

bool ZeroBot::unregisterPlugIn(std::string const &_name) {
	for(data::PriorityQueue< int, ZeroBotPlugIn & >::iterator it = plugIns.begin(); it != plugIns.end(); it++) {
		if((it->second).getName() == _name) {
			plugIns.erase(it);
			return true;
		}
	}
	return false;
}

void ZeroBot::run() {
	std::string receivedData;
	while((receivedData = socket.receive()) != "") {
		buffer.append(receivedData);
		std::string::size_type messageEnd = buffer.find('\n');
		if(messageEnd != std::string::npos) { // we have a complete message in our buffer, so process it:
			std::string receivedMessage = buffer.substr(0, messageEnd + 1);
			buffer.erase(0, messageEnd + 1);
			std::cout << receivedMessage << std::flush;
		}
	}
}

}
