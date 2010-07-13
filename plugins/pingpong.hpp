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

#ifndef PLUGINS_PINGPONG_HPP
#define PLUGINS_PINGPONG_HPP

#include <string>
#include <time.h>

#include "base.hpp"

namespace zerobot {

class PlugInPingPong : public PlugIn {
	public:
		PlugInPingPong(int _priority, std::string const &_serverName);
		virtual ~PlugInPingPong() {}

		virtual std::auto_ptr< PlugInResult > onConnect(state_t _state);
		virtual std::auto_ptr< PlugInResult > onPacket(state_t _state, IRC::Message const &_message);
		virtual std::auto_ptr< PlugInResult > onTimeTrigger(state_t _state);
		virtual std::auto_ptr< PlugInResult > onDisconnect(state_t _state);

	protected:
		std::string serverName;
		time_t lastPing;
		time_t lastPong;
};

}

#endif

