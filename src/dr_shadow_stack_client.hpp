/** @file */
#ifndef __DR_SHADOW_STACK_CLIENT_HPP__
#define __DR_SHADOW_STACK_CLIENT_HPP__


#include "dr_api.h"


/** A class used to contain mode specific function definitions
 * Specifically, the event handlers this client must used */
class SSHandlers final {
  private:
	/** The type 'on call' funciton signature */
	typedef void ( *const on_call_signature )( const app_pc ret_to_addr );

	/** The type 'on ret' funciton signature */
	typedef void ( *const on_ret_signature )( const app_pc instr_addr,
	                                          const app_pc target_addr );

	/** The type 'on signal' funciton signature */
	typedef void ( *const on_signal_signature )();

  public:
	/** Delete default constructor */
	SSHandlers() = delete;

	/** Constructor */
	SSHandlers( const on_call_signature c, const on_ret_signature r,
	            const on_signal_signature s );

	/** The 'on call' handler */
	const on_call_signature on_call;

	/** The 'on ret' handler */
	const on_ret_signature on_ret;

	/** The function called whenever a signal is caught */
	const on_signal_signature on_signal;

	/** Returns true if all function pointers are non-null */
	bool is_valid() const;
};

/** A static class used to rapidly get the tid of the current thread
 *  This class will store the result of a gettid call in
 *  thread local storage, avoiding the overhead of calling */
class QuickTID {
  public:
	/** Called to setup the class */
	static void setup();

	/** Gets the tid of the current thread quickly
	 *  Records if in tls if it has yet to be seen */
	static pid_t fetch( void *drcontext );

  private:
	/** True if setup was called, false otherwise */
	static bool is_setup;

	/** The index of tls the local tids are stored at */
	static int tls_index;
};


#endif
