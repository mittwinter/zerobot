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

#include "../lib/rand.hpp"

#include "disconnect.hpp"

namespace zerobot {

std::vector< std::string > PlugInDisconnect::quitMessages;

PlugInDisconnect::PlugInDisconnect( int _priority ) : PlugIn( priority, "disconnect" ) {
	if( quitMessages.size() == 0 ) {
		quitMessages.push_back( "I'll die alone ... and disconnected." );
		quitMessages.push_back( "PADI! - Fight for your right!" );
	}
}

std::auto_ptr< PlugInResult > PlugInDisconnect::onConnect( state_t state ) {
	return std::auto_ptr< PlugInResult >( NULL );
}

std::auto_ptr< PlugInResult > PlugInDisconnect::onPacket( state_t state, IRC::Message const &message ) {
	return std::auto_ptr< PlugInResult >( NULL );
}

std::auto_ptr< PlugInResult > PlugInDisconnect::onPacketSent( state_t state, IRC::Message const &message ) {
	return std::auto_ptr< PlugInResult >( NULL );
}

std::auto_ptr< PlugInResult > PlugInDisconnect::onTimeTrigger( state_t state ) {
	return std::auto_ptr< PlugInResult >( NULL );
}

std::auto_ptr< PlugInResult > PlugInDisconnect::onDisconnect( state_t state ) {
	std::auto_ptr< PlugInResult > result( NULL );
	switch( state ) {
		case STATE_DISCONNECTING:
			result = std::auto_ptr< PlugInResult >( new PlugInResult );
			result->newState = STATE_DISCONNECTED;
			result->messages.push_back( new IRC::MessageQuit( quitMessages.at( util::rand( quitMessages.size() - 1 ) ) ) );
			break;
		default:
			break;
	}
	return result;
}

}

