#include <cstdlib> // for EXIT_SUCCESS and EXIT_FAILURE
#include <iostream>
#include <sstream>

#include "../plugins/base.hpp"
#include "../plugins/pingpong.hpp"
#include "zerobot.hpp"

#include "../irc/message.hpp"

int main(int argc, char *argv[]) {
	if(argc < 3) {
		std::cerr << "Usage:" << std::endl
		          << "\t " << argv[0] << " <server> <port>" << std::endl;
		return EXIT_FAILURE;
	}
	std::stringstream sstrPort;
	sstrPort << argv[2];
	int serverPort = 0;
	sstrPort >> serverPort;
	zerobot::ZeroBot bot(argv[1], serverPort);
	// Register plug-ins:
	zerobot::PlugIn *plugIn = NULL;
	plugIn = new zerobot::PlugInPingPong(0);
	bot.registerPlugIn(*plugIn);
	// Run bot:
	bot.run();
	return EXIT_SUCCESS;
}
