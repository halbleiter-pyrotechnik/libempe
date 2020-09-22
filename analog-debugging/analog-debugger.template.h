
#ifndef ANALOG_DEBUGGER_H
#define ANALOG_DEBUGGER_H


/**
 * The complete list of available DAC channels
 */
typedef enum
{
    // Project specific:
    DAC_CHANNEL1 = 0,
    DAC_CHANNEL2,
    DAC_CHANNEL3,
    DAC_CHANNEL4,

    // Obligatory:
    DAC_CHANNEL_COUNT
} dac_channel_e;

/**
 * The complete list of values
 * that can be selected for debugging on a DAC
 */
typedef enum
{
    // Obligatory:
    DEBUGGING_DISABLED = 0,

    // Project specific:
    DEBUG_MY_CUSTOM_SYSTEM_VALUE,
    DEBUG_ANOTHER_CUSTOM_VALUE,

    // Obligatory:
    DEBUG_VALUE_COUNT
} debug_value_e;


#endif
