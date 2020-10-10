
#ifndef EMPE_PWM_UNIT_H
#define EMPE_PWM_UNIT_H

#include <stdint.h>
#include <stdbool.h>


#ifndef M_2x_PI
#define M_2x_PI 6.283185307179586f
#endif

#ifndef RAD
#define RAD (M_PI / 180.0)
#endif

/*
 * You may define another table length elsewhere in your code.
 * Just make sure, it is defined before this file is included.
 */
#ifndef DEADTIME_LOOKUP_TABLE_LENGTH
#define DEADTIME_LOOKUP_TABLE_LENGTH 1120
#endif


typedef struct
{
    /**
     * Ticks to be added to a dead-time in ticks
     * in order to achieve the expected dead-time
     * (driver-dependent offset; may be left zero)
     */
    float driver_correction_hs_to_ls;
    float driver_correction_ls_to_hs;

    /*
     * Boundaries for dead-time durations
     */
    float deadtime_min;
    float deadtime_max;

    /*
     * Dead-time function parameters
     */
    float deadtime_curve_begin;
    float deadtime_curve_end;
    float deadtime_curve_plateau;
    float deadtime_outside_curve;
    float deadtime_curve_height;
    float deadtime_curve_factor;

    /**
     * Phase angle in degrees
     * when the dead-time reaches it's maximum (choke current is low)
     */
    float angle_current_threshold;
    /**
     * Phase angle in degrees
     * when the choke current has reached zero
     */
    float angle_current_zero;

    /**
     * The four angles controlling the function behaviour
     * (calculated from the angles above)
     */
    float angle1, angle2, angle3, angle4;

    /*
     * Lookup table for pre-calculated dead-time values
     * as float i.e. in seconds respectively in PWM unit ticks
     */
    float deadtime_hs_to_ls[DEADTIME_LOOKUP_TABLE_LENGTH];
    float deadtime_ls_to_hs[DEADTIME_LOOKUP_TABLE_LENGTH];
    uint16_t ticks_hs_to_ls[DEADTIME_LOOKUP_TABLE_LENGTH];
    uint16_t ticks_ls_to_hs[DEADTIME_LOOKUP_TABLE_LENGTH];

} deadtime_parameters_t;

/**
 * This struct stores properties of the PWM unit and the FET driver
 * relevant for conversion of dutycycles from float to ticks.
 */
typedef struct
{
    /** The frequency of the PWM (in Hz) unit determines the time per tick */
    float tick_frequency;

    /** The frequency of one switching period (in Hz) */
    float switching_frequency;

    /** The number of ticks elapsing in the PWM unit per switching period */
    uint16_t ticks_per_period;

    /** A value which (during debugging) is clearly identifiable as maximum */
    uint16_t ticks_max;

    /**
     * The minimum number of ticks a gate signal must be high
     * for the driver to reliably recognize it and turn on the FET
     */
    uint16_t minimum_driver_on_time;

    /**
     * The minimum number of ticks a gate signal must be low
     * for the driver to reliably recognize it and turn off the FET
     */
    uint16_t minimum_driver_off_time;

    /**
     * The number of turn-on ticks above which a gate signal will remain high,
     * since the driver will not be able to switch the FET off.
     */
    uint16_t flat_high_threshold;

    /**
     * Whether to use variable dead-times or the constants above
     */
    bool use_variable_deadtimes;

    /**
     * The dead-time between the falling edge of the highside
     * and the rising edge of the lowside gate signal (in seconds)
     */
    float deadtime_hs_to_ls;

    /**
     * The dead-time between the falling edge of the lowside
     * and the rising edge of the highside gate signal (in seconds)
     */
    float deadtime_ls_to_hs;

    /**
     * The number of ticks to pause between the falling edge
     * of the highside and the rising edge of the lowside gate signal
     */
    uint16_t deadtime_ticks_hs_to_ls;

    /**
     * The number of ticks to pause between the falling edge
     * of the lowside and the rising edge of the highside gate signal
     */
    uint16_t deadtime_ticks_ls_to_hs;

    /**
     * Properties relevant during dead-time conversion
     * from nanoseconds to PWM unit ticks
     */
    deadtime_parameters_t* variable_deadtimes;

} pwm_unit_properties_t;


/**
 * Pre-calculate all dead times and store
 * them in the lookup table
 */
void pwm_unit_init(
        pwm_unit_properties_t* pwm_unit
        );

/**
 * Return the lookup table index for the given phase angle in rad
 */
uint16_t get_deadtime_lookup_table_index(float angle);

/**
 * Returns the optimal dead time between the falling edge of the high-side
 * and the rising edge of the complementary low-side gate signal (in FPGA ticks)
 * depending on the corresponding half-bridge's momentary phase angle (in rad)
 */
uint16_t get_deadtime_hs_to_ls_by_angle(
        pwm_unit_properties_t*,
        float angle
        );

/**
 * Returns the optimal dead time between the falling edge of the low-side
 * and the rising edge of the complementary high-side gate signal (in FPGA ticks)
 * depending on the corresponding half-bridge's momentary phase angle (in rad)
 */
uint16_t get_deadtime_ls_to_hs_by_angle(
        pwm_unit_properties_t*,
        float angle
        );


#endif
