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

#ifndef PLUGINS_LOG_HPP
#define PLUGINS_LOG_HPP

#include <list>
#include <string>

#include "../lib/irc/log_sqlite.hpp"
#include "base.hpp"

namespace zerobot {

class PlugInLog : public PlugIn {
	public:
		PlugInLog( int priority, std::string const &name, std::string const &channelName, std::string const &databaseFilename );
		virtual ~PlugInLog();

		virtual std::auto_ptr< PlugInResult > onConnect( state_t state );
		virtual std::auto_ptr< PlugInResult > onPacket( state_t state, IRC::Message const &message );
		virtual std::auto_ptr< PlugInResult > onPacketSent( state_t state, IRC::Message const &message );
		virtual std::auto_ptr< PlugInResult > onTimeTrigger( state_t state );
		virtual std::auto_ptr< PlugInResult > onDisconnect( state_t state );

		std::string const &getChannelName() const { return channelName; }

	protected:
		std::string channelName;
		std::list< std::string > channelUsers;
		IRC::LogSQLite logSQLite;

		void parseChannelUsers( std::string const &channelUsersStr );
		void addChannelUser( std::string const &user );
		void removeChannelUser( std::string const &user );
};

}

#endif
