/*	This file is part of zerobot.

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

#ifndef PLUGINS_URLTITLE_HPP
#define PLUGINS_URLTITLE_HPP

#include <buffio.h>
#include <curl/curl.h>
#include <expat.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <tidy.h>

#include "base.hpp"

namespace zerobot {

namespace urltitle {

size_t curlWriteDataCallback(void *_data, size_t _size, size_t _nmemb, void *_plugIn);
void expatStartElementHandler(void *_parser, const XML_Char * _name, const XML_Char **_attributes);
void expatEndElementHandler(void *_parser, const XML_Char *_name);
void expatCharacterDataHandler(void *_parser, const XML_Char *_str, int _length);

class CurlHTMLDownloader {
	public:
		CurlHTMLDownloader(std::string const &_url) throw(std::runtime_error);
		~CurlHTMLDownloader();

		std::string const &getBuffer() const { return buffer; }

		void perform();

		size_t writeDataCallback(void *_data, size_t _size, size_t _nmemb);

	private:
		static unsigned int MAX_BUFFER_SIZE;

		static bool curlInitialized;

		std::string url;
		std::string buffer;
		char *errorBuffer;
		CURL *handle;
		struct curl_slist *headers;
};

class HTMLTidy {
	public:
		HTMLTidy(std::string const &_document);
		~HTMLTidy();

		std::string const &getResultDocument() const { return resultDocument; }

		void run() throw(std::runtime_error);

	private:
		std::string const &document;
		std::string resultDocument;

		TidyDoc handle;
};

class ExpatHTMLTitleParser {
	public:
		ExpatHTMLTitleParser(std::string const &_document);
		~ExpatHTMLTitleParser();

		std::string const &getTitle() const { return title; }

		void parse() throw(std::runtime_error);

		void startElementHandler(const XML_Char *_name, const XML_Char **_attributes);
		void endElementHandler(const XML_Char *_name);
		void characterDataHandler(const XML_Char *_str, int _length);

	private:
		XML_Parser expatParser;

		std::string const &document;
		bool inTitle;
		std::string title;

		void canonicalizeTitle();
};

}

class PlugInURLTitle : public PlugIn {
	public:
		PlugInURLTitle(int _priority);
		virtual ~PlugInURLTitle();

		virtual std::auto_ptr< PlugInResult > onConnect(state_t _state);
		virtual std::auto_ptr< PlugInResult > onPacket(state_t _state, IRC::Message const &_message);
		virtual std::auto_ptr< PlugInResult > onTimeTrigger(state_t _state);
		virtual std::auto_ptr< PlugInResult > onDisconnect(state_t _state);

	protected:
		static char const *whitespace;
};

}

#endif
