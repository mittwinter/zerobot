#ifndef IRC_LOG_HPP
#define IRC_LOG_HPP

#include <string>

namespace IRC {

class Log {
	public:
		Log() {}
		virtual ~Log() {}

		virtual void logNamesList(std::string const &_channelName, std::string const &_namesListStr) = 0;
		virtual void logNick(std::string const &_channelName, std::string const &_oldNickname, std::string const &_newNickname) = 0;
		virtual void logQuit(std::string const &_channelName, std::string const &_nickname, std::string const &_quitMessage) = 0;
		virtual void logJoin(std::string const &_channelName, std::string const &_nickname) = 0;
		virtual void logPrivMsg(std::string const &_channelName, std::string const &_nickname, std::string const &_message) = 0;

	protected:
};

}

#endif

