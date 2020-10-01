/**
 * Here we deal with the generation of gate signals from dutycycles.
 */

#ifndef EMPE_GATE_SIGNALS_H
#define EMPE_GATE_SIGNALS_H


#include <stdint.h>


/** Adjust this as required */
typedef uint16_t tick_number_t;


/**
 * A struct to store all turn-on and turn-off times
 * of one half-bridge i.e. one high-side and one low-side switch
 *
 * The order of words in this struct is reversed,
 * because in MSB-first mode the SPI peripheral
 * transmits words from last to first.
 */
typedef union
{
    tick_number_t tick_number[4];
    struct
    {
        tick_number_t tick_numbers_lowside[2];
        tick_number_t tick_numbers_highside[2];
    };
    struct
    {
        tick_number_t tick_number_lowside_falling;
        tick_number_t tick_number_lowside_rising;
        tick_number_t tick_number_highside_falling;
        tick_number_t tick_number_highside_rising;
    };
} gate_signals_halfbridge_t;


/**
 * A struct to store turn-on and turn-off times of all six
 * switches in a three-phase inverter system.
 *
 * The order of words in this struct is reversed,
 * because in MSB-first mode the SPI peripheral
 * transmits words from last to first.
 */
typedef union
{
    tick_number_t tick_number[12];
    gate_signals_halfbridge_t phase[3];
    struct
    {
        gate_signals_halfbridge_t phase_w;
        gate_signals_halfbridge_t phase_v;
        gate_signals_halfbridge_t phase_u;
    };
    struct
    {
        tick_number_t phase_w_tick_number_lowside_falling;
        tick_number_t phase_w_tick_number_lowside_rising;
        tick_number_t phase_w_tick_number_highside_falling;
        tick_number_t phase_w_tick_number_highside_rising;
        tick_number_t phase_v_tick_number_lowside_falling;
        tick_number_t phase_v_tick_number_lowside_rising;
        tick_number_t phase_v_tick_number_highside_falling;
        tick_number_t phase_v_tick_number_highside_rising;
        tick_number_t phase_u_tick_number_lowside_falling;
        tick_number_t phase_u_tick_number_lowside_rising;
        tick_number_t phase_u_tick_number_highside_falling;
        tick_number_t phase_u_tick_number_highside_rising;
    };
} gate_signals_threephase_t;


/**
 * Set all tick numbers to zero
 */
void gate_signals_disable_halfbridge(gate_signals_halfbridge_t*);

/**
 * Set all tick numbers to zero
 */
void gate_signals_disable_threephase(gate_signals_threephase_t*);


#include "dutycycles.h"
#include "pwm_unit.h"
#include "halfbridge.h"
#include "threephase.h"
#include "../modulations/simultaneous-turnon.h"


/**
 * Calculates the tick numbers for one half-bridge (high-side and low-side switches)
 * as well as the new half-bridge state
 */
void gate_signals_calculate_halfbridge(
        pwm_unit_properties_t*,
        float dutycycle_highside,
        float dutycycle_lowside,
        switching_mode_e,
        halfbridge_state_e*,
        gate_signals_halfbridge_t*
        );

/**
 * Calculate gate signals i.e. tick numbers for
 * all switches of a three-phase inverter
 */
void gate_signals_calculate_threephase(
        threephase_inverter_values_t*,
        gate_signals_threephase_t*
        );


#endif
