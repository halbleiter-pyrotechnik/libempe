
#include <stdio.h>
#include <stdint.h>
#include "sawtooth.h"

void test_sawtooth(char* filename)
{
    FILE* f;
    f = fopen(filename, "w");

    /*
     * Test function parameters
     */
    const float value_start = -3.0;
    const float value_stop = 42.0;
    const float frequency_function = 50.0;
    const float frequency_update = 14e3;
    const float runtime = 0.25;

    sawtooth_t function;
    sawtooth_init(
            &function,
            value_start,
            value_stop,
            frequency_function,
            frequency_update
            );

    for (uint32_t i=0; i<(uint32_t) roundf(runtime*frequency_update); i++)
    {
        // Calculate momentary function value
        sawtooth_update(&function);

        fprintf(f, "%d;%.2f;\n", i, function.f.value);

        // Increment function by one step
        function_step(&function.f);
    }

    fclose(f);
}
