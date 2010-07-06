#ifndef DATABASE_SQLITE_HPP
#define DATABASE_SQLITE_HPP

#include <stdexcept>
#include <string>

#include <sqlite3.h>

namespace lib {

class DatabaseSQLite {
	public:
		DatabaseSQLite(std::string const &_filename = "");
		virtual ~DatabaseSQLite();
		
		void setFilename(std::string const &_filename) { filename = _filename; }
		std::string const &getFilename(std::string const &_filename) const { return filename; }

		void open() throw(std::runtime_error);
		void close() throw(std::runtime_error);

	protected:
		std::string filename;
		sqlite3 *connection;
};

}

#endif

