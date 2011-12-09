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

#include <algorithm>
#include <sstream>
#include <typeinfo>

#include "log.hpp"

namespace zerobot {

PlugInLog::PlugInLog( int priority, std::string const &name, std::string const &channelName, std::string const &databaseFilename )
		: PlugIn( priority, name )
		, channelName( channelName )
		, logSQLite( databaseFilename ) {
	logSQLite.open();
}

PlugInLog::~PlugInLog() {
	logSQLite.close();
}

std::auto_ptr< PlugInResult > PlugInLog::onConnect( state_t state ) {
	return std::auto_ptr< PlugInResult >( NULL );
}

std::auto_ptr< PlugInResult > PlugInLog::onPacket( state_t state, IRC::Message const &message ) {
	try {
		IRC::MessageNumericReply const &numericReply = dynamic_cast< IRC::MessageNumericReply const & >( message );
		try {
			// seems that the channel name can be at first or last position in parameter list "in the wild", so honour this:
			if( numericReply.getParamaters().at( 0 ) == channelName
					|| (numericReply.getParamaters().size() > 1 && numericReply.getParamaters().at( numericReply.getParamaters().size() - 1 ) == channelName)
					) {
				if( numericReply.getReplyCode() == IRC::RPL_NAMREPLY ) {
					logSQLite.logNamesList( channelName, numericReply.getTrailing() );
					parseChannelUsers( numericReply.getTrailing() );
				}
			}
		}
		catch(std::out_of_range) {}
	}
	catch(std::bad_cast) {}
	try {
		IRC::MessageNick const &nickMessage = dynamic_cast< IRC::MessageNick const & >( message );
		if( std::find( channelUsers.begin(), channelUsers.end(), nickMessage.getPrefix()->getNick() ) != channelUsers.end() ) {
			removeChannelUser( nickMessage.getPrefix()->getNick() );
			addChannelUser( nickMessage.getNickname() );
			logSQLite.logNick( channelName, nickMessage.getPrefix()->getNick(), nickMessage.getNickname() );
		}
	}
	catch(std::bad_cast) {}
	try {
		IRC::MessageQuit const &quitMessage = dynamic_cast< IRC::MessageQuit const & >( message );
		if( std::find( channelUsers.begin(), channelUsers.end(), quitMessage.getPrefix()->getNick() ) != channelUsers.end() ) {
			removeChannelUser( quitMessage.getPrefix()->getNick() );
			logSQLite.logQuit( channelName, quitMessage.getPrefix()->getNick(), quitMessage.getQuitMessage() );
		}
	}
	catch(std::bad_cast) {}
	try {
		IRC::MessageJoin const &joinMessage = dynamic_cast< IRC::MessageJoin const & >( message );
		if( joinMessage.getChannelName() == channelName && joinMessage.getPrefix() != NULL && joinMessage.getPrefix()->getNick().size() > 0 ) {
			addChannelUser( joinMessage.getPrefix()->getNick() );
			logSQLite.logJoin( channelName, joinMessage.getPrefix()->getNick() );
		}
	}
	catch(std::bad_cast) {}
	try {
		IRC::MessagePrivMsg const &privMsg = dynamic_cast< IRC::MessagePrivMsg const & >( message );
		if( privMsg.getReceiver() == channelName ) {
			logSQLite.logPrivMsg( channelName, privMsg.getPrefix()->getNick(), privMsg.getMessage() );
		}
	}
	catch(std::bad_cast) {}
	try {
		IRC::MessagePart const &partMessage = dynamic_cast< IRC::MessagePart const & >( message );
		if( std::find( channelUsers.begin(), channelUsers.end(), partMessage.getPrefix()->getNick() ) != channelUsers.end()) {
			removeChannelUser( partMessage.getPrefix()->getNick() );
			logSQLite.logQuit( channelName, partMessage.getPrefix()->getNick(), partMessage.getPartMessage() );
		}
	}
	catch(std::bad_cast) {}
	return std::auto_ptr< PlugInResult >( NULL );
}

std::auto_ptr< PlugInResult > PlugInLog::onPacketSent( state_t state, IRC::Message const &message ) {
	return std::auto_ptr< PlugInResult >( NULL );
}

std::auto_ptr< PlugInResult > PlugInLog::onTimeTrigger( state_t state ) {
	return std::auto_ptr< PlugInResult >( NULL );
}

std::auto_ptr< PlugInResult > PlugInLog::onDisconnect( state_t state ){
	// reset this plug-in:
	if( channelUsers.size() > 0 ) {
		channelUsers.clear();
	}
	return std::auto_ptr< PlugInResult >( NULL );
}

void PlugInLog::parseChannelUsers( std::string const &channelUsersStr ) {
	std::stringstream sstrUsers;
	sstrUsers << channelUsersStr;
	std::string lastUser, user;
	do {
		lastUser = user;
		sstrUsers >> user;
		if( user != lastUser && user.size() > 0 ) {
			addChannelUser( user );
		}
	}
	while( user != lastUser && user.size() != 0 );
}

void PlugInLog::addChannelUser( std::string const &user ) {
	if( std::find( channelUsers.begin(), channelUsers.end(), user ) == channelUsers.end() ) {
		channelUsers.push_back( user );
	}
}

void PlugInLog::removeChannelUser( std::string const &user ) {
	std::list< std::string >::iterator it = std::find( channelUsers.begin(), channelUsers.end(), user );
	if( it != channelUsers.end() ) {
		channelUsers.erase( it );
	}
}

}
