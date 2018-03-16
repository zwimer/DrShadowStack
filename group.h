#ifndef __GROUP_H__
#define __GROUP_H__

/*********************************************************/
/*                                                       */
/*                  Function Declarations                */
/*                                                       */
/*********************************************************/


// Start the group, then change the default signal handlers
// of common program killing signals to terminate the group
void setup_group();

// Ends the group
void terminate_group();


#endif
