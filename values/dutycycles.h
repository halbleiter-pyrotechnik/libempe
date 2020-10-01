
#ifndef EMPE_DUTYCYCLES_H
#define EMPE_DUTYCYCLES_H


#include <stdint.h>
#include <stdbool.h>


/**
 * Struct to hold all six duty-cycle values
 * for a three-phase inverter system
 *
 * Duty-cycle values range from 0.0 (0%) to 1.0 (100%).
 */
typedef union
{
    /**
     * Array indices:
     *  0: High-side switch phase U
     *  1: High-side switch phase V
     *  2: High-side switch phase W
     *  3: Low-side switch phase U
     *  4: Low-side switch phase V
     *  5: Low-side switch phase W
     */
    float dutycycle[6];
    struct
    {
        float dutycycle_highside[3];
        float dutycycle_lowside [3];
    };
    struct
    {
        float dutycycle_highside_u;
        float dutycycle_highside_v;
        float dutycycle_highside_w;
        float dutycycle_lowside_u;
        float dutycycle_lowside_v;
        float dutycycle_lowside_w;
    };
} dutycycles_threephase_t;


/**
 * This function makes sure the dutycycles
 * lie within the acceptable range.
 */
bool check_dutycycles(dutycycles_threephase_t*);


#endif
