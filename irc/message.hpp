#ifndef IRCMESSAGE_HPP
#define IRCMESSAGE_HPP

#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace IRC {

class Message {
	public:
		Message() {}
		virtual ~Message() {}

	protected:
};

class Prefix {
	public:
		Prefix(std::string const &_nick, std::string const &_user = "", std::string const &_host = "");
		virtual ~Prefix() {}

		std::string const &getNick() const { return nick; }
		std::string const &getUser() const { return user; }
		std::string const &getHost() const { return host; }

	protected:
		std::string nick;
		std::string user;
		std::string host;
};

class RawMessage : public Message {
	public:
		RawMessage(std::string const &_command);
		RawMessage(std::string const &_command, std::vector< std::string > const &_parameters);
		RawMessage(std::string const &_command, std::vector< std::string > const &_parameters, std::string const &_trailing);
		RawMessage(std::auto_ptr< Prefix > _prefix, std::string const &_command, std::vector< std::string > const &_parameters, std::string const &_trailing);
		virtual ~RawMessage() {}

		Prefix const *getPrefix() const { return prefix.get(); }
		std::string const &getCommand() const { return command; }
		std::vector< std::string > const &getParamaters() const { return parameters; }
		std::string const &getTrailing() const { return trailing; }

	protected:
		std::auto_ptr< Prefix > prefix;
		std::string command;
		std::vector< std::string > parameters;
		std::string trailing;
};

class MessagePingPong : public Message {
	public:
		MessagePingPong(std::string _serverName);
		virtual ~MessagePingPong() {}

		std::string const &getServerName() const { return serverName; }

	protected:
		std::string serverName;
};

class MessagePing : public MessagePingPong {
	public:
		MessagePing(std::string _serverName) : MessagePingPong(_serverName) {}
		virtual ~MessagePing() {}

		operator RawMessage() const {
			std::vector< std::string > parameters;
			parameters.push_back(getServerName());
			//return RawMessage("PING", parameters); // TODO: Fix this!
			RawMessage tmp("PING", parameters);
			return tmp;
		}

	protected:
};

class MessagePong : public MessagePingPong {
	public:
		MessagePong(std::string _serverName) : MessagePingPong(_serverName) {}
		virtual ~MessagePong() {}

		operator RawMessage() const {
			std::vector< std::string > parameters;
			parameters.push_back(getServerName());
			//return RawMessage("PONG", parameters); // TODO: Fix this!
			RawMessage tmp("PONG", parameters);
			return tmp;
		}

	protected:
};

}

std::ostream &operator<<(std::ostream &_out, IRC::Prefix const &_prefix);
std::ostream &operator<<(std::ostream &_out, IRC::RawMessage const &_message);

#endif

