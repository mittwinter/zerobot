#include <iostream> // TODO: remove after debugging

#include "zerobot.hpp"

namespace zerobot {

ZeroBot::ZeroBot(std::string const &_serverName, int _serverPort) : socket(_serverName, _serverPort) {
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
