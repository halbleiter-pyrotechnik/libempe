
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "triangle.h"

void test_triangle(char* filename)
{
    FILE* f;
    f = fopen(filename, "w");

    /*
     * Test function parameters
     */
    const float value_min = -3.0;
    const float value_max = 5.0;
    const float dutycycle = 0.35;
    const float frequency_function = 25.0;
    const float frequency_update = 1e3;
    const float runtime = 0.2;

    triangle_t function;
    triangle_init_with_dutycycle(
            &function,
            value_min,
            value_max,
            dutycycle,
            frequency_function,
            frequency_update
            );

    for (uint32_t i=0; i<(uint32_t) roundf(runtime*frequency_update); i++)
    {
        // Calculate momentary function value
        triangle_update(&function);

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
