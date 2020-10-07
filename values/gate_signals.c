
#include "gate_signals.h"
#include "halfbridge.h"
#include "math.h"


inline void gate_signals_disable_halfbridge(gate_signals_halfbridge_t* setpoints)
{
    for (uint8_t i=0; i<sizeof(setpoints->tick_number)/sizeof(setpoints->tick_number[0]); i++)
        setpoints->tick_number[i] = 0;
}


inline void gate_signals_disable_threephase(gate_signals_threephase_t* setpoints)
{
    for (uint8_t i=0; i<sizeof(setpoints->tick_number)/sizeof(setpoints->tick_number[0]); i++)
        setpoints->tick_number[i] = 0;
}


#define max(a, b)   ((a > b) ? a : b)


inline void gate_signals_calculate_halfbridge(
        pwm_unit_properties_t* pwm_unit,
        float dutycycle_highside,
        float dutycycle_lowside,
        switching_mode_e switching_mode,
        halfbridge_state_e* state,
        gate_signals_threephase_t* common_mode_bias,
        gate_signals_threephase_t* gate_signals_threephase,
        uint8_t phase_index,
        float phase_angle
        )
{
    if (phase_index > 2)
        return;
    gate_signals_halfbridge_t* gate_signals = &gate_signals_threephase->phase[phase_index];

    /*
     * Determine the optimal dead times, depending on the phase angle
     */
    uint16_t deadtime_hs_to_ls = get_deadtime_hs_to_ls_by_angle(phase_angle);
    uint16_t deadtime_ls_to_hs = get_deadtime_ls_to_hs_by_angle(phase_angle);

    /**
     * The number of ticks per switching period
     * when not counting the deadtimes
     */
    uint16_t ticks_max_minus_deadtimes = (pwm_unit->ticks_per_period-1) - deadtime_hs_to_ls - deadtime_ls_to_hs;

    uint16_t tick_count_highside = ticks_max_minus_deadtimes * dutycycle_highside;
    uint16_t tick_count_lowside = ticks_max_minus_deadtimes * dutycycle_lowside;
    uint16_t ticks_max = pwm_unit->ticks_max;

    if ((switching_mode != SIMULTANEOUS_HIGHSIDE_TURNON) && (switching_mode != SIMULTANEOUS_LOWSIDE_TURNON))
    {
        // SWITCHING_DISABLED or unsupported mode of operation => disable this half-bridge
        *state = HALFBRIDGE_DISABLED;

        // Disable both switches, high-side and low-side
        for (uint8_t i=0; i<4; i++)
            gate_signals->tick_number[i] = ticks_max;

        // There is no need to continue calculating
        return;
    }

    /*
     * Evaluate the turn-on durations to determine the half-bridge's current state
     */
    bool flat_high_highside = false;
    bool flat_low_highside = false;
    bool flat_high_lowside = false;
    bool flat_low_lowside = false;
    halfbridge_state_e new_state = REGULAR_PWM;

    if (tick_count_highside < pwm_unit->minimum_driver_on_time)
    {
        new_state = INTERMITTENT_LOWSIDE;
        // The switch shall stay off.
        tick_count_highside = 0;
        flat_low_highside = true;
        // The complementary switch may turn on longer.
        tick_count_lowside = (pwm_unit->ticks_per_period-1) * dutycycle_lowside;
    }
    if (tick_count_lowside < pwm_unit->minimum_driver_on_time)
    {
        new_state = INTERMITTENT_HIGHSIDE;
        // The switch shall stay off.
        tick_count_lowside = 0;
        flat_low_lowside = true;
        // The complementary switch may turn on longer.
        tick_count_highside = (pwm_unit->ticks_per_period-1) * dutycycle_highside;
    }
    if (tick_count_highside >= pwm_unit->flat_high_threshold)
    {
        new_state = FLAT_HIGH_HIGHSIDE;
        flat_high_highside = true;
        tick_count_highside = ticks_max;
    }
    if (tick_count_lowside >= pwm_unit->flat_high_threshold)
    {
        new_state = FLAT_HIGH_LOWSIDE;
        flat_high_lowside = true;
        tick_count_lowside = ticks_max;
    }

    /*
     * Calculate turn-on and turn-off ticks
     * according to the half-bridge state chosen above
     */
    if (switching_mode == SIMULTANEOUS_HIGHSIDE_TURNON)
    {
        /*
         * Regular PWM
         * with all high-side switches turned on simultaneously
         */
        uint16_t ticks = deadtime_ls_to_hs;
        gate_signals->tick_number_highside_rising = ticks;
        ticks += tick_count_highside;
        gate_signals->tick_number_highside_falling = ticks;
        if (tick_count_highside > 0)
            ticks += deadtime_hs_to_ls;
        gate_signals->tick_number_lowside_rising = ticks;
        ticks += tick_count_lowside;
        gate_signals->tick_number_lowside_falling = ticks;

        if (flat_high_highside)
        {
            /*
             * When transitioning from regular PWM to flat-high highside,
             * the lowside might just have been turned off.
             * A dead time must be inserted, before the
             * high-side switch can safely be turned on.
             *
             * After this intermediate transition, the highside can
             * safely be turned on for the complete period.
             */
            if (*state == REGULAR_PWM)
                gate_signals->tick_number_highside_rising  = max(deadtime_ls_to_hs, pwm_unit->minimum_driver_off_time);
            else
                // Highside is already intermittent or flat-high
                gate_signals->tick_number_highside_rising  = 0;

            gate_signals->tick_number_highside_falling = ticks_max;

            // The low-side switch must be off.
            gate_signals->tick_number_lowside_rising   = ticks_max;
            gate_signals->tick_number_lowside_falling  = ticks_max;
        }
        else if (flat_high_lowside)
        {
            gate_signals->tick_number_lowside_rising   = 0;
            gate_signals->tick_number_lowside_falling  = ticks_max;

            // The high-side switch must be off.
            gate_signals->tick_number_highside_rising  = ticks_max;
            gate_signals->tick_number_highside_falling = ticks_max;
        }
        else
        {
            if (flat_low_highside)
            {
                /*
                 * Intermittent switching on the low-side switch:
                 * The high-side switch stays off.
                 */
                gate_signals->tick_number_highside_rising  = ticks_max;
                gate_signals->tick_number_highside_falling = ticks_max;
            }
            else if (flat_low_lowside)
            {
                /*
                 * Intermittent switching on the high-side switch:
                 * The low-side switch stays off.
                 */
                gate_signals->tick_number_lowside_rising   = ticks_max;
                gate_signals->tick_number_lowside_falling  = ticks_max;
            }

            /**
             * The first (and only the first) high-side pulse
             * after a lowside flat-high / highside flat-low period,
             * is delayed by about 103ns.
             * This is compensated below.
             *
             * CAVEAT: This only works, if the control frequency equals the switching frequency.
             */
//            if ((setpoints->state[phase] != REGULAR_PWM) && !(flat_high_highside || flat_high_lowside || flat_low_highside || flat_low_lowside))
//            {
//                // Accelerate this pulse
//                gate_signals->tick_number_highside_rising  -= 8;
//                gate_signals->tick_number_highside_falling -= 8;
//            }
        }
    }
    else if (switching_mode == SIMULTANEOUS_LOWSIDE_TURNON)
    {
        /*
         * Regular PWM
         * with all low-side switches turned on simultaneously
         */
        uint16_t ticks = deadtime_hs_to_ls;
        gate_signals->tick_number_lowside_rising   = ticks;
        ticks += tick_count_lowside;
        gate_signals->tick_number_lowside_falling  = ticks;
        if (tick_count_lowside > 0)
            ticks += deadtime_ls_to_hs;
        gate_signals->tick_number_highside_rising  = ticks;
        ticks += tick_count_highside;
        gate_signals->tick_number_highside_falling = ticks;

        if (flat_high_lowside)
        {
            /*
             * When transitioning from the regular PWM to flat-high lowside,
             * the high-side switch may just have been turned off at he beginning of the switching period.
             * A dead-time must be inserted at first.
             */
            if (*state == REGULAR_PWM)
                gate_signals->tick_number_lowside_rising   = max(deadtime_hs_to_ls, pwm_unit->minimum_driver_off_time);
            else
                gate_signals->tick_number_lowside_rising   = 0;
            gate_signals->tick_number_lowside_falling = ticks_max;

            // The high-side switch must be off.
            gate_signals->tick_number_highside_rising  = ticks_max;
            gate_signals->tick_number_highside_falling = ticks_max;
        }
        else if (flat_high_highside)
        {
            // The high-side switch is permanently turned on.
            gate_signals->tick_number_highside_rising  = 0;
            gate_signals->tick_number_highside_falling = ticks_max;

            // The low-side switch must be off.
            gate_signals->tick_number_lowside_rising   = ticks_max;
            gate_signals->tick_number_lowside_falling  = ticks_max;
        }
        else
        {
            if (flat_low_lowside)
            {
                /*
                 * Intermittent high-side switching:
                 * The low-side switch stays off.
                 */
                gate_signals->tick_number_lowside_rising   = ticks_max;
                gate_signals->tick_number_lowside_falling  = ticks_max;
            }
            else if (flat_low_highside)
            {
                /*
                 * Intermittent low-side switching:
                 * The high-side switch stays off.
                 */
                gate_signals->tick_number_highside_rising  = ticks_max;
                gate_signals->tick_number_highside_falling = ticks_max;
            }
        }
    }

    /*
     * Simplify tick numbers if possible
     */
    if (dutycycle_highside == 0.0)
    {
        gate_signals->tick_number_highside_rising = ticks_max;
        gate_signals->tick_number_highside_falling = ticks_max;
    }
    if (dutycycle_lowside == 0.0)
    {
        gate_signals->tick_number_lowside_rising = ticks_max;
        gate_signals->tick_number_lowside_falling = ticks_max;
    }

    *state = new_state;
}


void gate_signals_calculate_threephase(
        threephase_inverter_values_t* values,
        gate_signals_threephase_t* gate_signals,
        float angle
        )
{
    static gate_signals_threephase_t common_mode_bias;

    for (uint8_t i=0; i<3; i++)
    {
        gate_signals_calculate_halfbridge(
                values->pwm_unit_properties,
                values->dutycycles.dutycycle_highside[i],
                values->dutycycles.dutycycle_lowside[i],
                values->switching_mode,
                &values->halfbridge_states.state[i],
                &common_mode_bias,
                gate_signals,
                i,
                angle
                );
        angle += M_PI_2_3;
        if (angle > M_TWOPI)
            angle -= M_TWOPI;
    }
}
