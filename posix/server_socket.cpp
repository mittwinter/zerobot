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

#include <cstring>
#include <errno.h>
#include <iostream>
#include <sstream>
#include <sys/time.h>

#include "server_socket.hpp"

namespace posix {

ServerSocket::ServerSocket(int _port, int _family, int _type, int _protocol, bool _blocking) throw(std::runtime_error) : Socket(_family, _type, _protocol, _blocking), port(_port), clientSocket(0) {

	// Setup data structured as needed by getaddrinfo():
	struct addrinfo hints;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = family;
	hints.ai_socktype = type;
	hints.ai_protocol = protocol;
	hints.ai_flags = AI_PASSIVE;
	// Convert given port to string:
	std::stringstream sstrPort;
	sstrPort << _port;
	// Finally call getaddrinfo():
	struct addrinfo *result = NULL;
	int gaiError = getaddrinfo(NULL, sstrPort.str().c_str(), &hints, &result);
	if(gaiError != 0) {
		throw std::runtime_error(gai_strerror(gaiError));
	}
	// Iterate over all returned address infos and try to bind to each one,
	// until one is successful:
	struct addrinfo *resultIterator = NULL;
	for(resultIterator = result; resultIterator != NULL; resultIterator = resultIterator->ai_next) {
		socket = ::socket(resultIterator->ai_family, resultIterator->ai_socktype, resultIterator->ai_protocol);
		// Attempt to open socket failed, try next one (iteration, if available):
		if(socket == -1) {
			if(close(socket) == -1) {
				throw std::runtime_error(strerror(errno));
			}
			socket = 0;
			continue;
		}
		// Reuse socket if it is in TIME_WAIT state:
		int reuse = 1;
		if(setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) == -1) {
			if(close(socket) == -1) {
				throw std::runtime_error(strerror(errno));
			}
			throw std::runtime_error(std::string("posix::ServerSocket: setsockopt() with SO_REUSEADDR failed().\n") + strerror(errno));
		}
		// Bind to socket:
		if(bind(socket, resultIterator->ai_addr, resultIterator->ai_addrlen) == 0) {
			std::clog << "posix::ServerSocket: Successfully bound to port " << _port << "." << std::endl;
			break;
		}
		else { // bind() failed:
			std::cerr << "posix::ServerSocket: Failed to bind() to port " << _port << "." << std::endl;
			if(close(socket) == -1) {
				throw std::runtime_error(strerror(errno));
			}
			socket = 0;
		}
	}
	// Clean up result by getaddrinfo():
	freeaddrinfo(result);
	result = NULL;
	// Check wether all attempts failed or not:
	if(resultIterator == NULL) {
		throw std::runtime_error("posix::ServerSocket: Failed to bind to any socket returned by getaddrinfo().");
	}
}

ServerSocket::~ServerSocket() {
	if(clientSocket > 0) {
		if(close(clientSocket) == -1) {
			std::cerr << "posix::~ServerSocket(): close() failed." << std::endl;
			std::cerr << "\t" << strerror(errno) << std::endl;
		}
		clientSocket = 0;
	}
}

void ServerSocket::listen() throw(std::runtime_error) {
	listen(socket);
}

void ServerSocket::accept() throw(std::runtime_error) {
	if(clientSocket > 0) {
		throw std::runtime_error("posix::ServerSocket: Client connection already accept()ed.");
	}
	clientSocket = accept(socket);
	if(clientSocket == -1) {
		clientSocket = 0;
		throw std::runtime_error(std::string("posix::ServerSocket: accept() failed.\n\t") + strerror(errno));
	}
}

std::string ServerSocket::receive() throw(std::runtime_error, std::invalid_argument) {
	if(type == SOCK_STREAM) {
		return receive(clientSocket);
	}
	else { // FIXME: if(type == SOCK_DGRAM)
		return receive(socket);
	}
}

std::string ServerSocket::receiveFrom(std::string &_srcIP, int &_srcPort) throw(std::runtime_error) {
	if(type == SOCK_STREAM) {
		return receiveFrom(clientSocket, _srcIP, _srcPort);
	}
	else { // FIXME: if(type == SOCK_DGRAM)
		return receiveFrom(socket, _srcIP, _srcPort);
	}
}

