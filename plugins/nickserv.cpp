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

#include <iostream>
#include <string>

#include "nickserv.hpp"

namespace zerobot {

std::auto_ptr< PlugInResult > PlugInNickServ::onConnect( state_t state ) {
	return std::auto_ptr< PlugInResult >( NULL );
}

std::auto_ptr< PlugInResult > PlugInNickServ::onPacket( state_t state, IRC::Message const &message ) {
	return std::auto_ptr< PlugInResult >( NULL );
}

std::auto_ptr< PlugInResult > PlugInNickServ::onPacketSent( state_t state, IRC::Message const &message ) {
	return std::auto_ptr< PlugInResult >( NULL );
}

std::auto_ptr< PlugInResult > PlugInNickServ::onTimeTrigger( state_t state ) {
	std::auto_ptr< PlugInResult > result = std::auto_ptr< PlugInResult >( NULL );
	if( state == STATE_CONNECTED && !identified ) {
		std::string password;
		std::cout << "Please enter NickServ password: " << std::flush;
		std::cin >> password;
		result = std::auto_ptr< PlugInResult >( new PlugInResult );
		result->messages.push_back( new IRC::MessagePrivMsg( "NickServ", "IDENTIFY " + password ) );
		identified = true; // TODO: Really check for this!
	}
	return result;
}

std::auto_ptr< PlugInResult > PlugInNickServ::onDisconnect( state_t state ) {
	// reset this plug-in:
	if( identified ) {
		identified = false;
	}
	return std::auto_ptr< PlugInResult >( NULL );
}

}

