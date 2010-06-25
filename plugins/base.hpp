#ifndef ZEROBOT_PLUG_IN_HPP
#define ZEROBOT_PLUG_IN_HPP

#include <list>
#include <string>

#include "../irc/message.hpp"
#include "../src/zerobot_state.hpp"

namespace zerobot {

class PlugInResult {
	public:
		PlugInResult();

		std::list< std::string > messages;
		state_t newState;
};

class PlugIn {
	public:
		PlugIn(unsigned int _priority, std::string const &_name);

		unsigned int getPriority() const { return priority; }
		std::string const &getName() const { return name; }

		virtual std::auto_ptr< PlugInResult > onConnect(IRC::Message const &_message) = 0;
		virtual std::auto_ptr< PlugInResult > onPacket(IRC::Message const &_message) = 0;
		virtual std::auto_ptr< PlugInResult > onDisconnect(IRC::Message const &_message) = 0;

	protected:
		const unsigned int priority;
		const std::string name;
};

}

#endif

