#ifndef PLUGINS_PINGPONG_HPP
#define PLUGINS_PINGPONG_HPP

#include <memory>

#include "../irc/message.hpp"
#include "base.hpp"

namespace zerobot {

class PlugInPingPong : public PlugIn {
	public:
		PlugInPingPong(int _priority);
		virtual ~PlugInPingPong() {}

		virtual std::auto_ptr< PlugInResult > onConnect(IRC::Message const &_message);		
		virtual std::auto_ptr< PlugInResult > onPacket(IRC::Message const &_message);		
		virtual std::auto_ptr< PlugInResult > onDisconnect(IRC::Message const &_message);		

	protected:
};

}

#endif

