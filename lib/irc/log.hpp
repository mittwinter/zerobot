#ifndef IRC_LOG_HPP
#define IRC_LOG_HPP

#include "message.hpp"

namespace IRC {

class Log {
	public:
		Log() {}
		virtual ~Log() {}

		virtual void logMessage(Message const &_message) = 0;

	protected:
};

}

#endif

