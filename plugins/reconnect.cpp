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

#include "reconnect.hpp"

namespace zerobot {

PlugInReconnect::PlugInReconnect( int priority ) : PlugIn( priority, "reconnect" ) {
}

std::auto_ptr< PlugInResult > PlugInReconnect::onConnect( state_t state ) {
	return std::auto_ptr< PlugInResult >( NULL );
}

std::auto_ptr< PlugInResult > PlugInReconnect::onPacket( state_t state, IRC::Message const &message ) {
	return std::auto_ptr< PlugInResult >( NULL );
}

std::auto_ptr< PlugInResult > PlugInReconnect::onPacketSent( state_t state, IRC::Message const &message ) {
	return std::auto_ptr< PlugInResult >( NULL );
}

std::auto_ptr< PlugInResult > PlugInReconnect::onTimeTrigger( state_t state ) {
	return std::auto_ptr< PlugInResult >( NULL );
}

std::auto_ptr< PlugInResult > PlugInReconnect::onDisconnect( state_t state ) {
	std::auto_ptr< PlugInResult > result( NULL );
	if( state == STATE_DISCONNECTED ) {
		result = std::auto_ptr< PlugInResult >( new PlugInResult );
		result->newState = STATE_CONNECTING;
	}
	return result;
}

}

