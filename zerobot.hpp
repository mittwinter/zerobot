#ifndef ZEROBOT_HPP
#define ZEROBOT_HPP

#include <string>

#include "include/priority_queue.hpp"

#include "socket.hpp"
#include "zerobot_plug_in.hpp"

namespace zerobot {

class ZeroBot {
	public:
		ZeroBot(std::string const &_serverName, int _serverPort);

		void registerPlugIn(ZeroBotPlugIn &_plugIn);
		bool unregisterPlugIn(std::string const &_name);

		void run();

	protected:
		ClientSocket socket;
		std::string buffer;
		data::PriorityQueue< int, ZeroBotPlugIn & > plugIns;
};

}

#endif

