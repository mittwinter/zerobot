#ifndef PLUGINS_URLTITLE_HPP
#define PLUGINS_URLTITLE_HPP

#include "base.hpp"

namespace zerobot {

class PlugInURLTitle : public PlugIn {
	public:
		PlugInURLTitle(int _priority);
		virtual ~PlugInURLTitle() {}

		virtual std::auto_ptr< PlugInResult > onConnect(state_t _state);
		virtual std::auto_ptr< PlugInResult > onPacket(state_t _state, IRC::Message const &_message);
		virtual std::auto_ptr< PlugInResult > onTimeTrigger(state_t _state);
		virtual std::auto_ptr< PlugInResult > onDisconnect(state_t _state);

	protected:
};

}

#endif