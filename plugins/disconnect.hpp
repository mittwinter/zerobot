#ifndef PLUGINS_DISCONNECT_HPP
#define PLUGINS_DISCONNECT_HPP

#include <string>
#include <vector>

#include "base.hpp"

namespace zerobot {

class PlugInDisconnect : public PlugIn {
	public:
		PlugInDisconnect(int _priority);
		virtual ~PlugInDisconnect() {}

		virtual std::auto_ptr< PlugInResult > onConnect(state_t _state);
		virtual std::auto_ptr< PlugInResult > onPacket(state_t _state, IRC::Message const &_message);
		virtual std::auto_ptr< PlugInResult > onTimeTrigger(state_t _state);
		virtual std::auto_ptr< PlugInResult > onDisconnect(state_t _state);

	protected:
		static std::vector< std::string > quitMessages;
};

}

#endif

