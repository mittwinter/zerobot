#include <cstdlib> // for EXIT_SUCCESS and EXIT_FAILURE
#include <fstream>
#include <iostream>
#include <memory>

#include "../irc/message.hpp"
#include "../irc/parser.hpp"

int main(int argc, char *argv[]) {
	if(argc < 2) {
		std::cerr << "Missing raw irc dump." << std::endl
		          << "\t " << argv[0] << " <raw-irc-log>" << std::endl;
		return EXIT_FAILURE;
	}
	std::auto_ptr< IRC::RawParser > rawParser(new IRC::RawParser);
	std::auto_ptr< IRC::Parser > parser(new IRC::Parser);
	std::auto_ptr< IRC::RawMessage > parsedRawMessage(NULL);
	std::auto_ptr< IRC::Message > parsedMessage(NULL);

	std::ifstream rawLog(argv[1]);
	if(rawLog.good()) {
		while(!rawLog.eof()) {
			std::string rawLine;
			std::getline(rawLog, rawLine);
			if(rawLine.size() > 2 && rawLine.substr(0, 3) == ">> ") {
				try {
					parsedRawMessage = rawParser->parseString(rawLine.substr(3));
					parsedMessage = parser->parseMessage(rawLine.substr(3));
					// std::cout << "Parsed to " << *parsedRawMessage << std::endl;
				}
				catch(std::runtime_error e) {
					std::cerr << e.what() << std::endl;
				}
			}
		}
		rawLog.close();
		return EXIT_SUCCESS;
	}
	else {
		rawLog.close();
		return EXIT_FAILURE;
	}
}
