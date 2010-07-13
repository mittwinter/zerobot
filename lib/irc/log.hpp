/*	This file is part of zerobot.
	Copyright (C) 2010 Martin Wegner

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

#ifndef IRC_LOG_HPP
#define IRC_LOG_HPP

#include <string>

namespace IRC {

class Log {
	public:
		Log() {}
		virtual ~Log() {}

		virtual void logNamesList(std::string const &_channelName, std::string const &_namesListStr) = 0;
		virtual void logNick(std::string const &_channelName, std::string const &_oldNickname, std::string const &_newNickname) = 0;
		virtual void logQuit(std::string const &_channelName, std::string const &_nickname, std::string const &_quitMessage) = 0;
		virtual void logJoin(std::string const &_channelName, std::string const &_nickname) = 0;
		virtual void logPrivMsg(std::string const &_channelName, std::string const &_nickname, std::string const &_message) = 0;

	protected:
};

}

#endif