void ServerSocket::send(std::string const &_data) throw(std::runtime_error) {
	send(clientSocket, _data);
}

void ServerSocket::sendTo(std::string const &_data, std::string const &_destination, int _destinationPort) throw(std::runtime_error) {
	sendTo(socket, _data, _destination, _destinationPort);
}

void ServerSocket::closeClientSocket() throw(std::runtime_error) {
	if(close(clientSocket) == -1) {
		std::cerr << "posix::~ServerSocket(): close() failed." << std::endl;
		std::cerr << "\t" << strerror(errno) << std::endl;
	}
	clientSocket = 0;
}

void ServerSocket::listen(int _socket) throw(std::runtime_error) {
	if(type != SOCK_STREAM) {
		throw std::runtime_error("posix::ServerSocket: listen() called on socket with type != SOCK_STREAM!");
	}
	if(::listen(socket, 8) == -1) {
		throw std::runtime_error(std::string("posix::ServerSocket: listen() failed.\n\t") + strerror(errno));
	}
}

int ServerSocket::accept(int _socket) throw(std::runtime_error) {
	if(type != SOCK_STREAM) {
		throw std::runtime_error("posix::ServerSocket: accept() called on socket with type != SOCK_STREAM!");
	}
	fd_set socketSet;
	FD_ZERO(&socketSet);
	FD_SET(socket, &socketSet);
	if(isBlocking()) {
		if(select(socket + 1, &socketSet, NULL, NULL, NULL) == -1) {
			throw std::runtime_error(strerror(errno));
		}
	}
	else {
		struct timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 0;
		if(select(socket + 1, &socketSet, NULL, NULL, &timeout) == -1) {
			throw std::runtime_error(strerror(errno));
		}
	}
	if(FD_ISSET(socket, &socketSet)) {
		int acceptedSocket = ::accept(socket, NULL, NULL);
		if(acceptedSocket == -1) {
			throw std::runtime_error(std::string("posix::ServerSocket: accept() failed.\n\t") + strerror(errno));
		}
		char *clientHostname = new char[512];
		char *clientIPAddress = new char[40]; // IPv6 address + \0
		if(resolveSocketConnection(acceptedSocket, clientHostname, 512, clientIPAddress, 40)) {
			std::clog << "posix::ServerSocket: New connection from " << clientHostname << " (" << clientIPAddress << ")." << std::endl;
		}
		else {
			std::clog << "posix::ServerSocket: New connection accepted." << std::endl;
		}
		delete[] clientHostname;
		clientHostname = NULL;
		delete[] clientIPAddress;
		clientIPAddress = NULL;
		return acceptedSocket;
	}
	else {
		return 0;
	}
}

ServerMultiSocket::ServerMultiSocket(int _port, int _family, int _protocol, bool _blocking) throw(std::runtime_error) : ServerSocket(_port, _family, SOCK_STREAM, _protocol, _blocking) {
}

ServerMultiSocket::~ServerMultiSocket() {
	for(const_iterator it = clientSockets.begin(); it != clientSockets.end(); it++) {
		if(close(*it) == -1) {
			std::cerr << "posix::~ServerSocket(): close() failed." << std::endl;
			std::cerr << "\t" << strerror(errno) << std::endl;
		}
	}
	clientSockets.clear();
}

