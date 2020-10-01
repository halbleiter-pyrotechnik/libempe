/**
 * Modulation values are different from dutycycles:
 * They range from -1.0 to +1.0.
 */


#ifndef MODULATION_H
#define MODULATION_H

#include "values/setpoints.h"
#include "../transformations/alphabeta.h"
#include "../functions/trapezoid.h"


#ifndef M_PI_2_3
#define M_PI_2_3 2.0943951023931953
#endif


/**
 * One needs three modulation values for one three-phase inverter system
 */
typedef three_phase_values_t modulation_values_ccm_t;


/**
 * Convert modulation degrees (between -1.0 and +1.0)
 * to dutycycles i.e. semiconductor turn-on durations (between 0.0 and 1.0).
 */
void convert_ccm_modulation_to_dutycycles(modulation_values_ccm_t*, dutycycles_threephase_t*);

/**
 * This function sets the lowside dutycycles according to the highside dutycycles,
 * such that continuous current mode (CCM) is realized:
 *   Highside dutycycle + Lowside dutycycle = 1.0
 */
void add_complementary_lowside_dutycycles_ccm(dutycycles_threephase_t*);

/**
 * Adds a function value to every phase
 *
 * @param angle     Current phase angle of the first phase in rad.
 *                  Incremented by 2/3*pi for the remaining two phases.
 */
void modulation_imprint_function(
        modulation_values_ccm_t*,
        trapezoid_t*,
        float angle
        );


#endif
