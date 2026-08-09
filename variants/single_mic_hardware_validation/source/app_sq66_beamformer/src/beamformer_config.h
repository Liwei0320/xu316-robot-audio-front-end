#ifndef SQ66_BEAMFORMER_CONFIG_H_
#define SQ66_BEAMFORMER_CONFIG_H_

#define BF_MIC_COUNT                     4
#define BF_SAMPLE_RATE_HZ                48000

/* Bring-up mode: send DATA0 directly to USB without beamforming. */
#define BF_SINGLE_MIC_BRINGUP            1
#define BF_SINGLE_MIC_GAIN               64

/* Compare DATA0 against the unconnected DATA1 input during bring-up. */
#define BF_SINGLE_MIC_USB_TEST_TONE       0
#define BF_SINGLE_MIC_COMPARE_DATA1       1

/* Must remain a power of two and exceed the largest configured delay. */
#define BF_DELAY_LINE_LENGTH             64
#define BF_DELAY_FRACTION_BITS           8

/*
 * Per-channel calibration gain in Q15. 262144 is 8.0x. The official PDM
 * example applies 64x; this lower starting value leaves bring-up headroom.
 */
#define BF_MIC_GAIN_Q15_0                262144
#define BF_MIC_GAIN_Q15_1                262144
#define BF_MIC_GAIN_Q15_2                262144
#define BF_MIC_GAIN_Q15_3                262144

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

/* USB channel 1 can carry the processed beam or raw microphone 0. */
#define BF_USB_RIGHT_CHANNEL_RAW_DEBUG   1

#endif
