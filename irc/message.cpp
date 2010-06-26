#include "message.hpp"

namespace IRC {

Prefix::Prefix(std::string const &_nick, std::string const &_user, std::string const &_host) {
	nick = _nick;
	user = _user;
	host = _host;
}

RawMessage::RawMessage(std::string const &_command) : prefix(NULL) {
	command = _command;
}

RawMessage::RawMessage(std::string const &_command, std::vector< std::string > const &_parameters) : prefix(NULL) {
	command = _command;
	parameters = _parameters;
}

RawMessage::RawMessage(std::string const &_command, std::vector< std::string > const &_parameters, std::string const &_trailing) : prefix(NULL) {
	command = _command;
	parameters = _parameters;
	trailing = _trailing;
}

RawMessage::RawMessage(std::auto_ptr< Prefix > _prefix, std::string const &_command, std::vector< std::string > const &_parameters, std::string const &_trailing) : prefix(_prefix) {
	command = _command;
	parameters = _parameters;
	trailing = _trailing;
}

MessagePingPong::MessagePingPong(std::string const &_serverName) : serverName(_serverName) {
}

MessagePing::operator RawMessage() const {
	std::vector< std::string > parameters;
	parameters.push_back(getServerName());
	// return RawMessage("PING", parameters); // TODO: Fix this C++ grammar mess?
	RawMessage tmp("PING", parameters);
	return tmp;
}

MessagePong::operator RawMessage() const {
	std::vector< std::string > parameters;
	parameters.push_back(getServerName());
	// return RawMessage("PONG", parameters); // TODO: Fix this C++ grammar mess?
	RawMessage tmp("PONG", parameters);
	return tmp;
}

MessageNick::MessageNick(std::string const &_nickname) {
	nickname = _nickname;
}

MessageNick::operator RawMessage() const {
	std::vector< std::string > parameters;
	parameters.push_back(getNickname());
	RawMessage tmp("NICK", parameters);
	return tmp;
}

MessageUser::MessageUser(std::string const &_nickname, std::string const &_hostname, std::string const &_serverName, std::string const &_realName) {
	nickname = _nickname;
	hostname = _hostname;
	serverName = _serverName;
	realName = _realName;
}

MessageUser::operator RawMessage() const {
	std::vector< std::string > parameters;
	parameters.push_back(getNickname());
	parameters.push_back(getHostname());
	parameters.push_back(getServerName());
	RawMessage tmp("USER", parameters, getRealName());
	return tmp;
}

}

std::ostream &operator<<(std::ostream &_out, IRC::Prefix const &_prefix) {
	if(_prefix.getNick().size() > 0) {
		_out << ":" << _prefix.getNick() << std::flush;
		if(_prefix.getUser().size() > 0) {
			_out << "!" << _prefix.getUser() << std::flush;
		}
		if(_prefix.getHost().size() > 0) {
			_out << "@" << _prefix.getHost() << std::flush;
		}
	}
	else {
		_out << ":" << _prefix.getHost() << std::flush;
	}
	return _out;
}

std::ostream &operator<<(std::ostream &_out, IRC::RawMessage const &_message) {
	if(_message.getPrefix() != NULL) {
		_out << *(_message.getPrefix()) << " " << std::flush;
	}
	_out << _message.getCommand() << std::flush;
	for(std::vector< std::string >::const_iterator it = _message.getParamaters().begin(); it != _message.getParamaters().end(); it++) {
		_out << " " << *it << std::flush;
	}
	if(_message.getTrailing().size() > 0) {
		_out << " :" << _message.getTrailing();
	}
	_out << "\r\n" << std::flush;
	return _out;
}

