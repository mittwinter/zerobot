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

#include <iostream> // TODO: remove after debugging
#include <typeinfo>

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
			throw std::runtime_error(std::string("SQLite error: ") + sqlite3_errmsg(connection));
		}
		createSchema();
	} // else: this is just NOP
}

void LogSQLite::close() throw(std::runtime_error) {
	if(connection != NULL) {
		if(sqlite3_close(connection) != SQLITE_OK) {
			throw std::runtime_error(std::string("SQLite error: ") + sqlite3_errmsg(connection));
		}
		connection = NULL;
	}
}

void LogSQLite::logNamesList(std::string const &_channelName, std::string const &_namesListStr) {
	sqlite3_stmt *stmtInsert = NULL;
	std::cerr << "LogSQLite: Logging RPL_NAMREPLY message to table log_names." << std::endl;
	if(sqlite3_prepare_v2(connection, "INSERT INTO log_names(channel, namesList) VALUES($channel, $namesList)", -1, &stmtInsert, NULL) != SQLITE_OK) {
		throw std::runtime_error(std::string("SQLite error: ") + sqlite3_errmsg(connection));
	}
	if(sqlite3_bind_text(stmtInsert, sqlite3_bind_parameter_index(stmtInsert, "$channel"), _channelName.c_str(), _channelName.size(), SQLITE_TRANSIENT) != SQLITE_OK) {
		sqlite3_finalize(stmtInsert);
		throw std::runtime_error(std::string("SQLite error: ") + sqlite3_errmsg(connection));
	}
	if(sqlite3_bind_text(stmtInsert, sqlite3_bind_parameter_index(stmtInsert, "$namesList"), _namesListStr.c_str(), _namesListStr.size(), SQLITE_TRANSIENT) != SQLITE_OK) {
		sqlite3_finalize(stmtInsert);
		throw std::runtime_error(std::string("SQLite error: ") + sqlite3_errmsg(connection));
	}
	if(sqlite3_step(stmtInsert) != SQLITE_DONE) {
		sqlite3_finalize(stmtInsert);
		throw std::runtime_error(std::string("SQLite error: ") + sqlite3_errmsg(connection));
	}
	sqlite3_finalize(stmtInsert);
}

void LogSQLite::logNick(std::string const &_channelName, std::string const &_oldNickname, std::string const &_newNickname) {
	sqlite3_stmt *stmtInsert = NULL;
	std::cerr << "LogSQLite: Logging NICK message to table log_nick." << std::endl;
	if(sqlite3_prepare_v2(connection, "INSERT INTO log_nick(channel, oldNick, newNick) VALUES($channel, $oldNick, $newNick)", -1, &stmtInsert, NULL) != SQLITE_OK) {
		throw std::runtime_error(std::string("SQLite error: ") + sqlite3_errmsg(connection));
	}
	if(sqlite3_bind_text(stmtInsert, sqlite3_bind_parameter_index(stmtInsert, "$channel"), _channelName.c_str(), _channelName.size(), SQLITE_TRANSIENT) != SQLITE_OK) {
		sqlite3_finalize(stmtInsert);
		throw std::runtime_error(std::string("SQLite error: ") + sqlite3_errmsg(connection));
	}
	if(sqlite3_bind_text(stmtInsert, sqlite3_bind_parameter_index(stmtInsert, "$oldNick"), _oldNickname.c_str(), _oldNickname.size(), SQLITE_TRANSIENT) != SQLITE_OK) {
		sqlite3_finalize(stmtInsert);
		throw std::runtime_error(std::string("SQLite error: ") + sqlite3_errmsg(connection));
	}
	if(sqlite3_bind_text(stmtInsert, sqlite3_bind_parameter_index(stmtInsert, "$newNick"), _newNickname.c_str(), _newNickname.size(), SQLITE_TRANSIENT) != SQLITE_OK) {
		sqlite3_finalize(stmtInsert);
		throw std::runtime_error(std::string("SQLite error: ") + sqlite3_errmsg(connection));
	}
	if(sqlite3_step(stmtInsert) != SQLITE_DONE) {
		sqlite3_finalize(stmtInsert);
		throw std::runtime_error(std::string("SQLite error: ") + sqlite3_errmsg(connection));
	}
	sqlite3_finalize(stmtInsert);
}

