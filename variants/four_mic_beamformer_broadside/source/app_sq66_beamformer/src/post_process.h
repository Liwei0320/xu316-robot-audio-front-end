#ifndef SQ66_POST_PROCESS_H_
#define SQ66_POST_PROCESS_H_

#include <stdint.h>

typedef struct post_process_state {
    uint32_t envelope;
    uint32_t noise_floor;
    int32_t agc_gain_q15;
    int32_t gate_gain_q15;
    uint32_t gate_hangover;
    uint32_t agc_counter;
} post_process_state_t;

void post_process_init(post_process_state_t *state);
int32_t post_process_sample(post_process_state_t *state, int32_t sample);

#endif
