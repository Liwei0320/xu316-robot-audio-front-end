#include "beamformer.h"

#include <limits.h>
#include <string.h>

#if BF_MIC_COUNT != 4
#error This implementation requires exactly four microphones
#endif

#if (BF_DELAY_LINE_LENGTH < 2) \
    || ((BF_DELAY_LINE_LENGTH & (BF_DELAY_LINE_LENGTH - 1)) != 0)
#error BF_DELAY_LINE_LENGTH must be a power of two and at least two
#endif

#if (BF_DELAY_FRACTION_BITS < 1) || (BF_DELAY_FRACTION_BITS > 15)
#error BF_DELAY_FRACTION_BITS must be between 1 and 15
#endif

#if (((BF_DELAY_LINE_LENGTH - 2) << BF_DELAY_FRACTION_BITS) > UINT16_MAX)
#error Configured delay range does not fit in beamformer_state_t
#endif

#define BF_DELAY_MASK (BF_DELAY_LINE_LENGTH - 1)
#define BF_MAX_DELAY_Q8 ((BF_DELAY_LINE_LENGTH - 2) << BF_DELAY_FRACTION_BITS)

static int64_t round_shift_signed(int64_t value, unsigned shift)
{
    const int64_t rounding = (int64_t)1 << (shift - 1u);
    if (value >= 0) {
        return (value + rounding) >> shift;
    }
    return -(((-value) + rounding) >> shift);
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

static int32_t dc_block_and_calibrate(beamformer_state_t *state,
                                      unsigned microphone,
                                      int32_t input)
{
    const uint32_t initialized_bit = 1u << microphone;
    if ((state->dc_initialized_mask & initialized_bit) == 0) {
        state->dc_previous_input[microphone] = input;
        state->dc_previous_output[microphone] = 0;
        state->dc_initialized_mask |= initialized_bit;
        return 0;
    }

    int64_t output = (int64_t) input - state->dc_previous_input[microphone];
    output += round_shift_signed((int64_t) BF_DC_BLOCK_ALPHA_Q31
                                 * state->dc_previous_output[microphone], 31);

    const int32_t filtered = saturate_i32(output);
    state->dc_previous_input[microphone] = input;
    state->dc_previous_output[microphone] = filtered;

    output = round_shift_signed(
        (int64_t) filtered * state->gain_q15[microphone], 15);
    return saturate_i32(output);
}

static int32_t read_fractional_delay(const beamformer_state_t *state,
                                     unsigned microphone)
{
    const uint16_t delay_q8 = state->delay_q8[microphone];
    const unsigned whole = delay_q8 >> BF_DELAY_FRACTION_BITS;
    const unsigned fraction = delay_q8 & ((1u << BF_DELAY_FRACTION_BITS) - 1u);

    const unsigned newer_index = (state->write_index - whole) & BF_DELAY_MASK;
    const unsigned older_index = (newer_index - 1u) & BF_DELAY_MASK;
    const int32_t newer = state->delay_line[microphone][newer_index];
    const int32_t older = state->delay_line[microphone][older_index];

    const int64_t delta = (int64_t) older - newer;
    return saturate_i32((int64_t) newer
        + round_shift_signed(delta * fraction, BF_DELAY_FRACTION_BITS));
}

void beamformer_init(beamformer_state_t *state)
{
    static const int32_t gains[BF_MIC_COUNT] = {
        BF_MIC_GAIN_Q15_0,
        BF_MIC_GAIN_Q15_1,
        BF_MIC_GAIN_Q15_2,
        BF_MIC_GAIN_Q15_3
    };
    static const uint32_t delays[BF_MIC_COUNT] = {
        BF_DELAY_Q8_0,
        BF_DELAY_Q8_1,
        BF_DELAY_Q8_2,
        BF_DELAY_Q8_3
    };

    memset(state, 0, sizeof(*state));
    for (unsigned microphone = 0; microphone < BF_MIC_COUNT; ++microphone) {
        state->gain_q15[microphone] = gains[microphone];
    }
    beamformer_set_delays_q8(state, delays);
    post_process_init(&state->post);
}

void beamformer_set_delays_q8(beamformer_state_t *state,
                              const uint32_t delays_q8[BF_MIC_COUNT])
{
    for (unsigned microphone = 0; microphone < BF_MIC_COUNT; ++microphone) {
        uint32_t delay = delays_q8[microphone];
        if (delay > BF_MAX_DELAY_Q8) {
            delay = BF_MAX_DELAY_Q8;
        }
        state->delay_q8[microphone] = (uint16_t) delay;
    }
}

int32_t beamformer_process(beamformer_state_t *state,
                           const int32_t microphones[BF_MIC_COUNT])
{
    int64_t sum = 0;

    for (unsigned microphone = 0; microphone < BF_MIC_COUNT; ++microphone) {
        state->delay_line[microphone][state->write_index] =
            dc_block_and_calibrate(state, microphone, microphones[microphone]);
    }

    for (unsigned microphone = 0; microphone < BF_MIC_COUNT; ++microphone) {
        sum += read_fractional_delay(state, microphone);
    }

    state->write_index = (state->write_index + 1u) & BF_DELAY_MASK;
    const int32_t average = saturate_i32(sum / BF_MIC_COUNT);
    const int32_t beam = saturate_i32(round_shift_signed(
        (int64_t) average * BF_OUTPUT_GAIN_Q15, 15));
#if BF_ENABLE_POST_PROCESS
    return post_process_sample(&state->post, beam);
#else
    return beam;
#endif
}
