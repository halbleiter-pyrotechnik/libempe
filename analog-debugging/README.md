
## Analog debugging

In power electronic systems, the proper function of the controlling
microcontroller (MCU) must be tested and verified just like
with any piece of software. For this purpose it is often useful and
can sometimes even prove essential to be able to monitor not only the
states of the digital pins, but also changes in internal values i.e.
variables over time. We call this analog debugging here and it can
provide significant insight into the MCU's function.
Using an oscilloscope to monitor voltages and currents side-by-side
with virtual values allows to quickly get to the root of many
problems ranging from ADC conversion issues to control formula and
calculation implementation errors.

### Hardware prerequisites

During analog debugging digital-to-analog converters (DACs)
are used to output internal values to the oscilloscope.
Obviously, the hardware prerequisite for analog debugging is
therefore the availability of DACs
connected to the MCU.
The interface between a DAC and the MCU is not defined here
and must hence be implemented within the project.

### How does it work

The code in this library contains types and functions
to store, access, modify and convert arbitrary
floating point integers (floats).
It is assumed, that the raw value being sent to a DAC
will be required in the form of an unsigned integer (uint),
as it is the case for most DACs.
16 bits are reserved per DAC (uint16_t).
Prior to the update of DAC values
i.e. the transmission of converted, raw values
via the hardware interface,
floats are converted to uint16_t using a linear function
between minimum and maximum value boundaries.
Those are independently configurable for each debug value
(and must be configured in the
initialization phase of the firmware), e.g.:

~~~
void init_analog_debugger()
{
    // The voltage is expected between 0V and 230V.
    debug_value_set_min_max(DEBUG_VOLTAGE, 0.0, 230.0);

    // The current is expected between -10A and 10A.
    debug_value_set_min_max(DEBUG_CURRENT, -10.0, 10.0);

    // The power is expected between 0.5W and 1kW.
    debug_value_set_min_max(DEBUG_POWER, 0.5, 1000.0);
    ...
}
~~~

The general idea is, that anywhere within the main code
values can be stored for analog debugging using the function set_debug_value(), e.g.:
~~~
...
set_debug_value(DEBUG_VOLTAGE, voltage);
set_debug_value(DEBUG_CURRENT, current);

float power = voltage * current;

set_debug_value(DEBUG_POWER, power);
...
~~~

When the firmware is running, at some point it must be selected,
which values shall be debugged,
i.e. monitored on the oscilloscope,
i.e. transmitted to the DACs.
This is accomplished using the function
select_value_for_debugging(), e.g.:

~~~
void init_analog_debugger()
{
    ...
    select_value_for_debugging(DAC_CHANNEL1, DEBUG_VOLTAGE);
    select_value_for_debugging(DAC_CHANNEL2, DEBUG_CURRENT);
    select_value_for_debugging(DAC_CHANNEL3, DEBUG_POWER);
    select_value_for_debugging(DAC_CHANNEL4, DEBUGGING_DISABLED);
}
~~~

The selected values must then, e.g. periodically,
be transmitted to the available DACs.
The transmission buffer can be written using
the function write_debug_values_to_output(), e.g.:

~~~
void loop()
{
    ... // calculations

    write_debug_values_to_output(
        &spi_buffer
        );

    spi_transmit_to_dacs(&spi_buffer);
}
~~~

### analog-debugger.h

The firmware project must contain a file named analog-debugger.h,
which declares the following two types:

* dac_channel_e
* debug_value_e

#### dac_channel_e

An enumeration type must be declared containing a list of available DAC channels and the total number of DAC channels (as DAC_CHANNEL_COUNT).

For example, let's say the system features four DACs:

~~~
typedef enum
{
    DAC_CHANNEL1 = 0,
    DAC_CHANNEL2,
    DAC_CHANNEL3,
    DAC_CHANNEL4,
    DAC_CHANNEL_COUNT
} dac_channel_e;
~~~

#### debug_value_e

A a second enumeration type must list all the available values, which may be relevant for analog debugging within the specific project.

For example, let's say the system features one input voltage sensor and one output current sensor. One may wish to debug the following values:

~~~
typedef enum
{
    DEBUGGING_DISABLED = 0,
    DEBUG_INPUT_VOLTAGE,
    DEBUG_DUTYCYCLE,
    DEBUG_OUTPUT_CURRENT,
    DEBUG_POWER,
    DEBUG_VALUE_COUNT
} debug_value_e;
~~~