void LogSQLite::logQuit(std::string const &_channelName, std::string const &_nickname, std::string const &_quitMessage) {
	sqlite3_stmt *stmtInsert = NULL;
	std::cerr << "LogSQLite: Logging QUIT/PART message to table log_quit." << std::endl;
	if(sqlite3_prepare_v2(connection, "INSERT INTO log_quit(channel, nick, quitMessage) VALUES($channel, $nick, $quitMessage)", -1, &stmtInsert, NULL) != SQLITE_OK) {
		throw std::runtime_error(std::string("SQLite error: ") + sqlite3_errmsg(connection));
	}
	if(sqlite3_bind_text(stmtInsert, sqlite3_bind_parameter_index(stmtInsert, "$channel"), _channelName.c_str(), _channelName.size(), SQLITE_TRANSIENT) != SQLITE_OK) {
		sqlite3_finalize(stmtInsert);
		throw std::runtime_error(std::string("SQLite error: ") + sqlite3_errmsg(connection));
	}
	if(sqlite3_bind_text(stmtInsert, sqlite3_bind_parameter_index(stmtInsert, "$nick"), _nickname.c_str(), _nickname.size(), SQLITE_TRANSIENT) != SQLITE_OK) {
		sqlite3_finalize(stmtInsert);
		throw std::runtime_error(std::string("SQLite error: ") + sqlite3_errmsg(connection));
	}
	if(sqlite3_bind_text(stmtInsert, sqlite3_bind_parameter_index(stmtInsert, "$quitMessage"), _quitMessage.c_str(), _quitMessage.size(), SQLITE_TRANSIENT) != SQLITE_OK) {
		sqlite3_finalize(stmtInsert);
		throw std::runtime_error(std::string("SQLite error: ") + sqlite3_errmsg(connection));
	}
	if(sqlite3_step(stmtInsert) != SQLITE_DONE) {
		sqlite3_finalize(stmtInsert);
		throw std::runtime_error(std::string("SQLite error: ") + sqlite3_errmsg(connection));
	}
	sqlite3_finalize(stmtInsert);
}

void LogSQLite::logJoin(std::string const &_channelName, std::string const &_nickname) {
	sqlite3_stmt *stmtInsert = NULL;
	std::cerr << "LogSQLite: Logging JOIN message to table log_join." << std::endl;
	if(sqlite3_prepare_v2(connection, "INSERT INTO log_join(channel, nick) VALUES($channel, $nick)", -1, &stmtInsert, NULL) != SQLITE_OK) {
		throw std::runtime_error(std::string("SQLite error: ") + sqlite3_errmsg(connection));
	}
	if(sqlite3_bind_text(stmtInsert, sqlite3_bind_parameter_index(stmtInsert, "$channel"), _channelName.c_str(), _channelName.size(), SQLITE_TRANSIENT) != SQLITE_OK) {
		sqlite3_finalize(stmtInsert);
		throw std::runtime_error(std::string("SQLite error: ") + sqlite3_errmsg(connection));
	}
	if(sqlite3_bind_text(stmtInsert, sqlite3_bind_parameter_index(stmtInsert, "$nick"), _nickname.c_str(), _nickname.size(), SQLITE_TRANSIENT) != SQLITE_OK) {
		sqlite3_finalize(stmtInsert);
		throw std::runtime_error(std::string("SQLite error: ") + sqlite3_errmsg(connection));
	}
	if(sqlite3_step(stmtInsert) != SQLITE_DONE) {
		sqlite3_finalize(stmtInsert);
		throw std::runtime_error(std::string("SQLite error: ") + sqlite3_errmsg(connection));
	}
	sqlite3_finalize(stmtInsert);
}

