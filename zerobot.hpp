#ifndef ZEROBOT_HPP
#define ZEROBOT_HPP

#include <string>

#include "socket.hpp"

namespace zerobot {

class ZeroBot {
	public:
		ZeroBot(std::string const &_serverName, int _serverPort);

		void run();

	protected:
		ClientSocket socket;
		std::string buffer;
};

}

#endif

