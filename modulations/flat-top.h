/**
 * This file declares types and functions for flat-top modulation.
 */

#ifndef FLAT_TOP_H
#define FLAT_TOP_H


#include "../applications/pll.h"
#include "../modulations/modulation.h"
#include "../transformations/direct-quadrature.h"


#define FLATTOP_INDICATOR_INACTIVE      0.5
#define FLATTOP_INDICATOR_ACTIVE_TOP    1.0
#define FLATTOP_INDICATOR_ACTIVE_BOTTOM 0.0


/**
 * This function will modify the modulation degrees
 * depending on the current grid phase angle theta
 * in such a way, that switching is disabled
 * below respectively above certain thresholds.
 * This is done in order to reduce switching losses.
 */
void apply_flat_top_ccm(
        modulation_values_ccm_t* modulation,
        float modulation_amplitude,
        float theta
        );


#endif