void LogSQLite::logPrivMsg(std::string const &_channelName, std::string const &_nickname, std::string const &_message) {
	sqlite3_stmt *stmtInsert = NULL;
	std::cerr << "LogSQLite: Logging PRIVMSG message to log_privmsg." << std::endl;
	if(sqlite3_prepare_v2(connection, "INSERT INTO log_privmsg(channel, nick, message) VALUES($channel, $nick, $message)", -1, &stmtInsert, NULL) != SQLITE_OK) {
		throw std::runtime_error(std::string("SQLite error: ") + sqlite3_errmsg(connection));
	}
	if(sqlite3_bind_text(stmtInsert, sqlite3_bind_parameter_index(stmtInsert, "$channel"), _channelName.c_str(), _channelName.size(), SQLITE_TRANSIENT) != SQLITE_OK) {
		sqlite3_finalize(stmtInsert);
		throw std::runtime_error(std::string("SQLite error: ") + sqlite3_errmsg(connection));
	}
	if(sqlite3_bind_text(stmtInsert, sqlite3_bind_parameter_index(stmtInsert, "$nick"), _nickname.c_str(), _nickname.size(), SQLITE_TRANSIENT) != SQLITE_OK) {
		sqlite3_finalize(stmtInsert);
		throw std::runtime_error(std::string("SQLite error: ") + sqlite3_errmsg(connection));
	}
	if(sqlite3_bind_text(stmtInsert, sqlite3_bind_parameter_index(stmtInsert, "$message"), _message.c_str(), _message.size(), SQLITE_TRANSIENT) != SQLITE_OK) {
		sqlite3_finalize(stmtInsert);
		throw std::runtime_error(std::string("SQLite error: ") + sqlite3_errmsg(connection));
	}
	if(sqlite3_step(stmtInsert) != SQLITE_DONE) {
		sqlite3_finalize(stmtInsert);
		throw std::runtime_error(std::string("SQLite error: ") + sqlite3_errmsg(connection));
	}
	sqlite3_finalize(stmtInsert);
}

void LogSQLite::createSchema() {
	static const char *schema = "\
		CREATE TABLE IF NOT EXISTS log_names \
			( timestamp INTEGER DEFAULT(strftime('%s', 'now')) \
			, channel TEXT NOT NULL \
			, namesList TEXT NOT NULL \
			); \
		CREATE TABLE IF NOT EXISTS log_nick \
			( timestamp INTEGER DEFAULT(strftime('%s', 'now')) \
			, channel TEXT NOT NULL \
			, oldNick TEXT NOT NULL \
			, newNick TEXT NOT NULL \
			); \
		CREATE TABLE IF NOT EXISTS log_quit \
			( timestamp INTEGER DEFAULT(strftime('%s', 'now')) \
			, channel TEXT NOT NULL \
			, nick TEXT NOT NULL \
			, quitMessage TEXT NOT NULL \
			); \
		CREATE TABLE IF NOT EXISTS log_join \
			( timestamp INTEGER DEFAULT(strftime('%s', 'now')) \
			, channel TEXT NOT NULL \
			, nick TEXT NOT NULL \
			); \
		CREATE TABLE IF NOT EXISTS log_privmsg \
			( timestamp INTEGER DEFAULT(strftime('%s', 'now')) \
			, channel TEXT NOT NULL \
			, nick TEXT NOT NULL \
			, message TEXT NOT NULL \
			); \
		";
	char *errorMessagesPtr = NULL;
	if(sqlite3_exec(connection, schema, NULL, NULL, &errorMessagesPtr) != SQLITE_OK) {
		std::string errorMessages(errorMessagesPtr);
		sqlite3_free(errorMessagesPtr);
		throw std::runtime_error("SQLite error: " + errorMessages);
	}
}

}
