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

#ifndef PLUGINS_NICKSERV_HPP
#define PLUGINS_NICKSERV_HPP

#include <memory>

#include "../lib/SecretStorage.hpp"
#include "base.hpp"

namespace zerobot {

class PlugInNickServ : public PlugIn {
	public:
		PlugInNickServ( int priority ) : PlugIn( priority, "NickServ" ), identified( false ) {}
		virtual ~PlugInNickServ() {}

		virtual std::auto_ptr< PlugInResult > onConnect( state_t state );
		virtual std::auto_ptr< PlugInResult > onPacket( state_t state, IRC::Message const &message );
		virtual std::auto_ptr< PlugInResult > onPacketSent( state_t state, IRC::Message const &message );
		virtual std::auto_ptr< PlugInResult > onTimeTrigger( state_t state );
		virtual std::auto_ptr< PlugInResult > onDisconnect( state_t state );

	protected:
		std::auto_ptr< memory::SecretStorage > passwordStorage;
		bool identified;
};

}

#endif
