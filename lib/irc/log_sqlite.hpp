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
