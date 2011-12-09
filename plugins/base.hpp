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

#ifndef PLUGINS_BASE_HPP
#define PLUGINS_BASE_HPP

#include <list>
#include <memory>
#include <stdexcept>
#include <string>

#include "../lib/irc/message.hpp"
#include "../lib/zerobot_state.hpp"

namespace zerobot {

class CommandParser {
	public:
		CommandParser( std::string const &nickname, std::string const &message );
		virtual ~CommandParser() {}

		static std::string const &getCommandPrefix() { return commandPrefix; }

		virtual void parse() throw( std::runtime_error );

		std::string const &getCommand() const { return command; }
		std::vector< std::string > const &getArguments() const { return arguments; }

	protected:
		static char const *whitespace;
		static std::string const commandPrefix;
		std::string nickname;
		std::string message;
		std::string command;
		std::vector< std::string > arguments;

		void trim( std::string &str ) const;
		std::string extractWord( std::string &str ) const;
};

class PlugIn; // forward declaration of class PlugIn for PlugInResult
class PlugInResult {
	public:
		PlugInResult();

		std::list< IRC::Message * > messages;
		state_t newState;
		std::list< PlugIn * > registerPlugIns;
		std::list< std::string > unregisterPlugins;
};

class PlugIn {
	public:
		PlugIn( unsigned int priority, std::string const &name );
		virtual ~PlugIn() {}

		unsigned int getPriority() const { return priority; }
		std::string const &getName() const { return name; }
		std::string const &getCommandPrefix() const { return commandPrefix; }

		virtual std::auto_ptr< PlugInResult > onConnect( state_t state ) = 0;
		virtual std::auto_ptr< PlugInResult > onPacket( state_t state, IRC::Message const &message ) = 0;
		virtual std::auto_ptr< PlugInResult > onPacketSent( state_t state, IRC::Message const &message ) = 0;
		virtual std::auto_ptr< PlugInResult > onTimeTrigger( state_t state ) = 0;
		virtual std::auto_ptr< PlugInResult > onDisconnect( state_t state ) = 0;

	protected:
		const unsigned int priority;
		const std::string name;

		static std::string const commandPrefix;
};

}

#endif

