#ifndef PLUGINS_PINGPONG_HPP
#define PLUGINS_PINGPONG_HPP

#include "base.hpp"

namespace zerobot {

class PlugInPingPong : public PlugIn {
	protected:
		PlugInPingPong(int _priority);
		virtual ~PlugInPingPong() {}

		virtual PlugInResult onConnect(IRC::Message const &_message);		
		virtual PlugInResult onPacket(IRC::Message const &_message);		
		virtual PlugInResult onDisconnect(IRC::Message const &_message);		

	protected:
};

}

#endif

