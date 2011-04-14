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

#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <netdb.h>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <string>

namespace posix {

class Socket {
	public:
		Socket(int _family = AF_UNSPEC, int _type = SOCK_STREAM, int _protocol = getprotobyname("tcp")->p_proto, bool _blocking = true);
		virtual ~Socket();

		void setBlocking(bool _blocking);
		bool isBlocking() const { return blocking; }

		int getSocket() const { return socket; }

	protected:
		static unsigned int const BUFFER_SIZE;

		int family;
		int type;
		int protocol;
		bool blocking;

		int socket;

		virtual std::string receive(int _socket) throw(std::runtime_error, std::invalid_argument);
		virtual std::string receiveFrom(int _socket, std::string &_srcIP, int &_srcPort) throw(std::runtime_error);
		virtual void send(int _socket, std::string const &_data) throw(std::runtime_error);
		virtual void sendTo(int _socket, std::string const &_data, std::string const &_destination, int _destinationPort) throw(std::runtime_error);
};

bool resolveSocketConnection(int _socket, char *_hostname, size_t _hostnameSize, char *_ipAddress, size_t _ipAddressSize);
bool resolveSocketConnection(int _socket, std::string &_hostname, std::string &_ipAddress);

}

#endif

