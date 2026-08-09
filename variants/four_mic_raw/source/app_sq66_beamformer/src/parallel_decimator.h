#ifndef SQ66_PARALLEL_DECIMATOR_H_
#define SQ66_PARALLEL_DECIMATOR_H_

#include <stdint.h>

#include "xmath/xmath.h"
#include "mic_array/etc/fir_1x16_bit.h"

#ifdef __cplusplus
extern "C" {
#endif

void sq66_parallel_decimator_run(
    unsigned num_mics,
    uint32_t *stage1_history,
    const uint32_t *stage1_coefficients,
    unsigned stage2_decimation_factor,
    filter_fir_s32_t *stage2_filters,
    uint32_t *pdm_block,
    int32_t *sample_out);

#ifdef __cplusplus
}
#endif

#endif
