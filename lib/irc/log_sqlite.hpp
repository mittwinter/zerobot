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

		virtual void logMessage(Message const &_message);

	protected:
		std::string filename;
		sqlite3 *connection;

		virtual void createSchema();
};

}

#endif

