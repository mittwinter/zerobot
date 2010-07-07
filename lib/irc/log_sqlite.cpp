#include "log_sqlite.hpp"

namespace IRC {

LogSQLite::LogSQLite(std::string const &_filename) {
	connection = NULL;
	filename = _filename;
}

LogSQLite::~LogSQLite() {
	close();
}

void LogSQLite::open() throw(std::runtime_error) {
	if(connection == NULL) {
		if(sqlite3_open(filename.c_str(), &connection) != SQLITE_OK) {
			throw std::runtime_error(sqlite3_errmsg(connection));
		}
		createSchema();
	} // else: this is just NOP
}

void LogSQLite::close() throw(std::runtime_error) {
	if(connection != NULL) {
		if(sqlite3_close(connection) != SQLITE_OK) {
			throw std::runtime_error(sqlite3_errmsg(connection));
		}
		connection = NULL;
	}
}

void LogSQLite::logMessage(Message const &_message) {
}

void LogSQLite::createSchema() {
	static const char *schema = "\
		CREATE TABLE IF NOT EXISTS log \
			( timestamp INTEGER DEFAULT(strftime('%s', 'now')) \
			, source TEXT \
			, target TEXT \
			, type TEXT \
			, message TEXT \
			) \
		";
	sqlite3_stmt *stmtSchema = NULL;
	if(sqlite3_prepare_v2(connection, schema, -1, &stmtSchema, NULL) != SQLITE_OK) {
		throw std::runtime_error(sqlite3_errmsg(connection));
	}
	if(sqlite3_step(stmtSchema) != SQLITE_DONE) {
		sqlite3_finalize(stmtSchema);
		throw std::runtime_error(sqlite3_errmsg(connection));
	}
	sqlite3_finalize(stmtSchema);
}

}

