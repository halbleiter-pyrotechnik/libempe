
#ifndef EMPE_PWM_UNIT_H
#define EMPE_PWM_UNIT_H

#include <stdint.h>

#ifndef M_2x_PI
#define M_2x_PI 6.283185307179586f
#endif

#ifndef RAD
#define RAD (M_PI / 180.0)
#endif

#define DEADTIME_LOOKUP_TABLE_LENGTH 1120


typedef struct
{
    /*
     * Buffers to store pre-calculated dead-time values (in ticks)
     */
    uint16_t deadtime_hs_to_ls[DEADTIME_LOOKUP_TABLE_LENGTH];
    uint16_t deadtime_ls_to_hs[DEADTIME_LOOKUP_TABLE_LENGTH];
} deadtimes_t;

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

    deadtimes_t deadtimes;

} pwm_unit_properties_t;


/**
 * Pre-calculate all dead times and store
 * them in the lookup table
 */
void deadtime_lookup_table_init(
        pwm_unit_properties_t* pwm_unit
        );

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
