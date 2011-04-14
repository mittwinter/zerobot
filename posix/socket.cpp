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

#include <arpa/inet.h> // for inet_ntop() and ntohs()

#include <cstring>
#include <errno.h>
#include <iostream>
#include <sstream>

#include "socket.hpp"

namespace posix {

unsigned int const Socket::BUFFER_SIZE = 1024;

Socket::Socket(int _family, int _type, int _protocol, bool _blocking) : family(_family), type(_type), protocol(_protocol), socket(0) {
	setBlocking(_blocking);
}

Socket::~Socket() {
	if(socket > 0) {
		if(close(socket) == -1) {
			std::cerr << "posix::~Socket(): close() failed." << std::endl;
			std::cerr << "\t" << strerror(errno) << std::endl;
		}
		socket = 0;
	}
}

void Socket::setBlocking(bool _blocking) {
	blocking = _blocking;
}

std::string Socket::receive(int _socket) throw(std::runtime_error, std::invalid_argument) {
	fd_set socketSet;
	FD_ZERO(&socketSet);
	int maxSocket = _socket;
	FD_SET(_socket, &socketSet);
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
	if(FD_ISSET(_socket, &socketSet)) {
		char *buf = new char[BUFFER_SIZE];
		errno = 0; // explicitly set errno to zero to check for possible errors
		ssize_t bytesReceived = recv(_socket, buf, BUFFER_SIZE, 0);
		if(bytesReceived > 0) {
			std::string data(buf, bytesReceived);
			delete[] buf;
			return data;
		}
		else {
			if(!isBlocking() && errno == 0) {
				delete[] buf;
				std::stringstream sstrSocket;
				sstrSocket << _socket;
				throw std::invalid_argument("posix::Socket: Activity on socket " + sstrSocket.str() + " indicated by select(), but recv() returned zero bytes, assuming socket closed.");
			}
			else {
				delete[] buf;
				throw std::runtime_error(strerror(errno));
			}
		}
	}
	else {
		return "";
	}
}

std::string Socket::receiveFrom(int _socket, std::string &_srcIP, int &_srcPort) throw(std::runtime_error) {
	fd_set socketSet;
	FD_ZERO(&socketSet);
	int maxSocket = _socket;
	FD_SET(_socket, &socketSet);
	if(!isBlocking()) {
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
	if(FD_ISSET(_socket, &socketSet)) {
		char *buf = new char[BUFFER_SIZE];
		struct sockaddr_storage addrStorage;
		socklen_t addrLen = sizeof(struct sockaddr_storage);
		ssize_t bytesReceived = recvfrom(_socket, buf, BUFFER_SIZE, 0, reinterpret_cast< struct sockaddr * >(&addrStorage), &addrLen);
		if(bytesReceived > 0) {
			std::string data(buf, bytesReceived);
			char srcIP[INET6_ADDRSTRLEN];
			if(addrStorage.ss_family == AF_INET6) {
				if(inet_ntop(addrStorage.ss_family, (&(reinterpret_cast< struct sockaddr_in6 * >(&addrStorage))->sin6_addr), srcIP, sizeof(srcIP)) != NULL) {
					_srcIP = srcIP;
				}
				else {
					std::cerr << "posix::Socket::receiveFrom: Error retrieving source information." << std::endl;
					std::cerr << "\t" << strerror(errno) << std::endl;
				}
			}
			else if(addrStorage.ss_family == AF_INET) {
				if(inet_ntop(addrStorage.ss_family, (&(reinterpret_cast< struct sockaddr_in * >(&addrStorage))->sin_addr), srcIP, sizeof(srcIP)) != NULL) {
					_srcIP = srcIP;
				}
				else {
					std::cerr << "posix::Socket::receiveFrom: Error retrieving source information." << std::endl;
					std::cerr << "\t" << strerror(errno) << std::endl;
				}
			}
			_srcPort = ntohs((addrStorage.ss_family == AF_INET6 ? (reinterpret_cast< struct sockaddr_in6 *>(&addrStorage))->sin6_port : (reinterpret_cast< struct sockaddr_in * >(&addrStorage))->sin_port));
			delete[] buf;
			return data;
		}
		else {
			delete[] buf;
			throw std::runtime_error(strerror(errno));
		}
	}
	else {
		return "";
	}
}

void Socket::send(int _socket, std::string const &_data) throw(std::runtime_error) {
	if(::send(_socket, _data.c_str(), _data.length(), 0) == -1) {
		throw std::runtime_error(strerror(errno));
	}
}

void Socket::sendTo(int _socket, std::string const &_data, std::string const &_destination, int _destinationPort) throw(std::runtime_error) {
	struct addrinfo hints;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = family;
	hints.ai_socktype = type;
	hints.ai_protocol = protocol;
	hints.ai_flags = 0;
	std::stringstream sstrPort;
	sstrPort << _destinationPort;
	struct addrinfo *result = NULL;
	int gaiError = getaddrinfo(_destination.c_str(), sstrPort.str().c_str(), &hints, &result);
	if(gaiError != 0) {
		throw std::runtime_error(std::string("posix::Socket: getaddrinfo() failed.\n\t") + gai_strerror(gaiError));
	}
	if(result != NULL) {
		if(sendto(socket, _data.c_str(), _data.size(), 0, result->ai_addr, result->ai_addrlen) != -1) {
			freeaddrinfo(result);
		}
		else {
			freeaddrinfo(result);
			throw std::runtime_error(std::string("posix::Socket: sendto() failed.\n\t") + strerror(errno));
		}
	}
	else {
		throw std::runtime_error("posix::Socket: getaddrinfo() returned nothing, sendTo() failed.");
	}
}

bool resolveSocketConnection(int _socket, char *_hostname, size_t _hostnameSize, char *_ipAddress, size_t _ipAddressSize) {
	strcpy(_hostname, "");
	strcpy(_ipAddress, "");
	struct sockaddr_in6 address;
	socklen_t addressLength = sizeof(address);
	if(getpeername(_socket, reinterpret_cast< struct sockaddr * > (&address), &addressLength) == -1) {
		std::cerr << "posix::resolveSocketConnection(): getpeername() failed." << std::endl;
		std::cerr << "\t" << strerror(errno) << std::endl;
		return false;
	}
	int getnameinfoResult = getnameinfo(reinterpret_cast< struct sockaddr const * > (&address), addressLength, _hostname, _hostnameSize, NULL, 0, 0);
	if(getnameinfoResult != 0) {
		std::cerr << "posix::resolveSocketConnection(): Error retrieving client information via getnameinfo()." << std::endl;
		std::cerr << "\t" << gai_strerror(getnameinfoResult) << std::endl;
	}
	int getnameinfoResultIP = getnameinfo(reinterpret_cast< struct sockaddr const * > (&address), addressLength, _ipAddress, _ipAddressSize, NULL, 0, NI_NUMERICHOST);
	if(getnameinfoResultIP != 0) {
		std::cerr << "posix::resolveSocketConnection(): Error retrieving client information via getnameinfo()." << std::endl;
		std::cerr << "\t" << gai_strerror(getnameinfoResult) << std::endl;
	}
	return (getnameinfoResult == 0 && getnameinfoResultIP == 0);
}

bool resolveSocketConnection(int _socket, std::string &_hostname, std::string &_ipAddress) {
	char hostname[512];
	char ipAddress[INET6_ADDRSTRLEN];
	bool success = resolveSocketConnection(_socket, hostname, 512, ipAddress, INET6_ADDRSTRLEN);
	_hostname = hostname;
	_ipAddress = ipAddress;
	return success;
}

}

