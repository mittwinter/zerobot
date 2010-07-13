/*	This file is part of zerobot.
	Copyright (C) 2010 Martin Wegner

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

#ifndef ZEROBOT_PLUG_IN_HPP
#define ZEROBOT_PLUG_IN_HPP

#include <list>
#include <memory>
#include <string>

#include "../lib/irc/message.hpp"
#include "../src/zerobot_state.hpp"

namespace zerobot {

class PlugInResult {
	public:
		PlugInResult();

		std::list< IRC::Message * > messages;
		state_t newState;
};

class PlugIn {
	public:
		PlugIn(unsigned int _priority, std::string const &_name);

		unsigned int getPriority() const { return priority; }
		std::string const &getName() const { return name; }

		virtual std::auto_ptr< PlugInResult > onConnect(state_t _state) = 0;
		virtual std::auto_ptr< PlugInResult > onPacket(state_t _state, IRC::Message const &_message) = 0;
		virtual std::auto_ptr< PlugInResult > onTimeTrigger(state_t _state) = 0;
		virtual std::auto_ptr< PlugInResult > onDisconnect(state_t _state) = 0;

	protected:
		const unsigned int priority;
		const std::string name;
};

}

#endif

