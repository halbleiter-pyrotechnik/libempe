
#ifndef EMPE_THREEPHASE_H
#define EMPE_THREEPHASE_H


#include "pwm_unit.h"
#include "dutycycles.h"
#include "halfbridge.h"
#include "../modulations/simultaneous-turnon.h"


typedef struct
{
    /**
     * A pointer to a struct holding the properties
     * of the used PWM units and FET drivers
     */
    pwm_unit_properties_t* pwm_unit_properties;

    /**
     * The momentary dutycycle values
     * as calculated by the firmware
     */
    dutycycles_threephase_t dutycycles;

    /**
     * The momentary half-bridge states
     * as evaluated and updated by the
     * dutycycles-to-ticks conversion function
     */
    halfbridge_states_threephase_t halfbridge_states;

    /**
     * The switching modes of both momentary
     * and previous update cycle.
     * It is decided by the sign of the current.
     */
    switching_mode_e switching_mode;
    switching_mode_e previous_switching_mode;

} threephase_inverter_values_t;


#endif
