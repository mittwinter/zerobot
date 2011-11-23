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

size_t curlWriteDataCallback( void *data, size_t size, size_t nmemb, void *userData ) {
	CurlHTMLDownloader *downloader = static_cast< CurlHTMLDownloader * >( userData );
	return downloader->writeDataCallback( data, size, nmemb );
}

void expatStartElementHandler( void *userData, XML_Char const *name, XML_Char const **attributes ) {
	ExpatHTMLTitleParser *parser = static_cast< ExpatHTMLTitleParser * >( userData );
	parser->startElementHandler( name, attributes );
}

void expatEndElementHandler( void *userData, XML_Char const *name ) {
	ExpatHTMLTitleParser *parser = static_cast< ExpatHTMLTitleParser * >( userData );
	parser->endElementHandler( name );
}

void expatCharacterDataHandler( void *userData, XML_Char const *str, int length ) {
	ExpatHTMLTitleParser *parser = static_cast< ExpatHTMLTitleParser * >( userData );
	parser->characterDataHandler( str, length );
}

bool CurlHTMLDownloader::curlInitialized = false;
unsigned int CurlHTMLDownloader::MAX_BUFFER_SIZE = 10 * 1024;

CurlHTMLDownloader::CurlHTMLDownloader( std::string const &url ) throw( std::runtime_error ) : url( url ), errorBuffer( NULL ) {
	if( !curlInitialized ) {
		curl_global_init( CURL_GLOBAL_ALL );
		atexit( &curl_global_cleanup );
		curlInitialized = true;
	}
	std::clog << "CurlHTMLDownloader: Using libcurl version: " << curl_version() << std::endl;
	std::clog << "CurlHTMLDownloader: Reserving " << MAX_BUFFER_SIZE << " bytes for buffer." << std::endl;
	buffer.reserve( MAX_BUFFER_SIZE );

	std::clog << "CurlHTMLDownloader: Setting up curl and the write callbacks..." << std::endl;
	errorBuffer = new char[ CURL_ERROR_SIZE ];
	handle = curl_easy_init();
	if( handle == NULL ) {
		delete[] errorBuffer;
		throw std::runtime_error( "CurlHTMLDownloader: curl_easy_init() returned NULL." );
	}
	// Set up options:
	// - URL:
	curl_easy_setopt( handle, CURLOPT_URL, url.c_str() );
	// - our headers (only accept text/html documents):
	headers = NULL;
	headers = curl_slist_append( headers, "Accept: text/html" );
	curl_easy_setopt( handle, CURLOPT_HTTPHEADER, headers );
	// - follow Location: header:
	curl_easy_setopt( handle, CURLOPT_FOLLOWLOCATION, 1 );
	// - be verbose:
	//curl_easy_setopt( handle, CURLOPT_VERBOSE, 1 );
	// - error buffer:
	curl_easy_setopt( handle, CURLOPT_ERRORBUFFER, errorBuffer );
	// - set up write callback over global callback wrapper:
	curl_easy_setopt( handle, CURLOPT_WRITEFUNCTION, &urltitle::curlWriteDataCallback );
	curl_easy_setopt( handle, CURLOPT_WRITEDATA, this );
}

CurlHTMLDownloader::~CurlHTMLDownloader() {
	curl_slist_free_all( headers );
	curl_easy_cleanup( handle );
	delete[] errorBuffer;
/*	if( curlInitialized ) {
		curl_global_cleanup();
		curlInitialized = false;
	}
*/
}

void CurlHTMLDownloader::perform() {
	std::clog << "CurlHTMLDownloader: Fetching URL '" << url << "' (at most " << MAX_BUFFER_SIZE << " bytes)..."
	          << std::endl;
	if( curl_easy_perform( handle ) != 0 ) {
		// TODO: Handle errors here in a better way!
		//  We want to continue when we reached the buffer limit, but abort otherwise!
		std::cerr << "CurlHTMLDownloader: \t **********************************************************"
		          << std::endl;
		std::cerr << "CurlHTMLDownloader: \t libcurl reported the following errors:"
		          << std::endl;
		std::cerr << "CurlHTMLDownloader: \t " << errorBuffer
		          << std::endl;
		std::cerr << "CurlHTMLDownloader: \t **********************************************************"
		          << std::endl;
		//throw std::runtime_error( std::string( "CurlHTMLDownloader: curl_easy_perform() failed: " ) + curlErrorBuffer.get() );
	}
	std::clog << "CurlHTMLDownloader: ... done. [used buffer size: " << buffer.size() << " bytes] " << std::endl;
}

