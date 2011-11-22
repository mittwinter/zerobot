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

#include <bits/local_lim.h> // for HOST_NAME_MAX (POSIX limit)
#include <typeinfo>
#include <unistd.h>

#include "connect.hpp"

namespace zerobot {

PlugInConnect::PlugInConnect( int priority, std::string botNickname )
		: PlugIn( priority, "connect" )
		, botNickname( botNickname )
		, connectState( STATE_CONNECT_NOP )
		{
}

std::auto_ptr< PlugInResult > PlugInConnect::onConnect( state_t state ) {
	std::auto_ptr< PlugInResult > result( NULL );
	if( state == STATE_CONNECTING ) {
		switch( connectState ) {
			case STATE_CONNECT_NOP:
				result = std::auto_ptr< PlugInResult >( new PlugInResult );
				result->messages.push_back( new IRC::MessageNick( botNickname ) );
				char hostnameBuffer[ HOST_NAME_MAX ];
				gethostname( hostnameBuffer, HOST_NAME_MAX );
				result->messages.push_back( new IRC::MessageUser( botNickname
				                                                , hostnameBuffer
				                                                , "foobar"
				                                                , botNickname
				                                                )
				                          );
				connectState = STATE_CONNECT_NICK_USER_SENT;
				break;
			default:
				break;
		}
	}
	return result;
}

std::auto_ptr< PlugInResult > PlugInConnect::onPacket( state_t state, IRC::Message const &message ) {
	std::auto_ptr< PlugInResult > result( NULL );
	try {
		IRC::MessageNumericReply const &reply = dynamic_cast< IRC::MessageNumericReply const & >( message );
		if( connectState == STATE_CONNECT_NICK_USER_SENT ) {
			if( reply.getReplyCode() == IRC::ERR_NONICKNAMEGIVEN
					|| reply.getReplyCode() == IRC::ERR_ERRONEUSNICKNAME
					|| reply.getReplyCode() == IRC::ERR_NICKNAMEINUSE
					|| reply.getReplyCode() == IRC::ERR_NICKCOLLISION
					|| reply.getReplyCode() == IRC::ERR_ALREADYREGISTERED
					) {
				std::cerr << "PlugInConnect: " << reply << std::flush;
				result = std::auto_ptr< PlugInResult >( new PlugInResult );
				result->newState = STATE_DISCONNECTING;
			}
			else if( reply.getReplyCode() == IRC::RPL_ENDOFMOTD ) {
				result = std::auto_ptr< PlugInResult >( new PlugInResult );
				result->newState = STATE_CONNECTED;
			}
		}
	}
	catch( std::bad_cast const & ) {
	}
	return result;
}

std::auto_ptr< PlugInResult > PlugInConnect::onTimeTrigger( state_t state ) {
	return std::auto_ptr< PlugInResult >( NULL );
}

std::auto_ptr< PlugInResult > PlugInConnect::onDisconnect( state_t state ) {
	// reset this plug-in:
	if( connectState == STATE_CONNECT_NICK_USER_SENT ) {
		connectState = STATE_CONNECT_NOP;
	}
	return std::auto_ptr< PlugInResult >( NULL );
}

}

