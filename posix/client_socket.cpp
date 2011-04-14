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

#include "client_socket.hpp"

namespace posix {

ClientSocket::ClientSocket(std::string const &_serverName, int _serverPort, int _family, int _type, int _protocol, bool _blocking) throw(std::runtime_error) : Socket(_family, _type, _protocol, _blocking) {
	// Setup data structures as needed for getaddrinfo():
	struct addrinfo hints;
	struct addrinfo *result = NULL;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = family;
	hints.ai_socktype = type;
	hints.ai_protocol = protocol;
	hints.ai_flags = 0;
	std::stringstream sstrPort;
	sstrPort << _serverPort;
	// Actually call getaddrinfo():
	int gaiError = getaddrinfo(_serverName.c_str(), sstrPort.str().c_str(), &hints, &result);
	if(gaiError != 0) {
		throw std::runtime_error(gai_strerror(gaiError));
	}
	// Iterate over all returned address infos and try to connect to each one,
	// until one is successfull:
	struct addrinfo *resultPointer = NULL;
	for(resultPointer = result; resultPointer != NULL; resultPointer = resultPointer->ai_next) {
		socket = ::socket(resultPointer->ai_family, resultPointer->ai_socktype, resultPointer->ai_protocol);
		if(socket == -1) { // attempt to open socket failed
			if(close(socket) == -1) {
				throw std::runtime_error(strerror(errno));
			}
			socket = 0;
			continue;
		}
		// Connect to given host and port on this socket:
		int connectResult = connect(socket, resultPointer->ai_addr, resultPointer->ai_addrlen);
		if(connectResult == 0) { // socket successfully connected
			std::cerr << "posix::ClientSocket: Successfully connected to " << _serverName << " on port " << _serverPort << "." << std::endl;
			break;
		}
		else { // show error message, TODO: remove after debugging
			std::cerr << "posix::ClientSocket: Failed to connect() to " << _serverName << " on port " << _serverPort << "." << std::endl;
			std::cerr << "\t" << strerror(errno) << std::endl;
			if(close(socket) == -1) {
				throw std::runtime_error(strerror(errno));
			}
			socket = 0;
		}
	}
	freeaddrinfo(result);
	result = NULL;
	if(resultPointer == NULL) {
		throw std::runtime_error("posix::ClientSocket: connect() on any socket to " + _serverName + " on port " + sstrPort.str() + " failed.");
	}
}

std::string ClientSocket::receive() throw(std::runtime_error, std::invalid_argument) {
	return receive(socket);
}

void ClientSocket::send(std::string const &_data) throw(std::runtime_error) {
	send(socket, _data);
}

}

