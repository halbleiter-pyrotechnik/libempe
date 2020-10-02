
#include "modulation.h"


inline void add_complementary_lowside_dutycycles_ccm(
                dutycycles_threephase_t* setpoints
                )
{
    /*
     * In CCM highside and lowside on-durations add up to 100% switching period.
     */
    for (uint8_t i=0; i<3; i++)
    {
        setpoints->dutycycle_lowside[i]  = 1.0 - setpoints->dutycycle_highside[i];
    }
}


inline void convert_ccm_modulation_to_dutycycles(
        modulation_values_ccm_t* modulation,
        dutycycles_threephase_t* setpoints
        )
{
    for (uint8_t i=0; i<3; i++)
    {
        float d = 0.5 + modulation->value[i]/2.0;
        if (d > 1.0)
            d = 1.0;
        else if (d < 0.0)
            d = 0.0;
        setpoints->dutycycle_highside[i] = d;
    }

    add_complementary_lowside_dutycycles_ccm(setpoints);
}


void modulation_imprint_function(
        modulation_values_ccm_t* m,
        trapezoid_t* f,
        float angle
        )
{
    if (m == 0)
        return;
    if (f == 0)
        return;

    f->f.angle = angle;
    for (uint8_t i=0; i<3; i++)
    {
        float bias = trapezoid_update(f);
        m->value[i] += bias;
        f->f.angle += M_PI_2_3;
        if (f->f.angle > M_2x_PI)
            f->f.angle -= M_2x_PI;
    }
}

