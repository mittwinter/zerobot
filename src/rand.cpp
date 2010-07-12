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

#include <cstdlib>
#include <ctime>

#include "rand.hpp"

double util::rand(double _high) {
	static bool initialized = false;
	if(!initialized) {
		srand(time(NULL));
		initialized = true;
	}
	return (std::rand() / (RAND_MAX + 1.0)) * (_high + 1.0);
}

