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

#include <sstream>

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

Message::Message() : prefix(NULL) {
}

Message::Message(std::auto_ptr< Prefix > _prefix) : prefix(_prefix) {
}

MessageNumericReply::MessageNumericReply(reply_code_t _replyCode) {
	replyCode = _replyCode;
}

MessageNumericReply::MessageNumericReply(std::auto_ptr< Prefix > _prefix, reply_code_t _replyCode) : Message(_prefix) {
	replyCode = _replyCode;
}

MessageNumericReply::MessageNumericReply(reply_code_t _replyCode, std::vector< std::string > const &_parameters, std::string const &_trailing) : parameters(_parameters) {
	replyCode = _replyCode;
	trailing = _trailing;
}

MessageNumericReply::MessageNumericReply(std::auto_ptr< Prefix > _prefix, reply_code_t _replyCode, std::vector< std::string > const &_parameters, std::string const &_trailing) : Message(_prefix), parameters(_parameters) {
	replyCode = _replyCode;
	trailing = _trailing;
}

MessageNumericReply::operator RawMessage() const {
	std::stringstream sstrReplyCode;
	sstrReplyCode << getReplyCode();
	RawMessage tmp(sstrReplyCode.str(), getParamaters(), getTrailing());
	return tmp;
}

MessagePingPong::MessagePingPong(std::string const &_serverName) {
	serverName = _serverName;
}

MessagePingPong::MessagePingPong(std::auto_ptr< Prefix > _prefix, std::string const &_serverName) : Message(_prefix) {
	serverName = _serverName;
}

MessagePing::MessagePing(std::string const &_serverName) : MessagePingPong(_serverName) {
}

MessagePing::MessagePing(std::auto_ptr< Prefix > _prefix, std::string const &_serverName) : MessagePingPong(_prefix, _serverName) {
}

MessagePing::operator RawMessage() const {
	std::vector< std::string > parameters;
	parameters.push_back(getServerName());
	// return RawMessage("PING", parameters); // TODO: Fix this C++ grammar mess?
	RawMessage tmp("PING", parameters);
	return tmp;
}

MessagePong::MessagePong(std::string const &_serverName) : MessagePingPong(_serverName) {
}

MessagePong::MessagePong(std::auto_ptr< Prefix > _prefix, std::string const &_serverName) : MessagePingPong(_prefix, _serverName) {
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

MessageNick::MessageNick(std::auto_ptr< Prefix > _prefix, std::string const &_nickname) : Message(_prefix) {
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

MessageUser::MessageUser(std::auto_ptr< Prefix > _prefix, std::string const &_nickname, std::string const &_hostname, std::string const &_serverName, std::string const &_realName) : Message(_prefix) {
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

MessageQuit::MessageQuit(std::string const &_quitMessage) {
	quitMessage = _quitMessage;
}

MessageQuit::MessageQuit(std::auto_ptr< Prefix > _prefix, std::string const &_quitMessage) : Message(_prefix) {
	quitMessage = _quitMessage;
}

MessageQuit::operator RawMessage() const {
	std::vector< std::string > parameters;
	RawMessage tmp("QUIT", parameters, getQuitMessage());
	return tmp;
}

MessageJoin::MessageJoin(std::string const &_channelName) {
	channelName = _channelName;
}

MessageJoin::MessageJoin(std::auto_ptr< Prefix > _prefix, std::string const &_channelName) : Message(_prefix) {
	channelName = _channelName;
}

MessageJoin::operator RawMessage() const {
	std::vector< std::string > parameters;
	parameters.push_back(getChannelName());
	RawMessage tmp("JOIN", parameters);
	return tmp;
}

MessagePrivMsg::MessagePrivMsg (std::string const &_receiver, std::string const &_message) {
	receiver = _receiver;
	message = _message;
}

MessagePrivMsg::MessagePrivMsg (std::auto_ptr< Prefix > _prefix, std::string const &_receiver, std::string const &_message) : Message (_prefix) {
	receiver = _receiver;
	message = _message;
}

MessagePrivMsg::operator RawMessage() const {
	std::vector< std::string > parameters;
	parameters.push_back(getReceiver());
	RawMessage tmp("PRIVMSG", parameters, getMessage());
	return tmp;
}

MessagePart::MessagePart(std::string const &_channelName, std::string const &_partMessage) {
	channelName = _channelName;
	partMessage = _partMessage;
}

MessagePart::MessagePart(std::auto_ptr< Prefix > _prefix, std::string const &_channelName, std::string const &_partMessage ) : Message (_prefix) {
	channelName = _channelName;
	partMessage = _partMessage;
}

MessagePart::operator RawMessage() const {
	std::vector< std::string > parameters;
	parameters.push_back(getChannelName());
	RawMessage tmp("PART", parameters, getPartMessage());
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
	if(_message.getPrefixPtr() != NULL) {
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

