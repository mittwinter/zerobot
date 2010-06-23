#include "message.hpp"

namespace IRC {

Prefix::Prefix(std::string const &_nick, std::string const &_user, std::string const &_host) : nick(_nick), user(_user), host(_host) {
}

RawMessage::RawMessage(std::string const &_command) : command(_command) {
}

RawMessage::RawMessage(std::string const &_command, std::vector< std::string > const &_parameters) : command(_command), parameters(_parameters) {
}

RawMessage::RawMessage(std::string const &_command, std::vector< std::string > const &_parameters, std::string const &_trailing) : command(_command), parameters(_parameters), trailing(_trailing) {
}

RawMessage::RawMessage(std::auto_ptr< Prefix > _prefix, std::string const &_command, std::vector< std::string > const &_parameters, std::string const &_trailing) : prefix(_prefix), command(_command), parameters(_parameters), trailing(_trailing) {
}

MessagePingPong::MessagePingPong(std::string _serverName) : serverName(_serverName) {
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
		_out << " :" << _message.getTrailing() << std::flush;
	}
	return _out;
}
