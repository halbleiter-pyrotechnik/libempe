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


/**
 * Helper function to determine whether the current
 * phase angle lies inbetween a lower and a higher threshold
 */
static inline bool angle_lies_between(
        float angle,
        float lower_threshold,
        float upper_threshold
        )
{
    normalize_angle(&angle);
    normalize_angle(&lower_threshold);
    normalize_angle(&upper_threshold);

    if (lower_threshold < upper_threshold)
        return (angle > lower_threshold) && (angle < upper_threshold);
    else
        return (angle > lower_threshold) || (angle < upper_threshold);
}


void apply_flat_top_ccm(
        modulation_values_ccm_t* modulation,
        float modulation_amplitude,
        float theta
        )
{
    /*
     * Flat top parameters
     */
    const float flat_top_span = M_PI/6.0; // in phase angle degrees

    /*
     * Angles for three-phase sine (not cosine),
     * where angle_w > angle_v > angle_u
     */
    const float theta_flat_top[] = {
        M_PI/2,
        M_PI/2 - RAD_120_DEGREE,
        M_PI/2 - 2*RAD_120_DEGREE
        };

    const float theta_flat_top_lower_threshold[] = {
        theta_flat_top[0] - flat_top_span,
        theta_flat_top[1] - flat_top_span,
        theta_flat_top[2] - flat_top_span
        };

    const float theta_flat_top_upper_threshold[] = {
        theta_flat_top[0] + flat_top_span,
        theta_flat_top[1] + flat_top_span,
        theta_flat_top[2] + flat_top_span
        };

    const float theta_flat_bottom[] = {
        theta_flat_top[0] + M_PI,
        theta_flat_top[1] + M_PI,
        theta_flat_top[2] + M_PI
        };

    const float theta_flat_bottom_lower_threshold[] = {
        theta_flat_bottom[0] - flat_top_span,
        theta_flat_bottom[1] - flat_top_span,
        theta_flat_bottom[2] - flat_top_span
        };

    const float theta_flat_bottom_upper_threshold[] = {
        theta_flat_bottom[0] + flat_top_span,
        theta_flat_bottom[1] + flat_top_span,
        theta_flat_bottom[2] + flat_top_span
        };

    // Always on
    float switch_on_threshold = 100.0f;
//    float switch_on_threshold = 0.2f;
    const float modulation_max_value = +1.0f;
    const float modulation_min_value = -1.0f;

    /**
     * The minimal modulation amplitude (between 0.0 and 1.0)
     * required for flat-top to be applied.
     */
    const float flat_top_turn_on_threshold = 0.8;
    if (modulation_amplitude < flat_top_turn_on_threshold)
        return;

    const debug_value_e flat_top_indicator[] = {
        PFC_FLATTOP_INDICATOR_U,
        PFC_FLATTOP_INDICATOR_V,
        PFC_FLATTOP_INDICATOR_W
        };

    /*
     * For all three phases determine whether to switch to flat top mode or not
     */
    for (uint8_t i=0; i<3; i++)
    {
        if (angle_lies_between(theta, theta_flat_top_lower_threshold[i], theta_flat_top_upper_threshold[i]))
        {
            /*
             * Flat top mode
             */
            set_debug_value(flat_top_indicator[i], FLATTOP_INDICATOR_ACTIVE_TOP);

            float modulation_adjustment = modulation_max_value - modulation->value[i];
            if (modulation_adjustment < 0.0f)
            {
                // That's an error.
                continue;
            }

            if (fabs(modulation_adjustment) < switch_on_threshold)
            {
                modulation->value_u += modulation_adjustment;
                modulation->value_v += modulation_adjustment;
                modulation->value_w += modulation_adjustment;
            }
        }
        else if (angle_lies_between(theta, theta_flat_bottom_lower_threshold[i], theta_flat_bottom_upper_threshold[i]))
        {
            /*
             * Flat bottom mode
             */
            set_debug_value(flat_top_indicator[i], FLATTOP_INDICATOR_ACTIVE_BOTTOM);

            float modulation_adjustment = modulation->value[i] - modulation_min_value;
            if (modulation_adjustment < 0.0f)
            {
                // That's an error.
                continue;
            }

            if (fabs(modulation_adjustment) < switch_on_threshold)
            {
                modulation->value_u -= modulation_adjustment;
                modulation->value_v -= modulation_adjustment;
                modulation->value_w -= modulation_adjustment;
            }
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
