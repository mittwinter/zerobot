#ifndef SOCKET_HPP
#define SOCKET_HPP

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

		std::string receive();
		void send(std::string const &_data);

	protected:
		static const unsigned int BUFFER_SIZE;
};

}

#endif

