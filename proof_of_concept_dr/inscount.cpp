#include "dr_api.h"
#include "drmgr.h"
#include "droption.h"
#include <string.h>

# define DISPLAY_STRING(msg) dr_printf("%s\n", msg);

#define NULL_TERMINATE(buf) buf[(sizeof(buf)/sizeof(buf[0])) - 1] = '\0'

static droption_t<bool> only_from_app
(DROPTION_SCOPE_CLIENT, "only_from_app", false,
 "Only count app, not lib, instructions",
 "Count only instructions in the application itself, ignoring instructions in "
 "shared libraries.");

/* Application module */
static app_pc exe_start;
/* we only have a global count */
static uint64 global_count;
/* A simple clean call that will be automatically inlined because it has only
 * one argument and contains no calls to other functions.
 */
static void inscount(uint num_instrs) { global_count += num_instrs; }
static void event_exit(void);
static dr_emit_flags_t event_bb_analysis(void *drcontext, void *tag,
                                         instrlist_t *bb,
                                         bool for_trace, bool translating,
                                         void **user_data);
static dr_emit_flags_t event_app_instruction(void *drcontext, void *tag,
                                             instrlist_t *bb, instr_t *inst,
                                             bool for_trace, bool translating,
                                             void *user_data);

DR_EXPORT void dr_client_main(client_id_t id, int argc, const char *argv[])
{
    dr_set_client_name("DynamoRIO Sample Client 'inscount'",
                       "http://dynamorio.org/issues");

    /* Options */
    if (!droption_parser_t::parse_argv(DROPTION_SCOPE_CLIENT, argc, argv, NULL, NULL))
        DR_ASSERT(false);
    drmgr_init();

    /* Get main module address */
    if (only_from_app.get_value()) {
        module_data_t *exe = dr_get_main_module();
        if (exe != NULL)
            exe_start = exe->start;
        dr_free_module_data(exe);
    }

    /* register events */
    dr_register_exit_event(event_exit);
    drmgr_register_bb_instrumentation_event(event_bb_analysis,
                                            event_app_instruction,
                                            NULL);

    /* make it easy to tell, by looking at log file, which client executed */
    dr_log(NULL, DR_LOG_ALL, 1, "Client 'inscount' initializing\n");
}

static void event_exit(void)
{
    char msg[512];
    int len;
    len = dr_snprintf(msg, sizeof(msg)/sizeof(msg[0]),
                      "Instrumentation results: %llu instructions executed\n",
                      global_count);
    DR_ASSERT(len > 0);
    NULL_TERMINATE(msg);
    DISPLAY_STRING(msg);
    drmgr_exit();
}

static dr_emit_flags_t
event_bb_analysis(void *drcontext, void *tag, instrlist_t *bb,
                  bool for_trace, bool translating, void **user_data)
{
    instr_t *instr;
    uint num_instrs;

    /* Only count in app BBs */
    if (only_from_app.get_value()) {
        module_data_t *mod = dr_lookup_module(dr_fragment_app_pc(tag));
        if (mod != NULL) {
            bool from_exe = (mod->start == exe_start);
            dr_free_module_data(mod);
            if (!from_exe) {
                *user_data = NULL;
                return DR_EMIT_DEFAULT;
            }
        }
    }
    /* Count instructions */
    for (instr = instrlist_first_app(bb), num_instrs = 0;
         instr != NULL;
         instr = instr_get_next_app(instr)) {
        num_instrs++;
    }
    *user_data = (void *)(ptr_uint_t)num_instrs;

    return DR_EMIT_DEFAULT;
}




int indt = -97;
static void cll(const void * const xip) {
	if (++indt < 0) return;
	for ( int i = 0; i < indt; ++i ) {
		printf("\t");
	}
	printf("------> %p\n", xip);
}

static void rtt(app_pc instr_addr, app_pc target_addr) {
	if (--indt < -1) return;
	for ( int i = -1; i < indt; ++i ) {
		printf("\t");
	}
	printf("<------ %p\n", (void *) target_addr);
}

static dr_emit_flags_t
event_app_instruction(void *drcontext, void *tag, instrlist_t *bb, instr_t *instr,
                      bool for_trace, bool translating, void *user_data)
{
    uint num_instrs;

	// If a call, insert print 'I am a call'
	if ( instr_is_call(instr) ) {
		// include/dr_defines.h:typedef byte * app_pc;
		// tools/DRcontrol.c:typedef unsigned char byte;
		// Thus app_pc = unsigned char *
		const void * const xip = (char *) instr_get_app_pc(instr) 
								 + instr_length(drcontext, instr);
		dr_insert_clean_call(drcontext, bb, instr,
							(void *)cll, false /* save fpstate */, 1,
							OPND_CREATE_INTPTR(xip));
	}

	if ( instr_is_return(instr) ) {
		dr_insert_mbr_instrumentation(drcontext, bb, instr, (void *) rtt,
                                      SPILL_SLOT_1);
	}

    if (!drmgr_is_first_instr(drcontext, instr))
        return DR_EMIT_DEFAULT;
    /* Only insert calls for in-app BBs */
    if (user_data == NULL)
        return DR_EMIT_DEFAULT;
    /* Insert clean call */
    num_instrs = (uint)(ptr_uint_t)user_data;
    dr_insert_clean_call(drcontext, bb, instrlist_first_app(bb),
                         (void *)inscount, false /* save fpstate */, 1,
                         OPND_CREATE_INT32(num_instrs));
    return DR_EMIT_DEFAULT;
}
