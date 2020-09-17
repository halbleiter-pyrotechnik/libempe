
#ifndef SIMULTANEOUS_TURNON
#define SIMULTANEOUS_TURNON


/**
 * Possible switching modes
 */
typedef enum
{
    SWITCHING_DISABLED,
    SIMULTANEOUS_HIGHSIDE_TURNON,
    SIMULTANEOUS_LOWSIDE_TURNON,
    PERIOD_CENTERED_SWITCHING
} switching_mode_e;


/**
 * Decides the switching mode according to the momentary phase angle
 */
void update_switching_mode(float phase_angle_rad, switching_mode_e*);


#endif
