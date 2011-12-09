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

#include "base.hpp"

namespace zerobot {

CommandParser::CommandParser( std::string const &nickname, std::string const &message )
		: nickname( nickname )
		, message( message ) {
}

void CommandParser::parse() throw( std::runtime_error ) {
	trim( message );
	std::string word;
	while( (word = extractWord( message )).size() > 0 ) {
		if( command.size() == 0 ) {
			if( word.at( word.size() - 1 ) == ':' && word != nickname + ':' ) {
				throw std::runtime_error( "CommandParser::parse(): Message not directed to us, ignoring..." );
			}
			else if( word.substr( 0, getCommandPrefix().size() ) == getCommandPrefix() ) {
				command = word.substr( getCommandPrefix().size() );
			}
		}
		else {
			arguments.push_back( word );
		}
	}
}

char const *CommandParser::whitespace = "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x20\x1a\x1b\x1c\x1d\x1e\x1f\x7f";
std::string const CommandParser::commandPrefix = "!";

void CommandParser::trim( std::string &str ) const {
	while( str.size() > 0 && (isspace( str.at( 0 ) ) || iscntrl( str.at( 0 ) )) ) {
		str.erase( 0, 1 );
	}
	while( str.size() > 0 && (isspace( str.at( str.size() - 1 ) ) || iscntrl( str.at( str.size() - 1 ) )) ) {
		str.erase( str.size() - 1, 1 );
	}
}

std::string CommandParser::extractWord( std::string &str ) const {
	if( str.size() > 0 ) {
		std::string::size_type whitespacePos = str.find_first_of( whitespace );
		for( unsigned int i = 0; i < str.size(); i++ ) {
		}
		std::string word = str.substr( 0, whitespacePos );
		str.erase( 0, ( whitespacePos == std::string::npos ? std::string::npos : whitespacePos + 1 ) );
		return word;
	}
	else {
		return std::string();
	}
}

PlugInResult::PlugInResult() : newState( STATE_NOP ) {
}

PlugIn::PlugIn( unsigned int priority, std::string const &name ) : priority( priority ), name( name ) {
}

}
