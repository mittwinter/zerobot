#include "database_sqlite.hpp"

namespace lib {

DatabaseSQLite::DatabaseSQLite(std::string const &_filename) {
	connection = NULL;
	filename = _filename;
}

DatabaseSQLite::~DatabaseSQLite() {
	close();
}

void DatabaseSQLite::open() throw(std::runtime_error) {
	if(connection == NULL) {
		if(sqlite3_open(filename.c_str(), &connection) != SQLITE_OK) {
			throw std::runtime_error(sqlite3_errmsg(connection));
		}
	} // else: this is just NOP
}

void DatabaseSQLite::close() throw(std::runtime_error) {
	if(connection != NULL) {
		if(sqlite3_close(connection) != SQLITE_OK) {
			throw std::runtime_error(sqlite3_errmsg(connection));
		}
		connection = NULL;
	}
}

}
