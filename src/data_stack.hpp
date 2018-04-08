/** @file */
#ifndef __DATA_STACK_HPP__
#define __DATA_STACK_HPP__

#include "utilities.hpp"

#include <sys/mman.h>
#include <string.h>


// Note: we do not use the grows down flag of mmap
// so that we can easily handle the case where it cannot grow

/** A macro wrapper for consistency without performance cost */
#define MPROTECT_DATA_IF_NEEDED(PROT) if ( protect ) { \
	Utilities::assert( mprotect( data.ptr, data.n, PROT ) == 0, "mprotect() failed." ); \
}

/** A class that holds two internal stack classes 
 *  which share a common implementation. The only
 *  difference is that one mprotects its data */
class DataStack final {

	/** A class just like std::stack, however it is
	 *  capable of protecting it's internal data
	 *  via mprotect! The only reason protect is passed 
	 *  as a boolean is for a compile time optimization ! */
	template<typename data_t, bool protect> 
	class ProtectableDataStack final {

		/** The size of the stack */
		size_t size;
		
		/** The size of the stack */
		size_t msize;
		
		/** A tiny struct containing an array and a size */
		template<typename T> struct alloc_info {
			/** The data pointer */
			T * ptr;
			/** The size of the data allocated */
			size_t n;
		};

		/** The stack data */
		alloc_info<data_t> data;

		/** A function that allocates page aligned memory
		 *  The resulting pointer and size are stores in info
		 *  Allocates n * sizeof(T) contiguous page aligned bytes.
		 *  Ensures that n * sizeof(T) is a multiple of the page size! */
		template <typename T> void page_aligned_alloc(alloc_info<T> & info, const size_t n) {
			static const auto page_size = Utilities::get_page_size();

			// Error checking
			info.n = n * sizeof(T);
			Utilities::log("DataStack allocating ", info.n, " bytes via mmap()...");
			Utilities::assert(	info.n % page_size == 0, 
								"Invalid n passed to page_aligned_alloc()" );

			// Map the memory and check for faliure
			info.ptr = ( T* ) mmap(	nullptr, info.n, PROT_READ | PROT_WRITE,
									MAP_PRIVATE | MAP_ANONYMOUS, -1, 0 );
			Utilities::assert( info.ptr != MAP_FAILED, "mmap() failed." );
		}

		/** A function that frees page aligned memory allocated by page_aligned_alloc */
		template <typename T> void page_aligned_free(alloc_info<T> & info) {
			Utilities::assert( munmap( info.ptr, info.n) != 0, "munmap() failed." );
		}

	public:

		/** The constructor */
		ProtectableDataStack() : size(0), msize(Utilities::get_page_size()) {
			page_aligned_alloc<data_t> ( data, msize );
			MPROTECT_DATA_IF_NEEDED( PROT_READ );
		}

		/** Disable other constructors */
		ProtectableDataStack & operator= ( const ProtectableDataStack & ) = default;
		/** Disable other constructors */
		ProtectableDataStack( const ProtectableDataStack & ) = delete;
		/** Disable other constructors */
		ProtectableDataStack( ProtectableDataStack && ) = delete;

		/** Push v onto the stack by copy */
		void push( const data_t v ) {

			// Unprotect the data if needed
			const bool resize = (size == msize);
			const int flags = PROT_WRITE | ( resize ? PROT_READ : 0 );
			MPROTECT_DATA_IF_NEEDED( flags );

			// If the stack size has to be increased, do so
			if ( resize ) {
				msize *= 2;

				// Allocate the new memory
				alloc_info<data_t> old(data);
				page_aligned_alloc<data_t> ( data, msize );

				// Copy the data over
				Utilities::log("Stack copying data into new allocation");
				memcpy( data.ptr, old.ptr, old.n );
				page_aligned_free<data_t> ( old );
			}

			// Add v to the stack
			data.ptr[size] = v;
			size += 1;
			
			// Protect the data again if needed
			MPROTECT_DATA_IF_NEEDED( PROT_READ )
		}

		/** Pops the stack */
		void pop() noexcept {
			size -= 1;
		}

		/** Return the top element of the stack by copy */
		const data_t top() const noexcept {
			return data.ptr[size - 1];
		}

		/** Returns true if size == 0*/
		size_t empty() const noexcept {
			return (size == 0);
		}
	};

public:

	/** Expose the non-protected stack */
	template<typename T> using Stack = ProtectableDataStack<T, false>;

	/** Expose the protected stack */
	template<typename T> using ProtectedStack = ProtectableDataStack<T, true>;
};


/** Protect the global namespace */
#undef MPROTECT_DATA_IF_NEEDED


#endif
