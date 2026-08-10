#ifndef SQ66_BEAMFORMER_CONFIG_H_
#define SQ66_BEAMFORMER_CONFIG_H_

#define BF_MIC_COUNT                     4
#define BF_SAMPLE_RATE_HZ                48000

/* First validation isolates delay-and-sum from the unverified AGC/noise gate. */
#define BF_ENABLE_POST_PROCESS           0

/* Must remain a power of two and exceed the largest configured delay. */
#define BF_DELAY_LINE_LENGTH             64
#define BF_DELAY_FRACTION_BITS           8

/*
 * Per-channel Q15 values are relative sensitivity calibration only. Start at
 * unity so no individual microphone can clip before the four-way average.
 */
#define BF_MIC_GAIN_Q15_0                32768
#define BF_MIC_GAIN_Q15_1                32768
#define BF_MIC_GAIN_Q15_2                32768
#define BF_MIC_GAIN_Q15_3                32768

/* Apply common gain only after averaging. 524288 in Q15 is 16.0x. */
#define BF_OUTPUT_GAIN_Q15               524288

/*
 * Steering compensation delays in Q8 samples. Zero delay on every channel is
 * a broadside beam and is the safe default until the final microphone geometry
 * is measured. Replace these values with output from tools/calc_delays.py.
 */
#define BF_DELAY_Q8_0                    0
#define BF_DELAY_Q8_1                    0
#define BF_DELAY_Q8_2                    0
#define BF_DELAY_Q8_3                    0

/* A first-order 70-Hz DC blocking filter coefficient in Q31 at 48 kHz. */
#define BF_DC_BLOCK_ALPHA_Q31            2127896178

#endif
