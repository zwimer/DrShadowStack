#include "dr_shadow_stack_client.hpp"
#include "dr_external_ss_events.hpp"
#include "quick_socket.hpp"
#include "utilities.hpp"
#include "message.hpp"


// The socket to be used by this client
static int sock = -1;


// The call handler. 
// This function is called whenever a call instruction is about 
// to execute. This function is static for optimization reasons */
static void on_call(const app_pc ret_to_addr) {
	Utilities::verbose_log("(client) Call @ ", (void *) ret_to_addr, " - 0x5");
	send_msg<Message::Call>(sock, (char *) & ret_to_addr );
}

// The ret handler. 
// This function is called whenever a ret instruction is about 
// to execute. This function is static for optimization reasons */
static void on_ret(const app_pc instr_addr, const app_pc target_addr) {
	Utilities::verbose_log("(client) Ret to ", (void *) target_addr);
	send_msg<Message::Ret>(sock, (char *) & target_addr );
	(void) recv_msg<Message::Continue>(sock);
}

// Called whenever a signal is called. Adds a wildcard to the shadow stack
// Note: the reason we use this instead of the signal event is this ignores ignored signals
static void on_signal() {
	send_msg<Message::NewSignal>(sock);
}


/*********************************************************/
/*                                                       */
/*                  	 From Header					 */
/*                                                       */
/*********************************************************/


// Setup the external stack server for the DynamoRIO client
void ExternalSS::setup(SSHandlers ** const handlers, const char * const socket_path) {
	*handlers = new SSHandlers(on_call, on_ret, on_signal);
	Utilities::log("Client connecting to ", socket_path);
	sock = QS::create_client(socket_path);
}
