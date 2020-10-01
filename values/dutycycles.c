
#include "dutycycles.h"
#include <stdint.h>
#include <math.h>


bool check_dutycycles(dutycycles_threephase_t* setpoints)
{
    bool made_correction = false;

    /*
     * Dutycycles may not exceed below 0% or above 100%
     */
    for (uint8_t i=0; i<sizeof(setpoints->dutycycle)/sizeof(setpoints->dutycycle[0]); i++)
    {
        if (isnan(setpoints->dutycycle[i]))
        {
            setpoints->dutycycle[i] = 0.0;
            made_correction = true;
        }
        else if (setpoints->dutycycle[i] > 1.0)
        {
            setpoints->dutycycle[i] = 1.0;
            made_correction = true;
        }
        else if (setpoints->dutycycle[i] < 0.0)
        {
            setpoints->dutycycle[i] = 0.0;
            made_correction = true;
        }
    }

    return made_correction;
}

