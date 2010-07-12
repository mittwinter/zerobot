/*	This file is part of zerobot.

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

#ifndef PLUGINS_JOIN_HPP
#define PLUGINS_JOIN_HPP

#include <string>

#include "base.hpp"

namespace zerobot {

class PlugInJoin : public PlugIn {
	public:
		PlugInJoin(int _priority, std::string const &_channelName);
		virtual ~PlugInJoin() {}

		virtual std::auto_ptr< PlugInResult > onConnect(state_t _state);
		virtual std::auto_ptr< PlugInResult > onPacket(state_t _state, IRC::Message const &_message);
		virtual std::auto_ptr< PlugInResult > onTimeTrigger(state_t _state);
		virtual std::auto_ptr< PlugInResult > onDisconnect(state_t _state);

	protected:
		typedef enum {
			STATE_JOIN_NOP,
			STATE_JOIN_SENT,
			STATE_JOIN_JOINED,
		} state_join_t;

		std::string channelName;
		state_join_t joinState;
};

}

#endif

