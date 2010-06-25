#ifndef ZEROBOT_PLUG_IN_HPP
#define ZEROBOT_PLUG_IN_HPP

#include <list>
#include <string>

#include "irc/message.hpp"
#include "zerobot_state.hpp"

namespace zerobot {

struct PlugInResult {
	std::list< std::string > messages;
	state_t newState;
};

class PlugIn {
	public:
		PlugIn(unsigned int _priority, std::string const &_name);

		unsigned int getPriority() const { return priority; }
		std::string const &getName() const { return name; }

		virtual PlugInResult onConnect(IRC::Message const &_message) = 0;
		virtual PlugInResult onPacket(IRC::Message const &_message) = 0;
		virtual PlugInResult onDisconnect(IRC::Message const &_message) = 0;

	protected:
		const unsigned int priority;
		const std::string name;
};

}

#endif

