#ifndef PLUGINS_LOG_HPP
#define PLUGINS_LOG_HPP

#include <list>
#include <string>

#include "../lib/irc/log_sqlite.hpp"
#include "base.hpp"

namespace zerobot {

class PlugInLog : public PlugIn {
	public:
		PlugInLog(int _priority, std::string const &_name, std::string const &_channelName, std::string const &_databaseFilename);
		virtual ~PlugInLog();

		virtual std::auto_ptr< PlugInResult > onConnect(state_t _state);
		virtual std::auto_ptr< PlugInResult > onPacket(state_t _state, IRC::Message const &_message);
		virtual std::auto_ptr< PlugInResult > onTimeTrigger(state_t _state);
		virtual std::auto_ptr< PlugInResult > onDisconnect(state_t _state);

		std::string const &getChannelName() const { return channelName; }

	protected:
		std::string channelName;
		std::list< std::string > channelUsers;
		IRC::LogSQLite logSQLite;

		void parseChannelUsers(std::string const &_channelUsersStr);
		void addChannelUser(std::string const &_user);
		void rmChannelUser(std::string const &_user);
};

}

#endif

