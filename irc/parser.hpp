#ifndef PARSER_HPP
#define PARSER_HPP

#include <memory>
#include <stdexcept>
#include <string>

#include "message.hpp"

namespace IRC {

class RawParser {
	public:
		RawParser(bool _debug = false) {}
		virtual ~RawParser() {}

		std::auto_ptr< RawMessage > parseString(std::string _message) const;

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
		Parser();
		virtual ~Parser() {}

		std::auto_ptr< Message > parseMessage(std::string _message) const;

	protected:
		std::auto_ptr< RawParser > rawParser;
};

}

#endif

