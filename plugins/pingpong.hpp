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

