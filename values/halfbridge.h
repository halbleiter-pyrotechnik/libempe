
#ifndef EMPE_HAlFBRIDGE_H
#define EMPE_HAlFBRIDGE_H


/**
 * The possible switching states of a half-bridge
 *
 * The decision, which states applies depends on the
 * properties of the used PWM unit and the FET driver
 * (see also @ref pwm_unit_properties_t).
 * It is made in the dutycycles-to-ticks conversion function.
 */
typedef enum
{
    HALFBRIDGE_DISABLED,
    REGULAR_PWM,
    FLAT_HIGH_HIGHSIDE,
    INTERMITTENT_HIGHSIDE,
    FLAT_HIGH_LOWSIDE,
    INTERMITTENT_LOWSIDE
} halfbridge_state_e;


/**
 * Holds the states of the three half-bridges
 * of a three-phase inverter system
 */
typedef union
{
    halfbridge_state_e state[3];
    struct
    {
        halfbridge_state_e state_u;
        halfbridge_state_e state_v;
        halfbridge_state_e state_w;
    };
} halfbridge_states_threephase_t;


#endif
