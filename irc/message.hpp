#ifndef IRCMESSAGE_HPP
#define IRCMESSAGE_HPP

#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace IRC {

typedef enum {
	ERR_NONICKNAMEGIVEN = 431,
	ERR_ERRONEUSNICKNAME = 432,
	ERR_NICKNAMEINUSE = 433,
	ERR_NICKCOLLISION = 436,
	ERR_ALREADYREGISTERED = 462,
} reply_code_t;

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

class RawMessage {
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

// TODO: Add prefix to these messages since it contains valuable information (e. g. in PRIVMSG or PING)
class Message {
	public:
		Message() {}
		virtual ~Message() {}

		virtual operator RawMessage() const = 0;

	protected:
};

class MessageNumericReply : public Message {
	public:
		MessageNumericReply(reply_code_t _replyCode);
		MessageNumericReply(reply_code_t _replyCode, std::vector< std::string > const &_parameters, std::string const &_trailing = "");
		virtual ~MessageNumericReply() {}

		virtual operator RawMessage() const;

		reply_code_t getReplyCode() const { return replyCode; }
		std::vector< std::string > const &getParamaters() const { return parameters; }
		std::string const &getTrailing() const { return trailing; }

	protected:
		reply_code_t replyCode;
		std::vector< std::string > parameters;
		std::string trailing;
};

class MessagePingPong : public Message {
	public:
		MessagePingPong(std::string const &_serverName);
		virtual ~MessagePingPong() {}

		std::string const &getServerName() const { return serverName; }

		virtual operator RawMessage() const = 0;

	protected:
		std::string serverName;
};

class MessagePing : public MessagePingPong {
	public:
		MessagePing(std::string const &_serverName) : MessagePingPong(_serverName) {}
		virtual ~MessagePing() {}

		virtual operator RawMessage() const;

	protected:
};

class MessagePong : public MessagePingPong {
	public:
		MessagePong(std::string const &_serverName) : MessagePingPong(_serverName) {}
		virtual ~MessagePong() {}

		virtual operator RawMessage() const;

	protected:
};

class MessageNick : public Message {
	public:
		MessageNick(std::string const &_nickname);
		virtual ~MessageNick() {}

		virtual operator RawMessage() const;

		std::string const &getNickname() const { return nickname; }

	protected:
		std::string nickname;
};

class MessageUser : public Message {
	public:
		MessageUser(std::string const &_nickname, std::string const &_hostname, std::string const &_serverName, std::string const &_realName);
		virtual ~MessageUser() {}

		virtual operator RawMessage() const;

		std::string const &getNickname() const { return nickname; }
		std::string const &getHostname() const { return hostname; }
		std::string const &getServerName() const { return serverName; }
		std::string const &getRealName() const { return realName; }

	protected:
		std::string nickname;
		std::string hostname;
		std::string serverName;
		std::string realName;
};

class MessageQuit : public Message {
	public:
		MessageQuit(std::string const &_quitMessage);
		virtual ~MessageQuit() {}

		virtual operator RawMessage() const;

		std::string const &getQuitMessage() const { return quitMessage; }

	protected:
		std::string quitMessage;
};

}

std::ostream &operator<<(std::ostream &_out, IRC::Prefix const &_prefix);
std::ostream &operator<<(std::ostream &_out, IRC::RawMessage const &_message);

#endif

