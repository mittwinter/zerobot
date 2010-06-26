#ifndef ZEROBOT_HPP
#define ZEROBOT_HPP

#include <memory>
#include <string>

#include "../include/priority_queue.hpp"
#include "../irc/parser.hpp"
#include "../plugins/base.hpp"
#include "socket.hpp"
#include "zerobot_state.hpp"

namespace zerobot {

class ZeroBot {
	public:
		ZeroBot(std::string const &_serverName, int _serverPort);
		~ZeroBot();

		void registerPlugIn(PlugIn &_plugIn);
		bool unregisterPlugIn(std::string const &_name);

		void run();

		std::string const &getServerName() const { return serverName; }
		int getServerPort() const { return serverPort; }

	protected:
		std::string serverName;
		int serverPort;

		state_t state;
		ClientSocket socket;
		std::string buffer;
		IRC::Parser parser;
		data::PriorityQueue< int, PlugIn & > plugIns;

		void processResult(std::auto_ptr< PlugInResult > _result);
		std::string receiveMessage();
		void processMessage(std::string const &_message);
};

}

#endif

