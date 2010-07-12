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

#ifndef IRC_LOG_SQLITE_HPP
#define IRC_LOG_SQLITE_HPP

#include <stdexcept>
#include <string>

#include <sqlite3.h>

#include "log.hpp"

namespace IRC {

class LogSQLite : Log {
	public:
		LogSQLite(std::string const &_filename = "");
		virtual ~LogSQLite();
		
		void setFilename(std::string const &_filename) { filename = _filename; }
		std::string const &getFilename(std::string const &_filename) const { return filename; }

		void open() throw(std::runtime_error);
		void close() throw(std::runtime_error);

		virtual void logNamesList(std::string const &_channelName, std::string const &_namesListStr);
		virtual void logNick(std::string const &_channelName, std::string const &_oldNickname, std::string const &_newNickname);
		virtual void logQuit(std::string const &_channelName, std::string const &_nickname, std::string const &_quitMessage);
		virtual void logJoin(std::string const &_channelName, std::string const &_nickname);
		virtual void logPrivMsg(std::string const &_channelName, std::string const &_nickname, std::string const &_message);

	protected:
		std::string filename;
		sqlite3 *connection;

		virtual void createSchema();
};

}

#endif
