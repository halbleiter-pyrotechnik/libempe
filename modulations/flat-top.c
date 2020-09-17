/**
 * This file implements flat-top modulation.
 */

#include "flat-top.h"
#include <stdint.h>
#include <math.h>
#include "values/debugging/debug.h"
#include "sine.h"


/*
 * Normalize an angle to lie between 0.0 and 2*PI
 */
static inline void normalize_angle(float* angle)
{
    while (*angle < 0.0f)
        *angle += (2 * M_PI);
    while (*angle > (2 * M_PI))
        *angle -= (2 * M_PI);
}


static inline void flat_top_expand_interval(
        flat_top_interval_t* interval,
        float span
        )
{
    interval->start = interval->center - span;
    normalize_angle(&interval->start);
    interval->stop  = interval->center + span;
    normalize_angle(&interval->stop);
}


void flat_top_init(
        flat_top_parameters_t* parameters,
        float flat_top_span,
        float ramp_span
        )
{
    /*
     * Angles for three-phase sine (not suitable for cosine!),
     * where angle_w > angle_v > angle_u
     */
    parameters->top_u.angle.center = M_PI/2;
    parameters->top_v.angle.center = M_PI/2 - RAD_120_DEGREE;
    parameters->top_w.angle.center = M_PI/2 - 2*RAD_120_DEGREE;

    /*
     * Flat-bottom angles are 180 degrees phase-shifted from the flat-top angles
     */
    for (uint8_t i=0; i<3; i++)
        parameters->bottom[i].angle.center = parameters->top[i].angle.center + M_PI;

    /*
     * Expand flat-bottom intervals using span value
     */
    for (uint8_t i=0; i<6; i++)
    {
        normalize_angle(&parameters->interval[i].angle.center);
        flat_top_expand_interval(&parameters->interval[i].angle, flat_top_span);

        parameters->interval[i].ramp_up.center = parameters->interval[i].angle.start;
        parameters->interval[i].ramp_down.center = parameters->interval[i].angle.stop;

        for (uint8_t j=0; j<2; j++)
        {
            flat_top_expand_interval(&parameters->interval[i].ramps[j], ramp_span);
        }
    }

    parameters->modulation_value_max = +1.0f;
    parameters->modulation_value_min = -1.0f;
    parameters->modulation_threshold = 0.8;
}


/**
 * Helper function to determine whether the current
 * phase angle lies inbetween a lower and a higher threshold
 */
static inline bool angle_lies_within(
        float angle,
        flat_top_intervals_t* interval
        )
{
    float upper_threshold = interval->angle.stop;
    float lower_threshold = interval->angle.start;

    if (lower_threshold < upper_threshold)
        return (angle > lower_threshold) && (angle < upper_threshold);
    else
        return (angle > lower_threshold) || (angle < upper_threshold);
}


void flat_top_apply_ccm(
        flat_top_parameters_t* parameters,
        modulation_values_ccm_t* modulation,
        float modulation_amplitude,
        float theta
        )
{
    if (modulation_amplitude < parameters->modulation_threshold)
        return;

    const debug_value_e flat_top_indicator[] = {
        PFC_FLATTOP_INDICATOR_U,
        PFC_FLATTOP_INDICATOR_V,
        PFC_FLATTOP_INDICATOR_W
        };

    // All pre-configured angles are normalized.
    normalize_angle(&theta);

    /*
     * For all three phases determine whether to switch to flat top mode or not
     */
    for (uint8_t i=0; i<3; i++)
    {
        if (angle_lies_within(theta, &parameters->top[i]))
        {
            /*
             * Flat top mode
             */
            set_debug_value(flat_top_indicator[i], FLATTOP_INDICATOR_ACTIVE_TOP);

            float delta = parameters->modulation_value_max - modulation->value[i];
            if (delta < 0.0f)
            {
                // That's an error.
                continue;
            }

            modulation->value_u += delta;
            modulation->value_v += delta;
            modulation->value_w += delta;
        }
        else if (angle_lies_within(theta, &parameters->bottom[i]))
        {
            /*
             * Flat bottom mode
             */
            set_debug_value(flat_top_indicator[i], FLATTOP_INDICATOR_ACTIVE_BOTTOM);

            float delta = modulation->value[i] - parameters->modulation_value_min;
            if (delta < 0.0f)
            {
                // That's an error.
                continue;
            }

            modulation->value_u -= delta;
            modulation->value_v -= delta;
            modulation->value_w -= delta;
        }
        else
        {
            /*
             * Inbetween flat top and flat bottom
             */
            set_debug_value(flat_top_indicator[i], FLATTOP_INDICATOR_INACTIVE);
        }
    }
}
