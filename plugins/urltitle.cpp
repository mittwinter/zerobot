#include <buffio.h>
#include <curl/curl.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <tidy.h>

#include "urltitle.hpp"

namespace zerobot {

size_t curlGlobalWriteDataCallback(void *_data, size_t _size, size_t _nmemb, void *_plugInStorage) {
	PlugInURLTitlePtrStorage *plugInStorage = static_cast< PlugInURLTitlePtrStorage * >(_plugInStorage);
	return plugInStorage->plugIn->curlWriteDataCallback(_data, _size, _nmemb);
}

// TODO: match all whitespace here!
char const *PlugInURLTitle::whitespace = " \t\x0f";
bool PlugInURLTitle::curlInitialized = false;
unsigned int PlugInURLTitle::curlMaxBufferSize = 20 * 1024;

PlugInURLTitle::PlugInURLTitle(int _priority) : PlugIn(_priority, "urltitle") {
	if(!curlInitialized) {
		curl_global_init(CURL_GLOBAL_ALL);
		curlInitialized = true;
	}
	std::cerr << "PlugInURLTitle: Using curl version: " << curl_version() << std::endl;
	curlBuffer.reserve(curlMaxBufferSize);
}

PlugInURLTitle::~PlugInURLTitle() {
	if(curlInitialized) {
		curl_global_cleanup();
		curlInitialized = false;
	}
}

std::auto_ptr< PlugInResult > PlugInURLTitle::onConnect(state_t _state) {
	return std::auto_ptr< PlugInResult >(NULL);
}

std::auto_ptr< PlugInResult > PlugInURLTitle::onDisconnect(state_t _state) {
	return std::auto_ptr< PlugInResult >(NULL);
}

std::auto_ptr< PlugInResult > PlugInURLTitle::onPacket(state_t _state, IRC::Message const &_message) {
	std::auto_ptr< PlugInResult > result(NULL);
	try {
		IRC::MessagePrivMsg const &privMessage = dynamic_cast< IRC::MessagePrivMsg const &>(_message);
		std::string const &messageText = privMessage.getMessage();
		std::string::size_type urlPos = messageText.find("http://");
		if(urlPos == std::string::npos) {
			urlPos = messageText.find("ftp://");
		}
		if(urlPos != std::string::npos) {
			result = std::auto_ptr< PlugInResult >(new PlugInResult);
			std::string::size_type urlPosEnd = messageText.find_first_of(whitespace, urlPos);
			std::string url = messageText.substr(urlPos, urlPosEnd - urlPos);
			// TODO: Refactor curl -> tidy -> expat into methods!
			// fetch site with curl:
			std::cerr << "PlugInURLTitle: Setting up curl and the write callbacks to this plug-in ..." << std::endl;
			std::auto_ptr< char > curlErrorBuffer = std::auto_ptr< char >(new char[CURL_ERROR_SIZE]);
			CURL *curlHandle = curl_easy_init();
			if(curlHandle == NULL) {
				throw std::runtime_error("PlugInURLTitle: curl_easy_init() returned NULL.");
			}
			curl_easy_setopt(curlHandle, CURLOPT_URL, url.c_str());
			curl_easy_setopt(curlHandle, CURLOPT_VERBOSE, 1);
			curl_easy_setopt(curlHandle, CURLOPT_ERRORBUFFER, curlErrorBuffer.get());
			// Set up write callback over global callback wrapper:
			PlugInURLTitlePtrStorage storage;
			storage.plugIn = this;
			curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, &curlGlobalWriteDataCallback);
			curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &storage);
			std::cerr << "PlugInURLTitle: Fetching URL '" << url << "':" << std::endl;
			if(curl_easy_perform(curlHandle) != 0) {
				// TODO: Handle errors here! We want to continue when we reached buffer limit, but abort otherwise!
				//throw std::runtime_error(std::string("PlugInURLTitle: curl_easy_perform() failed: ") + curlErrorBuffer.get());
			}
			curl_easy_cleanup(curlHandle);
			std::cerr << "---------------------------------------- curl result ----------------------------------------" << std::endl;
			std::cerr << curlBuffer << std::endl;
			std::cerr << "---------------------------------------- curl result end ------------------------------------" << std::endl;
			// tidy up the document:
			// try to create valid XHTML document for XML parser:
			TidyBuffer outputBuffer = { NULL };
			TidyBuffer errorBuffer = { NULL };
			TidyDoc tidyDocument = tidyCreate();
			int tidyResult = -1;
			if(tidyOptSetBool(tidyDocument, TidyXhtmlOut, yes)) {
				tidyResult = tidySetErrorBuffer(tidyDocument, &errorBuffer);
			}
			if(tidyResult >= 0) {
				tidyResult = tidyParseString(tidyDocument, curlBuffer.c_str());
			}
			if(tidyResult >= 0) {
				tidyResult = tidyCleanAndRepair(tidyDocument);
			}
			if(tidyResult >= 0) {
				tidyResult = tidyRunDiagnostics(tidyDocument);
			}
			if(tidyResult > 1) {
				if(!tidyOptSetBool(tidyDocument, TidyForceOutput, yes)) {
					tidyResult = -1;
				}
			}
			if(tidyResult >= 0) {
				tidyResult = tidySaveBuffer(tidyDocument, &outputBuffer);
			}
			if(tidyResult > 0) {
				std::cerr << "PlugInURLTitle: Diagnostics of libtidy:" << std::endl;
				std::cerr << errorBuffer.bp;
			}
			else if(tidyResult < 0) {
				std::stringstream sstrTidyResult;
				sstrTidyResult << tidyResult;
				throw std::runtime_error("A severe error occured while tidying up the received document (" + sstrTidyResult.str() + ").");
			}
			curlBuffer.clear();
			std::cerr << "---------------------------------------- tidy result ----------------------------------------" << std::endl;
			std::cerr << outputBuffer.bp << std::endl;
			std::cerr << "---------------------------------------- tidy result end ------------------------------------" << std::endl;
			//std::string xhtmlDocumentStr(static_cast< char * >(outputBuffer.bp));
			std::cerr << "PlugInURLTitle: Fetching done." << std::endl;
			result->messages.push_back(new IRC::MessagePrivMsg(privMessage.getReceiver(), "Found URL: " + url));
		}
	}
	catch(std::bad_cast) {}
	return result;
}

std::auto_ptr< PlugInResult > PlugInURLTitle::onTimeTrigger(state_t _state) {
	return std::auto_ptr< PlugInResult >(NULL);
}

size_t PlugInURLTitle::curlWriteDataCallback(void *_data, size_t _size, size_t _nmemb) {
	size_t writeSize = 0;
	if((curlBuffer.size() + (_size * _nmemb)) > curlMaxBufferSize) {
		writeSize = curlMaxBufferSize - curlBuffer.size();
	}
	else {
		writeSize = _size * _nmemb;
	}
	curlBuffer.append(static_cast< char * > (_data), writeSize);
	std::cerr << "PlugInURLTitle: Callback method was called and wrote " << writeSize << " bytes to internal buffer." << std::endl;
	return writeSize;
}

}