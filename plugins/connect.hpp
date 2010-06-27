#ifndef PLUGINS_CONNECT_HPP
#define PLUGINS_CONNECT_HPP

#include <string>

#include "base.hpp"

namespace zerobot {

class PlugInConnect : public PlugIn {
	public:
		PlugInConnect(int _priority, std::string _botNickname);
		virtual ~PlugInConnect() {}

		virtual std::auto_ptr< PlugInResult > onConnect(state_t _state);
		virtual std::auto_ptr< PlugInResult > onPacket(state_t _state, IRC::Message const &_message);
		virtual std::auto_ptr< PlugInResult > onTimeTrigger(state_t _state);
		virtual std::auto_ptr< PlugInResult > onDisconnect(state_t _state);

	protected:
		typedef enum {
			STATE_CONNECT_NOP,
			STATE_CONNECT_NICK_USER_SENT
		} state_connect_t;

		std::string botNickname;
		state_connect_t connectState;
};

}
#endif

