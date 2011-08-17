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

#ifndef ZEROBOT_HPP
#define ZEROBOT_HPP

#include <memory>
#include <string>

#include "../include/priority_queue.hpp"
#include "../lib/irc/parser.hpp"
#include "../plugins/base.hpp"
#include "../posix/client_socket.hpp"
#include "../lib/zerobot_state.hpp"

namespace zerobot {

class ZeroBot {
	public:
		ZeroBot(std::string const &_serverName, int _serverPort);
		~ZeroBot();

		void registerPlugIn(PlugIn &_plugIn);
		void registerAdminPlugIn(PlugInAdminBase &_plugIn);
		bool unregisterPlugIn(std::string const &_name);

		void run();

		std::string const &getServerName() const { return serverName; }
		int getServerPort() const { return serverPort; }

	protected:
		std::string serverName;
		int serverPort;

		state_t state;
		posix::ClientSocket socket;
		std::string buffer;
		IRC::Parser parser;
		data::PriorityQueue< int, PlugIn & > plugIns;

		state_t getState() const { return state; }
		void setState(state_t state) { this->state = state; }
		bool isConnected() const { return (getState() != STATE_DISCONNECTED); }

		void receiveMessages();
		void timeTriggerPlugins();
		void sleep() const;

		std::string receiveMessage();
		void sendMessage(IRC::Message const &message);

		void processResult(std::auto_ptr< PlugInResult > _result);
		void processMessage(std::string const &_message);
};

}

#endif

