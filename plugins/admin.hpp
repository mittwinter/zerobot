#ifndef PLUGINS_ADMIN_HPP
#define PLUGINS_ADMIN_HPP

#include <string>

#include "base.hpp"

namespace zerobot {

class PlugInAdmin : public PlugInAdminBase {
	public:
		PlugInAdmin(int _priority, std::string const &_adminNickname);
		virtual ~PlugInAdmin() {}

		virtual std::auto_ptr< PlugInResult > onConnect(state_t _state);
		virtual std::auto_ptr< PlugInResult > onPacket(state_t _state, IRC::Message const &_message);
		virtual std::auto_ptr< PlugInResult > onTimeTrigger(state_t _state);
		virtual std::auto_ptr< PlugInResult > onDisconnect(state_t _state);

	protected:
		std::string adminNickname;
		static std::string const versionString;

		std::string trim(std::string _str) const;
		bool checkAdminNickname(std::string const &_nickname) const;
};

}

#endif
