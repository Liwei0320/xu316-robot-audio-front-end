#include "post_process.h"

#include <limits.h>

#define POST_GATE_MIN_Q15          512
#define POST_GATE_OPEN_Q15         32768
#define POST_GATE_ATTACK_STEP      512
#define POST_GATE_RELEASE_STEP     16
#define POST_GATE_HANGOVER_SAMPLES 4800

#define POST_MIN_VOICE_LEVEL       (1u << 20)
#define POST_AGC_TARGET_LEVEL      (0x18000000u)
#define POST_AGC_MIN_Q15           16384
#define POST_AGC_MAX_Q15           (16 * 32768)
#define POST_LIMIT                 (0x70000000)

static int64_t round_shift_signed(int64_t value, unsigned shift)
{
    const int64_t rounding = (int64_t)1 << (shift - 1u);
    if (value >= 0) {
        return (value + rounding) >> shift;
    }
    return -(((-value) + rounding) >> shift);
}

static uint32_t magnitude_i32(int32_t value)
{
    if (value >= 0) {
        return (uint32_t) value;
    }
    if (value == INT32_MIN) {
        return (uint32_t) INT32_MAX;
    }
    return (uint32_t) (-value);
}

static int32_t saturate_i32(int64_t value)
{
    if (value > INT32_MAX) {
        return INT32_MAX;
    }
    if (value < INT32_MIN) {
        return INT32_MIN;
    }
    return (int32_t) value;
}

void post_process_init(post_process_state_t *state)
{
    state->envelope = 0;
    state->noise_floor = POST_MIN_VOICE_LEVEL;
    state->agc_gain_q15 = 32768;
    state->gate_gain_q15 = POST_GATE_OPEN_Q15;
    state->gate_hangover = 0;
    state->agc_counter = 0;
}

int32_t post_process_sample(post_process_state_t *state, int32_t sample)
{
    const uint32_t magnitude = magnitude_i32(sample);

    if (magnitude > state->envelope) {
        uint32_t step = (magnitude - state->envelope) >> 3;
        state->envelope += step ? step : 1;
    } else if (magnitude < state->envelope) {
        uint32_t step = (state->envelope - magnitude) >> 10;
        state->envelope -= step ? step : 1;
    }

    uint32_t threshold = state->noise_floor;
    if (threshold < (UINT32_MAX / 4u)) {
        threshold *= 4u;
    } else {
        threshold = UINT32_MAX;
    }
    if (threshold < POST_MIN_VOICE_LEVEL) {
        threshold = POST_MIN_VOICE_LEVEL;
    }

    if (state->envelope > threshold) {
        state->gate_hangover = POST_GATE_HANGOVER_SAMPLES;
    } else if (state->gate_hangover > 0) {
        --state->gate_hangover;
    } else {
        if (state->envelope > state->noise_floor) {
            uint32_t step = (state->envelope - state->noise_floor) >> 12;
            state->noise_floor += step ? step : 1;
        } else if (state->envelope < state->noise_floor) {
            uint32_t step = (state->noise_floor - state->envelope) >> 12;
            state->noise_floor -= step ? step : 1;
        }
    }

    const int32_t gate_target = state->gate_hangover
        ? POST_GATE_OPEN_Q15 : POST_GATE_MIN_Q15;
    if (state->gate_gain_q15 < gate_target) {
        state->gate_gain_q15 += POST_GATE_ATTACK_STEP;
        if (state->gate_gain_q15 > gate_target) {
            state->gate_gain_q15 = gate_target;
        }
    } else if (state->gate_gain_q15 > gate_target) {
        state->gate_gain_q15 -= POST_GATE_RELEASE_STEP;
        if (state->gate_gain_q15 < gate_target) {
            state->gate_gain_q15 = gate_target;
        }
    }

    if (++state->agc_counter >= 64u) {
        const uint32_t level = state->envelope ? state->envelope : 1u;
        int64_t desired = ((int64_t) POST_AGC_TARGET_LEVEL << 15) / level;
        if (desired < POST_AGC_MIN_Q15) {
            desired = POST_AGC_MIN_Q15;
        }
        if (desired > POST_AGC_MAX_Q15) {
            desired = POST_AGC_MAX_Q15;
        }

        if (desired > state->agc_gain_q15) {
            state->agc_gain_q15 += (int32_t) ((desired - state->agc_gain_q15) >> 6);
        } else {
            state->agc_gain_q15 -= (int32_t) ((state->agc_gain_q15 - desired) >> 6);
        }
        state->agc_counter = 0;
    }

    int64_t output = round_shift_signed(
        (int64_t) sample * state->agc_gain_q15, 15);
    output = round_shift_signed(output * state->gate_gain_q15, 15);
    if (output > POST_LIMIT) {
        output = POST_LIMIT;
    } else if (output < -POST_LIMIT) {
        output = -POST_LIMIT;
    }
    return saturate_i32(output);
}
