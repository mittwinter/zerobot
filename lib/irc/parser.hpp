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

#ifndef PARSER_HPP
#define PARSER_HPP

#include <memory>
#include <stdexcept>
#include <string>

#include "message.hpp"

namespace IRC {

class RawParser {
	public:
		RawParser(bool _debug = false);
		virtual ~RawParser() {}

		std::auto_ptr< RawMessage > parseString(std::string _message) const throw(std::runtime_error);

	protected:
		static std::string const special;

		bool debug;

		std::auto_ptr< Prefix > parsePrefix(std::string &_message) const throw(std::runtime_error);
		std::string parseNick(std::string &_message) const throw(std::runtime_error);
		std::string parseUser(std::string &_message) const throw(std::runtime_error);
		std::string parseHost(std::string &_message) const throw(std::runtime_error);
		std::string parseCommand(std::string &_message) const throw(std::runtime_error);
		std::vector< std::string > parseParameters(std::string &_message) const;
		std::string parseTrailing(std::string &_message) const;

		void skipSpace(std::string &_str) const;
};

class Parser {
	public:
		Parser(bool _debug = false);
		virtual ~Parser() {}

		std::auto_ptr< Message > parseMessage(std::string _message) const throw(std::runtime_error);

	protected:
		std::auto_ptr< RawParser > rawParser;
};

}

#endif

