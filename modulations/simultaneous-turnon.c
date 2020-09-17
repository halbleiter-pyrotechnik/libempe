
#include "simultaneous-turnon.h"
#include "math.h"


void update_switching_mode(
        float phase_angle_rad,
        switching_mode_e* mode
        )
{
    // Normalize angle
    while (phase_angle_rad < 0.0f)
        phase_angle_rad += M_TWOPI;
    while (phase_angle_rad > M_TWOPI)
        phase_angle_rad -= M_TWOPI;

    switching_mode_e result = SIMULTANEOUS_LOWSIDE_TURNON;

    static const float threshold5 = 5.0/6.0 * M_TWOPI;
    static const float threshold4 = 4.0/6.0 * M_TWOPI;
    static const float threshold3 = 3.0/6.0 * M_TWOPI;
    static const float threshold2 = 2.0/6.0 * M_TWOPI;
    static const float threshold1 = 1.0/6.0 * M_TWOPI;

    if (phase_angle_rad > threshold5)
        result = SIMULTANEOUS_HIGHSIDE_TURNON;
    else if (phase_angle_rad > threshold4)
        result = SIMULTANEOUS_LOWSIDE_TURNON;
    else if (phase_angle_rad > threshold3)
        result = SIMULTANEOUS_HIGHSIDE_TURNON;
    else if (phase_angle_rad > threshold2)
        result = SIMULTANEOUS_LOWSIDE_TURNON;
    else if (phase_angle_rad > threshold1)
        result = SIMULTANEOUS_HIGHSIDE_TURNON;

    *mode = result;
}