size_t CurlHTMLDownloader::writeDataCallback( void *data, size_t size, size_t nmemb ) {
	size_t writeSize = 0;
	if( (buffer.size() + (size * nmemb)) > MAX_BUFFER_SIZE ) {
		writeSize = MAX_BUFFER_SIZE - buffer.size();
	}
	else {
		writeSize = size * nmemb;
	}
	buffer.append( static_cast< char * > ( data ), writeSize );
	std::clog << "CurlHTMLDownloader: \t writeDataCallback() method was called and wrote "
	          << writeSize << " bytes to internal buffer." << std::endl;
	return writeSize;
}

HTMLTidy::HTMLTidy( std::string const &document ) : document( document ) {
	handle = tidyCreate();
}

HTMLTidy::~HTMLTidy() {
	tidyRelease( handle );
}

void HTMLTidy::run() throw( std::runtime_error ) {
	TidyBuffer outputBuffer = { 0 };
	TidyBuffer errorBuffer = { 0 };
	// try to create valid XHTML document for XML parser:
	int tidyResult = -1;
	if( tidyOptSetBool( handle, TidyXhtmlOut, yes ) ) {
		tidyResult = tidySetErrorBuffer( handle, &errorBuffer );
	}
	if( tidyResult >= 0 ) {
		tidyResult = tidyParseString( handle, document.c_str() );
	}
	if( tidyResult >= 0 ) {
		tidyResult = tidyCleanAndRepair( handle );
	}
	if( tidyResult >= 0 ) {
		tidyResult = tidyRunDiagnostics( handle );
	}
	if( tidyResult > 1 ) {
		if( !tidyOptSetBool( handle, TidyForceOutput, yes ) ) {
			tidyResult = -1;
		}
	}
	if( tidyResult >= 0 ) {
		tidyResult = tidySaveBuffer( handle, &outputBuffer );
	}
	if( tidyResult > 0 ) {
		std::clog << "*********************************" << std::endl;
		std::clog << "HTMLTidy: Diagnostics of libtidy:" << std::endl;
		std::clog << errorBuffer.bp;
		std::clog << "*********************************" << std::endl;
	}
	else if( tidyResult < 0 ) {
		std::stringstream sstrTidyResult;
		sstrTidyResult << tidyResult;
		throw std::runtime_error( "HTMLTidy: A severe error occured while tidying up the received document ("
		                          + sstrTidyResult.str()
		                          + ")."
		                        );
	}
	resultDocument.reserve( outputBuffer.size ); // avoid frequent (re-)allocations
	for( unsigned int i = 0; i < outputBuffer.size; i++ ) {
		resultDocument.insert( resultDocument.end(), static_cast< char >( *(outputBuffer.bp + i) ) );
	}
	tidyBufFree( &outputBuffer );
	tidyBufFree( &errorBuffer );
}

ExpatHTMLTitleParser::ExpatHTMLTitleParser( std::string const &document)
		: expatParser( XML_ParserCreate( NULL ) )
		, document( document )
		, inTitle( false )
		{
	// Set-up expat callbacks via global callback wrappers to this class:
	XML_SetElementHandler( expatParser, &expatStartElementHandler, &expatEndElementHandler );
	XML_SetCharacterDataHandler( expatParser, &expatCharacterDataHandler );
	XML_SetUserData( expatParser, this );
}

ExpatHTMLTitleParser::~ExpatHTMLTitleParser() {
	XML_ParserFree( expatParser );
}

void ExpatHTMLTitleParser::parse() throw( std::runtime_error ) {
	if( XML_Parse( expatParser, document.c_str(), document.size(), true ) != XML_STATUS_OK) {
		std::stringstream sstrErrorCode;
		sstrErrorCode << XML_GetErrorCode( expatParser );
		throw std::runtime_error( "ExpatHTMLTitleParser: expat error "
		                          + sstrErrorCode.str()
		                          + ": "
		                          + XML_ErrorString( XML_GetErrorCode( expatParser ) )
		                        );
	}
	canonicalizeTitle();
}

void ExpatHTMLTitleParser::startElementHandler( XML_Char const *name, XML_Char const **attributes ) {
	if( strcmp( name, "title" ) == 0 ) {
		inTitle = true;
	}
}

void ExpatHTMLTitleParser::endElementHandler( XML_Char const *name ) {
	if( strcmp( name, "title" ) == 0 ) {
		inTitle = false;
	}
}

void ExpatHTMLTitleParser::characterDataHandler( XML_Char const *str, int length ) {
	if( inTitle ) {
		title.append( str, length );
	}
}

