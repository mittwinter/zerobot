#pragma once
#ifndef SECRET_STORAGE_HPP
#define SECRET_STORAGE_HPP

#include <stddef.h> // size_t
#include <stdexcept>

namespace memory {

class SecretStorage {
	public:
		SecretStorage( size_t size ) throw( std::bad_alloc, std::runtime_error );
		virtual ~SecretStorage();

		size_t getSize() const { return size; }

		void set( void const *src, size_t num ) throw( std::runtime_error );
		void const *get() { return mem; }
		void reset();

	protected:
		static bool initialized;
		static bool mlockFailed;
		size_t size;
		void *mem;

		static void init() throw( std::runtime_error );
};

}

#endif

