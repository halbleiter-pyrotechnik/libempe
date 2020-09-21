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

static inline float get_normalized_angle(float angle)
{
    normalize_angle(&angle);
    return angle;
}


static inline void flat_top_expand_interval(flat_top_interval_t* interval)
{
    interval->start = interval->center - interval->width/2.0;
    normalize_angle(&interval->start);
    interval->stop  = interval->center + interval->width/2.0;
    normalize_angle(&interval->stop);
}


void flat_top_init(
        flat_top_parameters_t* parameters,
        float flat_top_width,
        float ramp_width
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

        parameters->interval[i].angle.width = flat_top_width;
        flat_top_expand_interval(&parameters->interval[i].angle);

        parameters->interval[i].ramp_in.center = parameters->interval[i].angle.start;
        parameters->interval[i].ramp_out.center = parameters->interval[i].angle.stop;

        for (uint8_t j=0; j<2; j++)
        {
            parameters->interval[i].ramps[j].value = 0.0;
            parameters->interval[i].ramps[j].width = ramp_width;
            flat_top_expand_interval(&parameters->interval[i].ramps[j]);
        }
    }

    parameters->modulation_value_max = +1.0f;
    parameters->modulation_value_min = -1.0f;
    parameters->modulation_threshold = 0.8;
    parameters->enable_ramping = (ramp_width > 0.0);
}


/**
 * Helper function to determine whether the current
 * phase angle lies inbetween a lower and a higher threshold
 */
static inline bool angle_lies_within(
        float angle,
        float lower_threshold,
        float upper_threshold
        )
{
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
    const debug_value_e flat_top_indicator[] = {
        PFC_FLATTOP_INDICATOR_U,
        PFC_FLATTOP_INDICATOR_V,
        PFC_FLATTOP_INDICATOR_W
        };

    for (uint8_t i=0; i<3; i++)
        set_debug_value(flat_top_indicator[i], FLATTOP_INDICATOR_INACTIVE);

    if (modulation_amplitude < parameters->modulation_threshold)
        return;

    // All pre-configured angles are normalized.
    normalize_angle(&theta);

    /*
     * For all three phases determine whether to switch to flat top mode or not
     */
    for (uint8_t i=0; i<3; i++)
    {
        if (angle_lies_within(theta, parameters->top[i].ramp_in.start, parameters->top[i].ramp_out.stop))
        {
            /*
             * Flat top mode
             */
            float delta = parameters->modulation_value_max - modulation->value[i];
            if (delta < 0.0f)
            {
                // That's an error.
                continue;
            }

            float flag = 0.5;
            if (parameters->enable_ramping)
            {
                const float step = 0.35;
                if (angle_lies_within(theta, parameters->top[i].ramp_in.start, parameters->top[i].ramp_in.stop))
                {
//                    delta *= parameters->top[i].ramp_in.value;
                    delta = parameters->top[i].ramp_in.value;
                    flag = parameters->top[i].ramp_in.value * FLATTOP_INDICATOR_INACTIVE;
                    parameters->top[i].ramp_in.value += step;
                    if (parameters->top[i].ramp_in.value > 1.0)
                        parameters->top[i].ramp_in.value = 1.0;
                }
                else if (angle_lies_within(theta, parameters->top[i].ramp_out.start, parameters->top[i].ramp_out.stop))
                {
                    delta *= parameters->top[i].ramp_out.value;
                    flag = parameters->top[i].ramp_out.value * FLATTOP_INDICATOR_INACTIVE;
                    parameters->top[i].ramp_out.value -= step;
                    if (parameters->top[i].ramp_out.value < 0.0)
                        parameters->top[i].ramp_out.value = 0.0;
                }
                else
                {
                    parameters->top[i].ramp_in.value = 0.0;
                    parameters->top[i].ramp_out.value = 1.0;
                }
            }
            set_debug_value(flat_top_indicator[i], FLATTOP_INDICATOR_INACTIVE + flag);

            modulation->value_u += delta;
            modulation->value_v += delta;
            modulation->value_w += delta;
        }
        else if (angle_lies_within(theta, parameters->bottom[i].ramp_in.start, parameters->bottom[i].ramp_out.stop))
        {
            /*
             * Flat bottom mode
             */
            float delta = modulation->value[i] - parameters->modulation_value_min;
            if (delta < 0.0f)
            {
                // That's an error.
                continue;
            }

            float flag = -0.5;
            if (parameters->enable_ramping)
            {
                const float step = 0.3;
                if (angle_lies_within(theta, parameters->bottom[i].ramp_in.start, parameters->bottom[i].ramp_in.stop))
                {
                    delta *= parameters->bottom[i].ramp_in.value;
                    flag = -1.0 * parameters->bottom[i].ramp_in.value * FLATTOP_INDICATOR_INACTIVE;
                    parameters->bottom[i].ramp_in.value += step;
                    if (parameters->bottom[i].ramp_in.value > 1.0)
                        parameters->bottom[i].ramp_in.value = 1.0;
                }
                else if (angle_lies_within(theta, parameters->bottom[i].ramp_out.start, parameters->bottom[i].ramp_out.stop))
                {
                    delta *= parameters->bottom[i].ramp_out.value;
                    flag = -1.0 * parameters->bottom[i].ramp_out.value * FLATTOP_INDICATOR_INACTIVE;
                    parameters->bottom[i].ramp_out.value -= step;
                    if (parameters->bottom[i].ramp_out.value < 0.0)
                        parameters->bottom[i].ramp_out.value = 0.0;
                }
                else
                {
                    parameters->bottom[i].ramp_in.value = 0.0;
                    parameters->bottom[i].ramp_out.value = 1.0;
                }
            }
            set_debug_value(flat_top_indicator[i], FLATTOP_INDICATOR_INACTIVE + flag);

            modulation->value_u -= delta;
            modulation->value_v -= delta;
            modulation->value_w -= delta;
        }
    }
}
