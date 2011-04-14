/*	This file is part of P2P chat.
	Copyright (C) 2010 Martin Wegner

	P2P chat is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	P2P chat is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with P2P chat.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CLIENT_SOCKET_HPP
#define CLIENT_SOCKET_HPP

#include <netdb.h>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>

#include "socket.hpp"

namespace posix {

class ClientSocket : public Socket {
	public:
		ClientSocket(std::string const &_serverName, int _serverPort, int _family = AF_UNSPEC, int _type = SOCK_STREAM, int _protocol = getprotobyname("tcp")->p_proto, bool _blocking = true) throw(std::runtime_error);
		virtual ~ClientSocket() {}

		std::string receive() throw(std::runtime_error, std::invalid_argument);
		void send(std::string const &_data) throw(std::runtime_error);

	protected:
		using Socket::send;
		using Socket::receive;
};

}

#endif

