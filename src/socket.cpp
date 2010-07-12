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

#include <cstring>
#include <iostream>
#include <memory>
#include <sstream>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include "socket.hpp"

namespace zerobot {

Socket::Socket(int _domain, int _type, int _protocol) {
	domain = _domain;
	type = _type;
	protocol = _protocol;
	if(_domain != AF_UNSPEC) {
		socket = ::socket(_domain, _type, _protocol);
		if(socket == -1) {
			throw std::runtime_error(strerror(errno));
		}
	}
}

Socket::~Socket() {
	shutdown(socket, SHUT_RDWR);
	close(socket);
}

const unsigned int ServerSocket::BUFFER_SIZE = 1024;

ServerSocket::ServerSocket(int _port, int _domain, int _type, int _protocol) : Socket(_domain, _type, _protocol) {
	clientSocket = 0;
	// reuse socket that is in TIME_WAIT state:
	int reuse = 1;
	if(setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) == -1) {
		throw std::runtime_error(strerror(errno));
	}
	// force blocking socket:
	int socketFlags = fcntl(socket, F_GETFL, 0);
	fcntl(socket, F_SETFL, socketFlags&(~O_NONBLOCK));
	// initialize address to listen on any and given port:
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htons(INADDR_ANY);
	addr.sin_port = htons(_port);
	memset(&(addr.sin_zero), 0, 8);
	if(bind(socket, reinterpret_cast< struct sockaddr * > (&addr), sizeof(struct sockaddr_in)) == -1) {
		throw std::runtime_error(strerror(errno));
	}
	if(listen(socket, 8) == -1) { // listen on socket with a backlog of 8
		throw std::runtime_error(strerror(errno));
	}
}

ServerSocket::~ServerSocket() {
}

void ServerSocket::accept() {
	if(clientSocket != 0) {
		throw std::runtime_error("client connection already accepted");
	}
	struct sockaddr_in clientAddress;
	socklen_t clientAddressLen = sizeof(struct sockaddr_in);
	clientSocket = ::accept(socket, reinterpret_cast< struct sockaddr * > (&clientAddress), &clientAddressLen);
	if(clientSocket == -1) {
		throw std::runtime_error(strerror(errno));
	}
	char clientHostname[256];
	int gaiReturn = getnameinfo(reinterpret_cast< struct sockaddr * > (&clientAddress), clientAddressLen, clientHostname, 256, NULL, 0, 0);
	if(gaiReturn != 0) {
		std::cerr << "Error retrieving client info (" << gai_strerror(gaiReturn) << ")" << std::endl;
	}
	std::cerr << "New connection from " << clientHostname << " "
	          << "("
	          << ((ntohl(clientAddress.sin_addr.s_addr)&0xff000000)>>24)
	          << "."
	          << ((ntohl(clientAddress.sin_addr.s_addr)&0xff0000)>>16)
	          << "."
	          << ((ntohl(clientAddress.sin_addr.s_addr)&0xff00)>>8)
	          << "."
	          << (ntohl(clientAddress.sin_addr.s_addr)&0xff)
	          << ")"
	          << std::endl;
}

std::string ServerSocket::receive() {
	char buf[BUFFER_SIZE]; // TODO: Refactor to use heap instead of stack!
	ssize_t bytesReceived = 0;
	bytesReceived = recv(clientSocket, buf, BUFFER_SIZE, 0);
	if(bytesReceived == -1) {
		throw std::runtime_error(strerror(errno));
	}
	std::string data(buf, bytesReceived);
	return data;
}

void ServerSocket::send(const std::string &_data) {
	if(::send(clientSocket, _data.c_str(), _data.length(), 0) == -1) {
		throw std::runtime_error(strerror(errno));
	}
}

const unsigned int ClientSocket::BUFFER_SIZE = 1024;

ClientSocket::ClientSocket(std::string const &_serverName, int _serverPort, int _domain, int _type, int _protocol) : Socket(_domain, _type, _protocol) {
	// Setup data structures as needed for getaddrinfo():
	struct addrinfo hints;
	struct addrinfo *result, *resultPointer;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = _domain;
	hints.ai_socktype = _type;
	hints.ai_flags = 0;
	hints.ai_protocol = _protocol;
	std::stringstream sstrPort;
	sstrPort << _serverPort;
	// Actually call getaddrinfo():
	int gaiError = getaddrinfo(_serverName.c_str(), sstrPort.str().c_str(), &hints, &result);
	if(gaiError != 0) {
		throw std::runtime_error(gai_strerror(gaiError));
	}
	// Iterate over all returned address infos and try to connect to each one,
	// until one is successfull:
	for(resultPointer = result; resultPointer != NULL; resultPointer = resultPointer->ai_next) {
		socket = ::socket(resultPointer->ai_family, resultPointer->ai_socktype, resultPointer->ai_protocol);
		if(socket == -1) { // attempt to open socket failed
			continue;
		}
		int connectError = connect(socket, resultPointer->ai_addr, resultPointer->ai_addrlen);
		if(connectError == 0) { // socket successfully connected
			break;
		}
		else { // show error message, TODO: remove after debugging
			std::cerr << strerror(errno) << std::endl;
		}
		// something went wrong during connect(), clean up and try next one (if there is one, normal iteration):
		close(socket);
		socket = 0;
	}
	freeaddrinfo(result);
	if(resultPointer != NULL) {
		// force non-blocking socket:
		int socketFlags = fcntl(socket, F_GETFL, 0);
		//fcntl(socket, F_SETFL, socketFlags&(~O_NONBLOCK));
		fcntl(socket, F_SETFL, socketFlags|O_NONBLOCK);
		std::cerr << "ClientSocket: Successfully connected to " << _serverName << " on port " << _serverPort << "." << std::endl;
	}
	else {
		throw std::runtime_error("ClientSocket: connect() to " + _serverName + " on port " + sstrPort.str() + " failed.");
	}
}

std::string ClientSocket::receive() throw(std::runtime_error) {
	char *buf = new char[BUFFER_SIZE];
	ssize_t bytesReceived = 0;
	bytesReceived = recv(socket, buf, BUFFER_SIZE, 0);
	if(bytesReceived <= 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
		delete[] buf;
		throw std::runtime_error(strerror(errno));
	}
	if(bytesReceived > 0) {
		std::string data(buf, bytesReceived);
		delete[] buf;
		//std::cerr << "ClientSocket: Received '" << data << "'" << std::endl; // TODO: remove this debug output
		//std::cerr << "ClientSocket: bytesReceived: " << bytesReceived << " errno: " << errno << " (EAGAIN: " << EAGAIN << " EWOULDBLOCK: " << EWOULDBLOCK << ")" << std::endl; // TODO: remove this debug output
		return data;
	}
	else {
		delete[] buf;
		return "";
	}
}

void ClientSocket::send(std::string const &_data) throw(std::runtime_error) {
	std::cout << ">>> " << _data << std::flush; // TODO: remove this debug output
	if(::send(socket, _data.c_str(), _data.length(), 0) == -1) {
		throw std::runtime_error(strerror(errno));
	}
}

}

