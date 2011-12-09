#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <iostream>
#include <sstream>
// mlock():
#include <sys/mman.h>
// get-/setrlimit():
#include <sys/time.h>
#include <sys/resource.h>

#include "SecretStorage.hpp"

namespace memory {

bool SecretStorage::initialized = false;
bool SecretStorage::mlockFailed = false;

SecretStorage::SecretStorage( size_t size ) throw( std::bad_alloc, std::runtime_error )
		: size( size ), mem( NULL ) {
	init();
	mem = malloc( size );
	if( mem != NULL ) {
		int retVal = mlock( mem, size );
		if( retVal != 0 && !mlockFailed ) {
			//throw std::runtime_error( std::string( "SecretStorage::SecretStorage(): Failed to lock memory via mlock(...):\n\t" ) + strerror( errno ) );
			std::cerr << "SecretStorage: WARNING: Using insecure memory!" << std::endl
			          << "SecretStorage: Failed to lock memory via mlock(...):" << std::endl
			          << "\t" << strerror( errno ) << std::endl;
			mlockFailed = true;
		}
		std::clog << "SecretStorage: " << getSize() << " bytes of secure memory allocated." << std::endl;
	}
	else {
		std::stringstream sstrSize; // "Allocate" this stringstream before call to malloc() as memory may be exhausted here?
		sstrSize << size;
		std::cerr << "SecretStorage: Failed to allocate " + sstrSize.str() + " bytes via malloc()." << std::endl;
		throw std::bad_alloc();
	}
}

SecretStorage::~SecretStorage() {
	reset();
	free( mem );
	mem = NULL;
}

void SecretStorage::set( void const *src, size_t num ) throw( std::runtime_error ) {
	if( num > size ) {
		std::stringstream sstrNum;
		sstrNum << num;
		std::stringstream sstrSize;
		sstrSize << size;
		throw std::runtime_error( "SecretStorage: Failed to set " + sstrNum.str() + " bytes in storage of size " + sstrSize.str() + " bytes." );
	}
	std::clog << "SecretStorage::set(): Copying " << num << " bytes to secure memory..." << std::endl;
	memcpy( mem, src, num );
}

void SecretStorage::reset() {
	memset( mem, 0, size );
}

void SecretStorage::init() throw( std::runtime_error ) {
	if( !initialized ) {
		std::clog << "SecretStorage::init(): Disabling core dumps for this process..." << std::endl;
		// Disable core dumps for this process:
		struct rlimit rlim;
		getrlimit( RLIMIT_CORE, &rlim );
		rlim.rlim_cur = 0;
		rlim.rlim_max = 0;
		int retVal = setrlimit( RLIMIT_CORE, &rlim );
		if( retVal != 0 ) {
			throw std::runtime_error( std::string( "SecretStorage::init(): Failed to setrlimit( RLIMIT_CORE, ... ) to disable core dumps:\n\t" ) + strerror( errno ) );
		}
	}
}

}

