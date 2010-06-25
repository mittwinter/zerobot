#ifndef ZEROBOT_HPP
#define ZEROBOT_HPP

#include <string>

#include "include/priority_queue.hpp"
#include "irc/parser.hpp"
#include "socket.hpp"
#include "plugins/base.hpp"
#include "zerobot_state.hpp"

namespace zerobot {

class ZeroBot {
	public:
		ZeroBot(std::string const &_serverName, int _serverPort);
		~ZeroBot();

		void registerPlugIn(PlugIn &_plugIn);
		bool unregisterPlugIn(std::string const &_name);

		void run();

	protected:
		state_t state;
		ClientSocket socket;
		std::string buffer;
		IRC::Parser parser;
		data::PriorityQueue< int, PlugIn & > plugIns;

		std::string receiveMessage();
		void processMessage(std::string _message);
};

}

#endif

