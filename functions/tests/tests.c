/**
 * This test plots the following functions:
 *  - triangle
 *  - sawtooth
 *  - ramp
 *  - trapezoid
 *
 * Nominal function frequency is 50 Hz,
 * one calculation step is one degree (360 degree per period).
 * Results are stored in csv files.
 */

#include "tests.h"

int main()
{
    test_sawtooth("sawtooth.csv");
    test_ramp("ramp.csv");
    test_triangle("triangle.csv");
    test_trapezoid("trapezoid.csv");
}
