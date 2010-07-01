#ifndef PLUGINS_JOIN_HPP
#define PLUGINS_JOIN_HPP

#include <string>

#include "base.hpp"

namespace zerobot {

class PlugInJoin : public PlugIn {
	public:
		PlugInJoin(int _priority, std::string const &_channelName);
		virtual ~PlugInJoin() {}

		virtual std::auto_ptr< PlugInResult > onConnect(state_t _state);
		virtual std::auto_ptr< PlugInResult > onPacket(state_t _state, IRC::Message const &_message);
		virtual std::auto_ptr< PlugInResult > onTimeTrigger(state_t _state);
		virtual std::auto_ptr< PlugInResult > onDisconnect(state_t _state);

	protected:
		typedef enum {
			STATE_JOIN_NOP,
			STATE_JOIN_SENT,
			STATE_JOIN_JOINED,
		} state_join_t;

		std::string channelName;
		state_join_t joinState;
};

}

#endif

