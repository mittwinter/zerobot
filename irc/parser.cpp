#include <cctype>
#include <iostream> // TODO: remove after debugging

#include "parser.hpp"

namespace IRC {

std::string const RawParser::special = "-[]\\`^{}|_"; // '|_' not allowed per RFC1459

RawParser::RawParser(bool _debug) : debug(_debug) {
}

std::auto_ptr< RawMessage > RawParser::parseString(std::string _message) const throw(std::runtime_error) {
	if(debug) {
		std::cerr << "RawParser: Parsing message '" << _message << "'" << std::endl;
	}
	std::auto_ptr< Prefix > prefix = parsePrefix(_message);
	skipSpace(_message);
	std::string command = parseCommand(_message);
	skipSpace(_message);
	std::vector< std::string > parameters = parseParameters(_message);
	skipSpace(_message);
	std::string trailing = parseTrailing(_message);
	return std::auto_ptr< RawMessage >(new RawMessage(prefix, command, parameters, trailing));
}

std::auto_ptr< Prefix > RawParser::parsePrefix(std::string &_message) const throw(std::runtime_error) {
	if(_message.size() >= 1 && _message.at(0) == ':') {
		if(debug) {
			std::cerr << "RawParser: Parsing prefix of message: '" << _message << "'" << std::endl;
		}
		_message.erase(0, 1); // remove colon
		std::string nick, user, host;
		// Determine whether there is a '!' or '@' or no '.' indicating a nick prefix, otherwise parse servername:
		if(_message.substr(0, _message.find_first_of(' ')).find_first_of("!@") != std::string::npos
				|| _message.substr(0, _message.find_first_of(' ')).find_first_of(".") == std::string::npos) {
			nick = parseNick(_message);
			user = parseUser(_message);
			host = parseHost(_message);
		}
		else {
			host = parseHost(_message);
		}
		if(debug) {
			std::cerr << "RawParser: Prefix is..." << std::endl
			          << "\t ...nick: " << nick << std::endl
			          << "\t ...user: " << user << std::endl
			          << "\t ...host: " << host << std::endl;
		}
		return std::auto_ptr< Prefix >(new Prefix(nick, user, host));
	}
	else {
		return std::auto_ptr< Prefix >(NULL);
	}
}

std::string RawParser::parseNick(std::string &_message) const throw(std::runtime_error) {
	std::string nick;
	while(_message.size() > 0 && !isspace(_message.at(0)) && _message.at(0) != '!' && _message.at(0) != '@') {
		if((nick.size() == 0 && isalpha(_message.at(0))) || isalnum(_message.at(0)) || special.find(_message.at(0)) != std::string::npos) {
			nick+=_message.at(0);
			_message.erase(0, 1);
		}
		else {
			throw std::runtime_error("RawParser: Found illegal char '" + _message.substr(0, 1) + "' in nick.");
		}
	}
	if(nick.size() > 0) {
		return nick;
	}
	else {
		throw std::runtime_error("RawParser: Empty nick in prefix not allowed.");
	}
}

std::string RawParser::parseUser(std::string &_message) const throw(std::runtime_error) {
	if(_message.at(0) == '!') {
		_message.erase(0, 1); // remove '!'
		std::string user;
		while(_message.size() > 0 && !isspace(_message.at(0)) && _message.at(0) != '@') {
			user+=_message.at(0);
			_message.erase(0, 1);
		}
		if(user.size() > 0) {
			return user;
		}
		else {
			throw std::runtime_error("RawParser: Empty user in prefix after '!' not allowed.");
		}
	}
	else {
		return "";
	}
}

std::string RawParser::parseHost(std::string &_message) const throw(std::runtime_error) {
	if(_message.at(0) == '@') {
		_message.erase(0, 1); // remove '@'
	}
	std::string host;
	while(_message.size() > 0 && !isspace(_message.at(0))) {
		host+=_message.at(0);
		_message.erase(0, 1);
	}
	return host;
}

std::string RawParser::parseCommand(std::string &_message) const throw(std::runtime_error) {
	if(_message.size() > 0) {
		std::string command;
		int (*checkFunc)(int) = NULL;
		if(isalpha(_message.at(0))) {
			checkFunc = &isalpha;
			if(debug) {
				std::cerr << "RawParser: Alphabetic command found." << std::endl;
			}
		}
		else if(isdigit(_message.at(0))) {
			checkFunc = &isdigit;
			if(debug) {
				std::cerr << "RawParser: Numeric command found." << std::endl;
			}
		}
		while(_message.size() > 0 && !isspace(_message.at(0))) {
			if(checkFunc != NULL && (*checkFunc)(_message.at(0))) {
				command+=_message.at(0);
				_message.erase(0, 1);
			}
			else {
				throw std::runtime_error("RawParser: Char '" + _message.substr(0, 1) + "' not allowed in command.");
			}
		}
		if(command.size() == 0) {
			throw std::runtime_error("RawParser: No command found in message.");
		}
		else if(isdigit(command.at(0)) && command.size() != 3) {
			throw std::runtime_error("RawParser: Invalid command '" + command + "' found.");
		}
		else {
			if(debug) {
				std::cerr << "RawParser: Command '" << command << "' found." << std::endl;
			}
			return command;
		}
	}
	else {
		throw std::runtime_error("RawParser: No command found in message.");
	}
}

std::vector< std::string > RawParser::parseParameters(std::string &_message) const {
	std::vector< std::string > parameters;
	std::string parameter;
	while(_message.size() > 0 && (parameter.size() > 0 || _message.at(0) != ':')) {
		if(isspace(_message.at(0))) {
			if(parameter.size() > 0) {
				parameters.push_back(parameter);
				parameter.clear();
			}
		}
		else {
			parameter+=_message.at(0);
		}
		_message.erase(0, 1);
	}
	if(parameters.size() > 0) {
		if(debug) {
			std::cerr << "RawParser: Parsed parameters..." << std::endl;
			for(std::vector< std::string >::const_iterator it = parameters.begin(); it != parameters.end(); it++) {
				std::cerr << "\t - " << *it << std::endl;
			}
		}
	}
	return parameters;
}

std::string RawParser::parseTrailing(std::string &_message) const {
	if(_message.size() > 0 && _message.at(0) == ':') {
		std::string trailing = _message.substr(1);
		_message.clear();
		if(debug) {
			std::cerr << "RawParser: Trailing is '" << trailing << "'." << std::endl;
		}
		return trailing;
	}
	else {
		return "";
	}
}

void RawParser::skipSpace(std::string &_str) const {
	while(_str.size() > 0 && isspace(_str.at(0))) {
		_str.erase(0, 1);
	}
}

Parser::Parser(bool _debug) : rawParser(new RawParser(_debug)) {
}

std::auto_ptr< Message > Parser::parseMessage(std::string _message) const throw(std::runtime_error) {
	std::auto_ptr< RawMessage > rawMessage = rawParser->parseString(_message);
	std::auto_ptr< Message > message(NULL);
	if(rawMessage->getCommand() == "PING") {
		try {
			message = std::auto_ptr< Message >(new MessagePing(rawMessage->getParamaters().at(0)));
		}
		catch(std::out_of_range e) {
		}
	}
	else if(rawMessage->getCommand() == "PONG") {
		try {
			message = std::auto_ptr< Message >(new MessagePong(rawMessage->getParamaters().at(0)));
		}
		catch(std::out_of_range e) {
		}
	}
	else {
		throw std::runtime_error("Parser: Message with command '" + rawMessage->getCommand() + "' not handled at the moment.");
	}
	return message;
}

}
