/*	This file is part of zerobot.
	Copyright (C) 2010-2011 Martin Wegner

	zerobot is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	zerobot is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with zerobot.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <cstdlib> // for EXIT_SUCCESS and EXIT_FAILURE
#include <getopt.h>
#include <iostream>
#include <list>
#include <sstream>
#include <string>

#include "../plugins/plugins.hpp"
#include "zerobot.hpp"

void showUsage(char const *_programName) {
		std::clog << "Usage:" << "\t" << _programName << "\t --server=<server> --port=<port> --nick=<nickname>" << std::endl
		          << "\t\t\t [--join=<channel> ...] [--log=<channel> ... --logfile=<file>] [--title] [--admin=<nickname>]" << std::endl;
}

int optionsFlagTitle = 0;

struct option options[] = {
		{ "server"
		, required_argument
		, 0
		, 's'
		},
		{ "port"
		, required_argument
		, 0
		, 'p'
		},
		{ "nick"
		, required_argument
		, 0
		, 'n'
		},
		{ "join"
		, required_argument
		, 0
		, 'j'
		},
		{ "log"
		, required_argument
		, 0
		, 'l'
		},
		{ "logfile"
		, required_argument
		, 0
		, 'f'
		},
		{ "title"
		, no_argument
		, &optionsFlagTitle
		, 1
		},
		{ "admin"
		, required_argument
		, 0
		, 'a'
		},
		{ 0, 0, 0, 0 }
	};

int main(int argc, char *argv[]) {
	int getoptResult = -1;
	int optionIndex = 0;
	bool serverNameFound = false, serverPortFound = false, nicknameFound = false, logfileFound = false;
	std::string serverName, serverPort, nickname, logfile, admin;
	std::list< std::string > joinChannels;
	std::list< std::string > logChannels;
	do {
		getoptResult = getopt_long(argc, argv, "s:p:n:j:l:f:ta:", options, &optionIndex);
		switch(getoptResult) {
			case -1: // all options parsed...
			case 0: // getopt() set flag...
				break;
			case 's':
				serverNameFound = true;
				serverName = optarg;
				break;
			case 'p':
				serverPortFound = true;
				serverPort = optarg;
				break;
			case 'n':
				nicknameFound = true;
				nickname = optarg;
				break;
			case 'j':
				joinChannels.push_back(optarg);
				break;
			case 'l':
				joinChannels.push_back(optarg);
				logChannels.push_back(optarg);
				break;
			case 'f':
				logfileFound = true;
				logfile = optarg;
				break;
//			case 't':
//				optionsFlagTitle = 1;
//				break;
			case 'a':
				admin = optarg;
				break;
			case '?':
				// TODO: print more detailled error message here!
				return EXIT_FAILURE;
				break;
			default:
				std::cerr << argv[0] << ": unknown option '" << optopt << "'" << std::endl;
				showUsage(argv[0]);
				return EXIT_FAILURE;
				break;
		}
	}
	while(getoptResult != -1);

	if(!serverNameFound || !serverPortFound || !nicknameFound) {
		std::clog << argv[0] << ": server, port and nickname are required" << std::endl;
		showUsage(argv[0]);
		return EXIT_FAILURE;
	}
	if(logChannels.size() > 0 && !logfileFound) {
		std::clog << argv[0] << ": logging facility needs a logfile" << std::endl;
		showUsage(argv[0]);
		return EXIT_FAILURE;
	}

	// Parse port to integer via stringstream:
	std::stringstream sstrPort;
	sstrPort << serverPort;
	int serverPortInt = 0;
	sstrPort >> serverPortInt;

	// Start up bot:
	zerobot::ZeroBot bot(serverName, serverPortInt);
	// Plug-ins:
	zerobot::PlugIn *plugIn = NULL;
	// Register basic plug-ins:
	// - Connect:
	plugIn = new zerobot::PlugInConnect(-10, nickname);
	bot.registerPlugIn(*plugIn);
	// - Disconnect:
	plugIn = new zerobot::PlugInDisconnect(-10);
	bot.registerPlugIn(*plugIn);
	// - PingPong:
	plugIn = new zerobot::PlugInPingPong(0, serverName);
	bot.registerPlugIn(*plugIn);
	// Register optional plug-ins, multiple definitions were allowed:
	for(std::list< std::string >::const_iterator it = joinChannels.begin(); it != joinChannels.end(); it++) {
		plugIn = new zerobot::PlugInJoin(-5, *it);
		bot.registerPlugIn(*plugIn);
	}
	for(std::list< std::string >::const_iterator it = logChannels.begin(); it != logChannels.end(); it++) {
		plugIn = new zerobot::PlugInLog(0, "log_" + *it, *it, logfile);
		bot.registerPlugIn(*plugIn);
	}
	if(optionsFlagTitle) {
		plugIn = new zerobot::PlugInURLTitle(5);
		bot.registerPlugIn(*plugIn);
	}
	plugIn = new zerobot::PlugInAdmin(0, admin); // it's not bad when admin's nickname was not specified
	bot.registerPlugIn(*plugIn);
	// Run bot:
	bot.run();
	return EXIT_SUCCESS;
}
