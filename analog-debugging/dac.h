/**
 * This file declares types and functions
 * to manage value debugging via analog debugger.
 */

#ifndef ANALOG_DEBUGGER_DAC_H
#define ANALOG_DEBUGGER_DAC_H

#include <stdbool.h>
#include "debug.h"

/*
 * The developer must have a file named analog-debugger.h in his/her project,
 * in which type dac_channel_e is declared enumerating all available DAC channels,
 * see also: analog-debugger.h.template
 */
#include "analog-debugger.h"


/**
 * Initialize analog value output via DACs
 */
void init_debugging_via_dac();

/**
 * Select a specific debug value for debugging via the given DAC
 */
void select_value_for_debugging(dac_channel_e, debug_value_e);

/**
 * Return the float value currently on the selected DAC debug channel
 */
debug_value_t* get_dac_debug_value(dac_channel_e);

/**
 * Write the selected debug values to the outgoing data stream
 */
void write_debug_values_to_output(
        bool auto_adjust_min_max,
        raw_setpoints_t*
        );


#endif
