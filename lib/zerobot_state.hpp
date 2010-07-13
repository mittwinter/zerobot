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

#ifndef ZEROBOT_STATE_HPP
#define ZEROBOT_STATE_HPP

namespace zerobot {

typedef enum {
	STATE_NOP,
	STATE_CONNECTING,
	STATE_CONNECTED,
	STATE_RUNNING,
	STATE_DISCONNECTING,
	STATE_DISCONNECTED
} state_t;

}

#endif