std::list< SocketResult * > ServerMultiSocket::process() throw(std::runtime_error) {
	std::list< SocketResult * > result;
	fd_set socketSet;
	int maxSocket = 0;
	FD_ZERO(&socketSet);
	FD_SET(socket, &socketSet); // Add server socket
	maxSocket = socket;
	for(const_iterator it = clientSockets.begin(); it != clientSockets.end(); it++) {
		FD_SET(*it, &socketSet);
		if(*it > maxSocket) {
			maxSocket = *it;
		}
	}
	if(isBlocking()) {
		if(select(maxSocket + 1, &socketSet, NULL, NULL, NULL) == -1) {
			throw std::runtime_error(strerror(errno));
		}
	}
	else {
		struct timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 0;
		if(select(maxSocket + 1, &socketSet, NULL, NULL, &timeout) == -1) {
			throw std::runtime_error(strerror(errno));
		}
	}
	if(FD_ISSET(socket, &socketSet)) {
		clientSockets.insert(accept(socket));
	}
	for(iterator it = clientSockets.begin(); it != clientSockets.end();) {
		if(FD_ISSET(*it, &socketSet)) {
			try {
				std::string reiceivedData = receive(*it);
				if(reiceivedData.size() > 0) {
					SocketByteString *socketResult = new SocketByteString;
					socketResult->socket = *it;
					socketResult->data = reiceivedData;
					result.push_back(socketResult);
					++it;
				}
				else {
					SocketClosed *socketResult = new SocketClosed;
					socketResult->socket = *it;
					result.push_back(socketResult);
					shutdownClient(it);
					it = clientSockets.begin(); // reset iterator since it may be invalid
				}
			}
			catch(std::runtime_error e) {
				std::cerr << "ibrc::ServerMultiSocket: Error receiving from socket " << *it << "." << std::endl;
				std::cerr << "\t " << e.what() << std::endl;
				SocketClosed *socketResult = new SocketClosed;
				socketResult->socket = *it;
				result.push_back(socketResult);
				shutdownClient(it);
				it = clientSockets.begin(); // reset iterator since it may be invalid
			}
		}
		else {
			++it;
		}
	}
	return result;
}

void ServerMultiSocket::send(int _client, std::string const &_data) throw(std::runtime_error) {
	Socket::send(_client, _data);
}

/*
 *** NO LONGER NEEDED ***
 *** BUG WAS IN Socket::receive() ***
// Own implementation of receive() without using select() since process() called it already.
// This fixes some nasty bug where two subsequent select() calls seem to invalidate this descriptor.
std::string ServerMultiSocket::receive(int _client) throw(std::runtime_error) {
	char *buf = new char[BUFFER_SIZE];
	ssize_t bytesReceived = recv(_client, buf, BUFFER_SIZE, 0);
	if(bytesReceived > 0) {
		std::string data(buf, bytesReceived);
		delete[] buf;
		return data;
	}
	else {
		delete[] buf;
		throw std::runtime_error(strerror(errno));
	}
}
*/

std::string ServerMultiSocket::receive(int _client) throw(std::runtime_error) {
	return Socket::receive(_client);
}

void ServerMultiSocket::sendAll(std::string const &_data) throw(std::runtime_error) {
	for(const_iterator it = clientSockets.begin(); it != clientSockets.end(); it++) {
		send(*it, _data);
	}
}

void ServerMultiSocket::sendAllExcept(std::string const &_data, std::set< int > _exceptClients) throw(std::runtime_error) {
	for(const_iterator it = clientSockets.begin(); it != clientSockets.end(); it++) {
		if(_exceptClients.find(*it) == _exceptClients.end()) {
			send(*it, _data);
		}
	}
}

void ServerMultiSocket::sendAllExcept(std::string const &_data, int _exceptClient) throw(std::runtime_error) {
	std::set< int > exceptClients;
	exceptClients.insert(_exceptClient);
	sendAllExcept(_data, exceptClients);
}

void ServerMultiSocket::shutdownClient(iterator _itSocket) {
	char *clientHostname = new char[512];
	char *clientIPAddress = new char[40]; // IPv6 address + \0
	if(resolveSocketConnection(*_itSocket, clientHostname, 512, clientIPAddress, 40)) {
		std::clog << "posix::ServerMultiSocket: Connection to " << clientHostname << " (" << clientIPAddress << ") closed." << std::endl;
	}
	else {
		std::clog << "posix::ServerMultiSocket: Connection closed." << std::endl;
	}
	delete[] clientHostname;
	clientHostname = NULL;
	delete[] clientIPAddress;
	clientIPAddress = NULL;
	if(close(*_itSocket) == -1) {
		throw std::runtime_error(strerror(errno));
	}
	clientSockets.erase(_itSocket);
}

}

