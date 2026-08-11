#ifndef SQ66_BEAMFORMER_H_
#define SQ66_BEAMFORMER_H_

#include <stdint.h>

#include "beamformer_config.h"
#include "post_process.h"

typedef struct beamformer_state {
    int32_t delay_line[BF_MIC_COUNT][BF_DELAY_LINE_LENGTH];
    int32_t dc_previous_input[BF_MIC_COUNT];
    int32_t dc_previous_output[BF_MIC_COUNT];
    int32_t gain_q15[BF_MIC_COUNT];
    uint16_t delay_q8[BF_MIC_COUNT];
    uint32_t write_index;
    uint32_t dc_initialized_mask;
    post_process_state_t post;
} beamformer_state_t;

void beamformer_init(beamformer_state_t *state);
void beamformer_set_delays_q8(beamformer_state_t *state,
                              const uint32_t delays_q8[BF_MIC_COUNT]);
int32_t beamformer_process(beamformer_state_t *state,
                           const int32_t microphones[BF_MIC_COUNT]);

#endif
