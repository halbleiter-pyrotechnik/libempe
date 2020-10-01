
#ifndef EMPE_PWM_UNIT_H
#define EMPE_PWM_UNIT_H


/**
 * This struct stores properties of the PWM unit and the FET driver
 * relevant for conversion of dutycycles from float to ticks.
 */
typedef struct
{
    /** The frequency of the PWM (in Hz) unit determines the time per tick */
    float pwm_unit_frequency;

    /** The frequency of one switching period (in Hz) */
    float switching_frequency;

    /** The number of ticks elapsing in the PWM unit per switching period */
    uint16_t ticks_per_period;

    /** A value which (during debugging) is clearly identifiable as maximum */
    uint16_t ticks_max;

    /**
     * The number of ticks to pause between the falling edge
     * of the highside and the rising edge of the lowside gate signal
     */
    uint16_t deadtime_hs_to_ls;

    /**
     * The number of ticks to pause between the falling edge
     * of the lowside and the rising edge of the highside gate signal
     */
    uint16_t deadtime_ls_to_hs;

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
     * The number of ticks per switching period
     * when not counting the deadtimes
     */
    uint16_t ticks_max_minus_deadtimes;

    /**
     * The number of turn-on ticks above which a gate signal will remain high,
     * since the driver will not be able to switch the FET off.
     */
    uint16_t flat_high_threshold;

} pwm_unit_properties_t;


#endif
