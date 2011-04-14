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

#include <cctype>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <typeinfo>

#include "urltitle.hpp"

namespace zerobot {

namespace urltitle {

size_t curlWriteDataCallback(void *_data, size_t _size, size_t _nmemb, void *_plugIn) {
	CurlHTMLDownloader *downloader = static_cast< CurlHTMLDownloader * >(_plugIn);
	return downloader->writeDataCallback(_data, _size, _nmemb);
}

void expatStartElementHandler(void *_parser, const XML_Char *_name, const XML_Char **_attributes) {
	ExpatHTMLTitleParser *parser = static_cast< ExpatHTMLTitleParser * >(_parser);
	parser->startElementHandler(_name, _attributes);
}

void expatEndElementHandler(void *_parser, const XML_Char *_name) {
	ExpatHTMLTitleParser *parser = static_cast< ExpatHTMLTitleParser * >(_parser);
	parser->endElementHandler(_name);
}

void expatCharacterDataHandler(void *_parser, const XML_Char *_str, int _length) {
	ExpatHTMLTitleParser *parser = static_cast< ExpatHTMLTitleParser * >(_parser);
	parser->characterDataHandler(_str, _length);
}

bool CurlHTMLDownloader::curlInitialized = false;
unsigned int CurlHTMLDownloader::MAX_BUFFER_SIZE = 10 * 1024;

CurlHTMLDownloader::CurlHTMLDownloader(std::string const &_url) throw(std::runtime_error) {
	url = _url;
	errorBuffer = NULL;
	if(!curlInitialized) {
		curl_global_init(CURL_GLOBAL_ALL);
		atexit(&curl_global_cleanup);
		curlInitialized = true;
	}
	std::cerr << "CurlHTMLDownloader: Using libcurl version: " << curl_version() << std::endl;
	std::cerr << "CurlHTMLDownloader: Reserving " << MAX_BUFFER_SIZE << " bytes for buffer." << std::endl;
	buffer.reserve(MAX_BUFFER_SIZE);

	std::cerr << "CurlHTMLDownloader: Setting up curl and the write callbacks ..." << std::endl;
	errorBuffer = new char[CURL_ERROR_SIZE];
	handle = curl_easy_init();
	if(handle == NULL) {
		delete[] errorBuffer;
		throw std::runtime_error("CurlHTMLDownloader: curl_easy_init() returned NULL.");
	}
	curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
	headers = NULL;
	headers = curl_slist_append(headers, "Accept: text/html");
	// Set up options:
	curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1);
	//curl_easy_setopt(handle, CURLOPT_VERBOSE, 1);
	curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, errorBuffer);
	// Set up write callback over global callback wrapper:
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, &urltitle::curlWriteDataCallback);
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, this);
}

CurlHTMLDownloader::~CurlHTMLDownloader() {
	curl_slist_free_all(headers);
	curl_easy_cleanup(handle);
	delete[] errorBuffer;
/*	if(curlInitialized) {
		curl_global_cleanup();
		curlInitialized = false;
	}
*/
}

void CurlHTMLDownloader::perform() {
	std::cerr << "CurlHTMLDownloader: Fetching URL '" << url << "' ..." << std::endl;
	if(curl_easy_perform(handle) != 0) {
		// TODO: Handle errors here in a better way! We want to continue when we reached buffer limit, but abort otherwise!
		std::cerr << "CurlHTMLDownloader: libcurl reported the following errors:" << std::endl;
		std::cerr << errorBuffer << std::endl;
		//throw std::runtime_error(std::string("CurlHTMLDownloader: curl_easy_perform() failed: ") + curlErrorBuffer.get());
	}
	std::cerr << "... done." << std::endl;
}

size_t CurlHTMLDownloader::writeDataCallback(void *_data, size_t _size, size_t _nmemb) {
	size_t writeSize = 0;
	if((buffer.size() + (_size * _nmemb)) > MAX_BUFFER_SIZE) {
		writeSize = MAX_BUFFER_SIZE - buffer.size();
	}
	else {
		writeSize = _size * _nmemb;
	}
	buffer.append(static_cast< char * > (_data), writeSize);
	//std::cerr << "CurlHTMLDownloader: Callback method was called and wrote " << writeSize << " bytes to internal buffer." << std::endl;
	return writeSize;
}

HTMLTidy::HTMLTidy(std::string const &_document) : document(_document) {
	handle = tidyCreate();
}

HTMLTidy::~HTMLTidy() {
	tidyRelease(handle);
}

