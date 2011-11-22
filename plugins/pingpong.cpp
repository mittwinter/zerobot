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

#include <iostream> // TODO: remove after debugging
#include <typeinfo>

#include "pingpong.hpp"

namespace zerobot {

PlugInPingPong::PlugInPingPong( int priority, std::string const &serverName )
		: PlugIn( priority, "pingpong" )
		, serverName( serverName )
		, lastPing( time( NULL ) )
		, lastPong( time( NULL ) )
		{
}

std::auto_ptr< PlugInResult > PlugInPingPong::onConnect( state_t state ) {
	return std::auto_ptr< PlugInResult >( NULL );
}

std::auto_ptr< PlugInResult > PlugInPingPong::onPacket( state_t state, IRC::Message const &message ) {
	try {
		IRC::MessagePing const &ping = dynamic_cast< IRC::MessagePing const & >( message );
		lastPing = time( NULL );
		std::auto_ptr< PlugInResult > result( new PlugInResult );
		result->messages.push_back( new IRC::MessagePong( ping.getServerName() ) );
		return result;
	}
	catch( std::bad_cast const & ) {}
	try {
		IRC::MessagePong const &pong __attribute__((unused)) = dynamic_cast< IRC::MessagePong const & >( message );
		lastPong = time( NULL );
	}
	catch( std::bad_cast const & ) {}
	return std::auto_ptr< PlugInResult >( NULL );
}

std::auto_ptr< PlugInResult > PlugInPingPong::onTimeTrigger( state_t state ) {
	std::auto_ptr< PlugInResult > result( NULL );
	if( (lastPing - lastPong) > 125 ) {
		std::cerr << "PlugInPingPong: Connection timeout..." << std::endl;
		result = std::auto_ptr< PlugInResult >( new PlugInResult );
		result->newState = STATE_DISCONNECTING;
	}
	else if( (time( NULL ) - lastPing) > 60 ) {
		std::clog << "PlugInPingPong: Last ping is more than 60 seconds in the past, pinging server ..."
		          << std::endl;
		lastPing = time( NULL );
		result = std::auto_ptr< PlugInResult >( new PlugInResult );
		result->messages.push_back( new IRC::MessagePing( serverName ) );
	}
	return result;
}

std::auto_ptr< PlugInResult > PlugInPingPong::onDisconnect( state_t state ) {
	// reset this plug-in:
	lastPing = time( NULL );
	lastPong = time( NULL );
	return std::auto_ptr< PlugInResult >( NULL );
}

}

