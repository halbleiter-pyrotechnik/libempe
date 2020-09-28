
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "ramp.h"

void test_ramp(char* filename)
{
    FILE* f;
    f = fopen(filename, "w");

    /*
     * Test function parameters
     */
    const float value_start = -3.0;
    const float value_stop = 5.0;
    const float ramp_up_time = 0.1;
    const float frequency_update = 1e3;
    const float runtime = 0.2;

    ramp_t function;
    ramp_init(
            &function,
            value_start,
            value_stop,
            ramp_up_time,
            frequency_update
            );

    for (uint32_t i=0; i<(uint32_t) roundf(runtime*frequency_update); i++)
    {
        // Calculate momentary function value
        ramp_update(&function);

        fprintf(f, "%d;%.2f;%.2f;\n",
                i,
                function.f.angle,
                function.f.value
                );

        // Increment function by one step
        function_step(&function.f);
    }

    fclose(f);
}
