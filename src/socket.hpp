/*	This file is part of zerobot.

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

#include <stdexcept>
#include <string>

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

namespace zerobot {

class Socket {
	public:
		Socket(int _domain = AF_INET, int _type = SOCK_STREAM, int _protocol = getprotobyname("tcp")->p_proto);
		virtual ~Socket();

	protected:
		int domain;
		int type;
		int protocol;
		int socket;
};

// TODO: Refactor ServerSocket for IPv6!
class ServerSocket : public Socket {
	public:
		ServerSocket(int _port, int _domain = AF_INET, int _type = SOCK_STREAM, int _protocol = getprotobyname("tcp")->p_proto);
		virtual ~ServerSocket();

		void accept();

		std::string receive();
		void send(std::string const &_data);

	protected:
		static const unsigned int BUFFER_SIZE;

		int clientSocket;
};

class ClientSocket : public Socket {
	public:
		ClientSocket(std::string const &_serverName, int _serverPort, int _domain = AF_UNSPEC, int _type = SOCK_STREAM, int _protocol = getprotobyname("tcp")->p_proto);
		virtual ~ClientSocket() {}

		std::string receive() throw(std::runtime_error);
		void send(std::string const &_data) throw(std::runtime_error);

	protected:
		static const unsigned int BUFFER_SIZE;
};

}

#endif

