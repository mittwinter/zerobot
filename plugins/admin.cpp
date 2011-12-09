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

#include <cctype>
#include <sstream>
#include <typeinfo>

#include "../lib/rand.hpp"
#include "../lib/version.hpp"
#include "join.hpp"

#include "admin.hpp"

namespace zerobot {

std::vector< std::string > PlugInAdmin::partPhrases;

PlugInAdmin::PlugInAdmin( int priority, std::string const &adminNickname )
		: PlugIn( priority, "admin" )
		, adminNickname( adminNickname )
		{
	if( partPhrases.size() == 0 ) {
		partPhrases.push_back( "Ok, I'll leave you guys alone ... but behave while I'm away!" );
		partPhrases.push_back( "I was zerobot...ot....ot.....ot......ot..." );
		partPhrases.push_back( "cya" );
	}
}

std::auto_ptr< PlugInResult > PlugInAdmin::onConnect( state_t state ) {
	return std::auto_ptr< PlugInResult >( NULL );
}

std::auto_ptr< PlugInResult > PlugInAdmin::onPacket( state_t state, IRC::Message const &message ) {
	std::auto_ptr< PlugInResult > result( NULL );
	try {
		// Track our nickname:
		IRC::MessageNick const &nickMessage = dynamic_cast< IRC::MessageNick const & >( message );
		if( nickname.size() > 0 && nickMessage.getPrefix() != NULL && nickname == nickMessage.getPrefix()->getNick() ) {
			nickname = nickMessage.getNickname();
		}
	}
	catch( std::bad_cast const & ) {}
	try {
		IRC::MessagePrivMsg const &privMessage = dynamic_cast< IRC::MessagePrivMsg const & >( message );
		if( checkForCommand( privMessage.getMessage() ) ) {
			result = parseCommandMessage( privMessage );
		}
	}
	catch( std::bad_cast const & ) {}
	return result;
}

std::auto_ptr< PlugInResult > PlugInAdmin::onPacketSent( state_t state, IRC::Message const &message ) {
	try {
		IRC::MessageNick const &nickMessage = dynamic_cast< IRC::MessageNick const & >( message );
		nickname = nickMessage.getNickname();
	}
	catch( std::bad_cast const & ) {}
	return std::auto_ptr< PlugInResult >( NULL );
}

std::auto_ptr< PlugInResult > PlugInAdmin::onTimeTrigger( state_t state ) {
	return std::auto_ptr< PlugInResult >( NULL );
}

std::auto_ptr< PlugInResult > PlugInAdmin::onDisconnect( state_t state ) {
	return std::auto_ptr< PlugInResult >( NULL );
}

bool PlugInAdmin::checkForCommand( std::string const &message ) const {
	return (    message.substr( 0, nickname.size() + 1 ) == nickname + ':'
	         || message.substr( 0, CommandParser::getCommandPrefix().size() ) == CommandParser::getCommandPrefix()
	       );
}

bool PlugInAdmin::checkAdminNickname( std::string const &nickname ) const {
	return (adminNickname.size() > 0 && adminNickname == nickname);
}

std::auto_ptr< PlugInResult > PlugInAdmin::parseCommandMessage( IRC::MessagePrivMsg const &privMessage ) {
	std::auto_ptr< PlugInResult > result( NULL );
	try {
		CommandParser parser( nickname, privMessage.getMessage() );
		parser.parse();
		result = std::auto_ptr< PlugInResult >( new PlugInResult );
		// *** Admin commands:
		if( checkAdminNickname( privMessage.getPrefix()->getNick() ) ) {
			// - !quit:
			if( parser.getCommand() == "quit" ) {
				result->newState = STATE_DISCONNECTING;
			}
			// - !join:
			else if( parser.getCommand() == "join" ) {
				for( std::vector< std::string >::const_iterator it = parser.getArguments().begin(); it != parser.getArguments().end(); it++ ) {
					PlugIn *plugIn = new PlugInJoin( -5, *it );
					result->registerPlugIns.push_back( plugIn );
				}
			}
			// - !leave:
			else if( parser.getCommand() == "leave" ) {
				// Check for given channels:
				if( parser.getArguments().size() > 0 ) {
					for( std::vector< std::string >::const_iterator it = parser.getArguments().begin(); it != parser.getArguments().end(); it++ ) {
						result->messages.push_back( new IRC::MessagePart( *it
						                                                , partPhrases.at( util::rand( partPhrases.size() - 1 ) )
						                                                )
						                          );
						result->unregisterPlugins.push_back( "join_" + *it );
					}
				}
				// Otherwise use channel this message originated:
				else if( privMessage.getReceiver().size() > 1 && privMessage.getReceiver().at( 0 ) == '#' ) {
					result->messages.push_back( new IRC::MessagePart( privMessage.getReceiver()
					                                                , partPhrases.at( util::rand( partPhrases.size() - 1 ) )
					                                                )
					                          );
					result->unregisterPlugins.push_back( "join_" + privMessage.getReceiver() );
				}
			}
		}
		// *** Other commands:
		// - !version:
		if( parser.getCommand() == "version" ) {
			std::clog << "!!!VERSION!!!" << std::endl;
			result->messages.push_back( new IRC::MessagePrivMsg( privMessage.getPrefix()->getNick()
			                                                   , zerobot::versionString
			                                                   )
			                          );
		}
	}
	catch( std::runtime_error const &e ) {
		std::cerr << "PlugInAdmin: CommandParser failed with:" << std::endl
				  << "\t" << e.what() << std::endl;
	}
	return result;
}

}

