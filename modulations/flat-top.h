/**
 * This file declares types and functions for flat-top modulation.
 */

#ifndef FLAT_TOP_H
#define FLAT_TOP_H


#include "../applications/pll.h"
#include "../modulations/modulation.h"
#include "../transformations/direct-quadrature.h"


#define FLATTOP_INDICATOR_INACTIVE      0.5
#define FLATTOP_INDICATOR_ACTIVE_TOP    1.0
#define FLATTOP_INDICATOR_ACTIVE_BOTTOM 0.0


/**
 * Stores the center angle as well as
 * the ramp-up and ramp-down angles of a flat-top interval
 */
typedef struct
{
    /**
     * The flat-top operation is centered around this phase angle.
     * It's width i.e. start and stop angles are defined
     * during initialization of this struct.
     */
    float angle_center;
    float angle_start, angle_stop;

    /**
     * A factor between 0.0 and 1.0
     * which is multiplied with the difference of the momentary modulation and the maximum.
     */
    float ramp_value;
    float angle_ramp_up_start, angle_ramp_up_stop;
    float angle_ramp_down_start, angle_ramp_down_stop;

} flat_top_interval_t;


/**
 * A struct to store all angles where to start and to stop
 * flat-top operation of a three-phase inverter system
 */
typedef union
{
    struct
    {
        flat_top_interval_t top[3];
        flat_top_interval_t bottom[3];

        /**
         * The minimal modulation amplitude (between 0.0 and 1.0)
         * required for flat-top to be applied.
         */
        float modulation_threshold;

        /**
         * The margins to which to adjust the modulation during flat-top
         */
        float modulation_value_min;
        float modulation_value_max;
    };
    struct
    {
        flat_top_interval_t top_u;
        flat_top_interval_t top_v;
        flat_top_interval_t top_w;
        flat_top_interval_t bottom_u;
        flat_top_interval_t bottom_v;
        flat_top_interval_t bottom_w;
    };
} flat_top_parameters_t;


/**
 * Initialize a flat-top parameter struct
 */
void flat_top_init(
        flat_top_parameters_t* parameters,
        float flat_top_span,
        float ramp_span
        );

/**
 * This function will modify the modulation degrees
 * depending on the current grid phase angle theta
 * in such a way, that switching is disabled
 * below respectively above certain thresholds.
 * This is done in order to reduce switching losses.
 */
void flat_top_apply_ccm(
        flat_top_parameters_t* parameters,
        modulation_values_ccm_t* modulation,
        float modulation_amplitude,
        float theta
        );


#endif
