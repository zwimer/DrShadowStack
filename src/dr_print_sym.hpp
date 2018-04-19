/** @file */
#ifndef __DR_PRINT_SYM_HPP__
#define __DR_PRINT_SYM_HPP__

#include "dr_api.h"


/** Wrap symbols in a static class */
struct Sym {
  private:
	/** For clarity of the dependency injection below */
	using PrintFn = void ( * )( const char *const format, ... );

	/** Record if syms is already setup */
	static bool setup;

  public:
	/** Disable construction */
	Sym() = delete;

	/** The setup function for dr_print_sym
	 *  Must be called *by the DR client* before print */
	static void init();

	/** This function should be called when the client terminates */
	static void finish();

	/** Print symbol information for the what is located at addr
	 *  description is a description of what the address addr points to */
	static void print( const char *const description, const app_pc addr );
};

#endif
