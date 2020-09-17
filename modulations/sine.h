
#ifndef SINE_MODULATION_H
#define SINE_MODULATION_H


#include "modulation.h"

#ifndef RAD_120_DEGREE
#define RAD_120_DEGREE 2.0943951023931953f
#endif


/**
 * Calculate dutycycles for a three-phase, sine modulated PWM signal
 * in continuous current mode (CCM)
 */
void sine_modulate_ccm(
        float phase_angle_rad,
        float amplitude,
        modulation_values_ccm_t*
        );


#endif
