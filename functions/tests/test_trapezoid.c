
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "trapezoid.h"

void test_trapezoid(char* filename)
{
    FILE* f;
    f = fopen(filename, "w");

    /*
     * Test function parameters
     */
    const float slope_rising = 2.0;
    const float slope_falling = -5.5;
    const float cutoff_min = -3.0;
    const float cutoff_max = 5.0;
    const float frequency_function = 25.0;
    const float frequency_update = 1e3;
    const float runtime = 0.2;

    trapezoid_t function;
    trapezoid_init(
            &function,
            slope_rising,
            slope_falling,
            cutoff_min,
            cutoff_max,
            frequency_function,
            frequency_update
            );

    const float deg = 180.0 / M_PI;
    fprintf(stdout, "Trapezoid\n--------------\n");
    fprintf(stdout, "Upper plateau: %.2f\n", function.f.value_max);
    fprintf(stdout, "Upper plateau start: %.2f deg\n", function.angle_stop_rising * deg);
    fprintf(stdout, "Upper plateau stop:  %.2f deg\n", function.angle_start_falling * deg);
    fprintf(stdout, "Lower plateau: %.2f\n", function.f.value_min);
    fprintf(stdout, "Lower plateau start: %.2f deg\n", function.angle_stop_falling * deg);
    fprintf(stdout, "Lower plateau stop:  %.2f deg\n", function.angle_restart_rising * deg);

    for (uint32_t i=0; i<(uint32_t) roundf(runtime*frequency_update); i++)
    {
        // Calculate momentary function value
        trapezoid_update(&function);

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