void ExpatHTMLTitleParser::canonicalizeTitle() {
	for( std::string::size_type i = 0; i < title.size(); i++ ) {
		if( title.at( i ) == '\r' || title.at( i ) == '\n' ) {
			title.replace(i, 1, 1, ' ');
		}
	}
	for( std::string::iterator it = title.begin(); it != title.end(); ) {
		if( isspace( *it ) ) {
			if( it != title.begin() ) { // do not move over beginning when looking back
				std::string::const_iterator last = it;
				last--;
				if( isspace( *last ) ) {
					it = title.erase( it );
				}
				else {
					it++;
				}
			}
			else {
				it = title.erase( it );
			}
		}
		else if( iscntrl( *it ) ) {
			it = title.erase( it );
		}
		else {
			it++;
		}
	}
	if(title.size() > 0 && isspace( title.at( title.size() - 1 ) ) ) {
		title.erase( title.size() - 1 );
	}
}

}

char const *PlugInURLTitle::whitespace = "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f\x7f";

PlugInURLTitle::PlugInURLTitle( int priority ) : PlugIn( priority, "urltitle" ) {
}

PlugInURLTitle::~PlugInURLTitle() {
}

std::auto_ptr< PlugInResult > PlugInURLTitle::onConnect( state_t state ) {
	return std::auto_ptr< PlugInResult >( NULL );
}

std::auto_ptr< PlugInResult > PlugInURLTitle::onDisconnect( state_t state ) {
	return std::auto_ptr< PlugInResult >( NULL );
}

std::auto_ptr< PlugInResult > PlugInURLTitle::onPacket( state_t state, IRC::Message const &message ) {
	std::auto_ptr< PlugInResult > result( NULL );
	try {
		IRC::MessagePrivMsg const &privMessage = dynamic_cast< IRC::MessagePrivMsg const & >( message );
		std::string const &messageText = privMessage.getMessage();
		std::string::size_type urlIndicatorPos = messageText.find( "://" );
		if( urlIndicatorPos != std::string::npos ) {
			std::string::size_type urlPosBegin = messageText.find_last_of( whitespace, urlIndicatorPos );
			std::string::size_type urlPosEnd = messageText.find_first_of( whitespace, urlIndicatorPos );
			std::string url = messageText.substr( urlPosBegin + 1, urlPosEnd - (urlPosBegin + 1) );
			std::clog << "PlugInURLTitle: Fetching title for '" << url << "'..." << std::endl;
			try {
				// * Fetch site with CurlHTMLDownloader:
				std::auto_ptr< urltitle::CurlHTMLDownloader > curlDownloader = std::auto_ptr< urltitle::CurlHTMLDownloader >( new urltitle::CurlHTMLDownloader( url ) );
				curlDownloader->perform();
				// * Tidy up the fetched document and generate valid XHTML,
				//    so that the XML parser running afterwards won't fail due to crappy HTML:
				std::auto_ptr< urltitle::HTMLTidy > htmlTidy = std::auto_ptr< urltitle::HTMLTidy >( new urltitle::HTMLTidy( curlDownloader->getBuffer() ) );
				htmlTidy->run();
				// * Parse the tidied document with ExpatHTMLTitleParser,
				//    which parses the title of an XHTML document:
				std::auto_ptr< urltitle::ExpatHTMLTitleParser > titleParser = std::auto_ptr< urltitle::ExpatHTMLTitleParser >( new urltitle::ExpatHTMLTitleParser( htmlTidy->getResultDocument() ) );
				titleParser->parse();
				// If title is not empty:
				if( titleParser->getTitle() != "") {
					std::clog << "PlugInURLTitle: ... obtained title: '" << titleParser->getTitle() << "'." << std::endl;
					// * Send message containing the title of the initial URL:
					result = std::auto_ptr< PlugInResult >( new PlugInResult );
					result->messages.push_back( new IRC::MessagePrivMsg( privMessage.getReceiver()
					                                                   , "Title: " + titleParser->getTitle()
					                                                   )
					                          );
				}
				else {
					std::clog << "PlugInURLTitle: ... unable to obtain title, ignoring URL." << std::endl;
				}
			}
			catch( std::runtime_error const &e ) {
				std::cerr << "PlugInURLTitle: Exception while parsing title: " << e.what() << std::endl;
			}
		}
	}
	catch( std::bad_cast const & ) {}
	return result;
}

std::auto_ptr< PlugInResult > PlugInURLTitle::onTimeTrigger( state_t state ) {
	return std::auto_ptr< PlugInResult >( NULL );
}

}
