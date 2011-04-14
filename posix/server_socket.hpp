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

#ifndef SERVER_SOCKET_HPP
#define SERVER_SOCKET_HPP

#include <list>
#include <netdb.h>
#include <set>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>

#include "socket.hpp"

namespace posix {

class ServerSocket : public Socket {
	public:
		ServerSocket(int _port, int _family = AF_UNSPEC, int _type = SOCK_STREAM, int _protocol = getprotobyname("tcp")->p_proto, bool _blocking = true) throw(std::runtime_error);
		virtual ~ServerSocket();

		virtual void accept() throw(std::runtime_error);
		virtual void listen() throw(std::runtime_error);

		virtual std::string receive() throw(std::runtime_error, std::invalid_argument);
		virtual std::string receiveFrom(std::string &_srcIP, int &_srcPort) throw(std::runtime_error);
		virtual void send(std::string const &_data) throw(std::runtime_error);
		virtual void sendTo(std::string const &_data, std::string const &_destination, int _destinationPort) throw(std::runtime_error);

		void closeClientSocket() throw(std::runtime_error);

		int getPort() const { return port; }
		int getClientSocket() const { return clientSocket; }
		bool hasAccepted() const { return (getClientSocket() > 0); }

	protected:
		int port;
		int clientSocket;

		using Socket::receive;
		using Socket::receiveFrom;
		using Socket::send;
		using Socket::sendTo;

		virtual void listen(int _socket) throw(std::runtime_error);
		virtual int accept(int _socket) throw(std::runtime_error);
};

class SocketResult {
	public:
		virtual ~SocketResult() {}

		int socket;
};

class SocketClosed : public SocketResult {
	public:
		virtual ~SocketClosed() {}
};

class SocketByteString : public SocketResult {
	public:
		virtual ~SocketByteString() {}

		std::string data;
};

class ServerMultiSocket : public ServerSocket {
	public:
		typedef std::set< int >::iterator iterator;
		typedef std::set< int >::const_iterator const_iterator;

		ServerMultiSocket(int _port, int _family = AF_UNSPEC, int _protocol = getprotobyname("tcp")->p_proto, bool _blocking = true) throw(std::runtime_error);
		virtual ~ServerMultiSocket();

		virtual std::list< SocketResult * > process() throw(std::runtime_error);

		virtual void send(int _client, std::string const &_data) throw(std::runtime_error);
		virtual std::string receive(int _client) throw(std::runtime_error);

		virtual void sendAll(std::string const &_data) throw(std::runtime_error);
		virtual void sendAllExcept(std::string const &_data, std::set< int > _exceptClients) throw(std::runtime_error);
		virtual void sendAllExcept(std::string const &_data, int _exceptClient) throw(std::runtime_error);

		iterator begin() { return clientSockets.begin(); }
		const_iterator begin() const { return clientSockets.begin(); }
		iterator end() { return clientSockets.end(); }
		const_iterator end() const { return clientSockets.end(); }
		iterator find(int _socket) { return clientSockets.find(_socket); }
		const_iterator find(int _socket) const { return clientSockets.find(_socket); }

		void shutdownClient(iterator _itSocket);

	protected:
		std::set< int > clientSockets;

		iterator addClient(int _socket);
};

}

#endif