void HTMLTidy::run() throw(std::runtime_error) {
	TidyBuffer outputBuffer = { 0 };
	TidyBuffer errorBuffer = { 0 };
	// try to create valid XHTML document for XML parser:
	int tidyResult = -1;
	if(tidyOptSetBool(handle, TidyXhtmlOut, yes)) {
		tidyResult = tidySetErrorBuffer(handle, &errorBuffer);
	}
	if(tidyResult >= 0) {
		tidyResult = tidyParseString(handle, document.c_str());
	}
	if(tidyResult >= 0) {
		tidyResult = tidyCleanAndRepair(handle);
	}
	if(tidyResult >= 0) {
		tidyResult = tidyRunDiagnostics(handle);
	}
	if(tidyResult > 1) {
		if(!tidyOptSetBool(handle, TidyForceOutput, yes)) {
			tidyResult = -1;
		}
	}
	if(tidyResult >= 0) {
		tidyResult = tidySaveBuffer(handle, &outputBuffer);
	}
	if(tidyResult > 0) {
		std::cerr << "HTMLTidy: Diagnostics of libtidy:" << std::endl;
		std::cerr << errorBuffer.bp;
	}
	else if(tidyResult < 0) {
		std::stringstream sstrTidyResult;
		sstrTidyResult << tidyResult;
		throw std::runtime_error("HTMLTidy: A severe error occured while tidying up the received document (" + sstrTidyResult.str() + ").");
	}
	resultDocument.reserve(outputBuffer.size); // avoid frequent (re-)allocations
	for(unsigned int i = 0; i < outputBuffer.size; i++) {
		resultDocument.insert(resultDocument.end(), static_cast< char >(*(outputBuffer.bp + i)));
	}
	tidyBufFree(&outputBuffer);
	tidyBufFree(&errorBuffer);
}


ExpatHTMLTitleParser::ExpatHTMLTitleParser(std::string const &_document) : document(_document) {
	inTitle = false;
	expatParser = XML_ParserCreate(NULL);
	XML_SetElementHandler(expatParser, &expatStartElementHandler, &expatEndElementHandler);
	XML_SetCharacterDataHandler(expatParser, &expatCharacterDataHandler);
	XML_SetUserData(expatParser, this);
}

ExpatHTMLTitleParser::~ExpatHTMLTitleParser() {
	XML_ParserFree(expatParser);
}

void ExpatHTMLTitleParser::parse() throw (std::runtime_error) {
	if(XML_Parse(expatParser, document.c_str(), document.size(), true) != XML_STATUS_OK) {
		std::stringstream sstrErrorCode;
		sstrErrorCode << XML_GetErrorCode(expatParser);
		throw std::runtime_error("ExpatHTMLTitleParser: expat error " + sstrErrorCode.str() + ": " + XML_ErrorString(XML_GetErrorCode(expatParser)));
	}
	canonicalizeTitle();
}

void ExpatHTMLTitleParser::startElementHandler(const XML_Char *_name, const XML_Char **_attributes) {
	if(strcmp(_name, "title") == 0) {
		inTitle = true;
	}
}

void ExpatHTMLTitleParser::endElementHandler(const XML_Char *_name) {
	if(strcmp(_name, "title") == 0) {
		inTitle = false;
	}
}

void ExpatHTMLTitleParser::characterDataHandler(const XML_Char *_str, int _length) {
	if(inTitle) {
		title.append(_str, _length);
	}
}

void ExpatHTMLTitleParser::canonicalizeTitle() {
	for(std::string::size_type i = 0; i < title.size(); i++) {
		if(title.at(i) == '\r' || title.at(i) == '\n') {
			title.replace(i, 1, 1, ' ');
		}
	}
	for(std::string::iterator it = title.begin(); it != title.end(); ) {
		if(isspace(*it)) {
			if(it != title.begin()) { // do not move over beginning when looking back
				std::string::const_iterator last = it;
				last--;
				if(isspace(*last)) {
					it = title.erase(it);
				}
				else {
					it++;
				}
			}
			else {
				it = title.erase(it);
			}
		}
		else if(iscntrl(*it)) {
			it = title.erase(it);
		}
		else {
			it++;
		}
	}
	if(title.size() > 0 && isspace(title.at(title.size() - 1))) {
		title.erase(title.size() - 1);
	}
}

}

char const *PlugInURLTitle::whitespace = "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f\x7f";

PlugInURLTitle::PlugInURLTitle(int _priority) : PlugIn(_priority, "urltitle") {
}

PlugInURLTitle::~PlugInURLTitle() {
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
			try {
				// fetch site with curl:
				std::auto_ptr< urltitle::CurlHTMLDownloader > curlDownloader = std::auto_ptr< urltitle::CurlHTMLDownloader >(new urltitle::CurlHTMLDownloader(url));
				curlDownloader->perform();
				// tidy up the fetched document and generate valid XHTML, so that the XML parser running afterwards won't fail for crappy HTML:
				std::auto_ptr< urltitle::HTMLTidy > htmlTidy = std::auto_ptr< urltitle::HTMLTidy >(new urltitle::HTMLTidy(curlDownloader->getBuffer()));
				htmlTidy->run();
				urltitle::ExpatHTMLTitleParser titleParser(htmlTidy->getResultDocument());
				titleParser.parse();
				if(titleParser.getTitle() != "") {
					result->messages.push_back(new IRC::MessagePrivMsg(privMessage.getReceiver(), "Title: " + titleParser.getTitle()));
				}
			}
			catch(std::runtime_error e) {
				std::cerr << "PlugInURLTitle: Exception while parsing title: " << e.what() << std::endl;
			}
		}
	}
	catch(std::bad_cast) {}
	return result;
}

std::auto_ptr< PlugInResult > PlugInURLTitle::onTimeTrigger(state_t _state) {
	return std::auto_ptr< PlugInResult >(NULL);
}


}
