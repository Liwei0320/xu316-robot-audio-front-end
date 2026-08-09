#include <limits.h>
#include <stdint.h>

#include "beamformer.h"
#include "xua.h"
#include "xua_pdm_mic.h"

static beamformer_state_t beamformer;

static int32_t scale_single_mic(int32_t sample)
{
    const int64_t scaled = (int64_t)sample * BF_SINGLE_MIC_GAIN;
    if (scaled > INT32_MAX) {
        return INT32_MAX;
    }
    if (scaled < INT32_MIN) {
        return INT32_MIN;
    }
    return (int32_t)scaled;
}

#if BF_SINGLE_MIC_USB_TEST_TONE
static int32_t diagnostic_test_tone(void)
{
    static unsigned phase;
    const int32_t sample_16 = phase < 24
        ? -12000 + (int32_t)phase * 1000
        : 12000 - (int32_t)(phase - 24) * 1000;

    phase = (phase + 1) % 48;
    return sample_16 * 65536;
}
#endif

void xua_user_pdm_init(unsigned channel_map[MIC_ARRAY_CONFIG_MIC_COUNT])
{
    /* X0D40..43 are bits 4..7 of XS1_PORT_8D. */
    channel_map[0] = 4;
#if MIC_ARRAY_CONFIG_MIC_COUNT > 1
    channel_map[1] = 5;
#endif
#if MIC_ARRAY_CONFIG_MIC_COUNT > 2
    channel_map[2] = 6;
#endif
#if MIC_ARRAY_CONFIG_MIC_COUNT > 3
    channel_map[3] = 7;
#endif
    beamformer_init(&beamformer);
}

void xua_user_pdm_process(int32_t mic_audio[MIC_ARRAY_CONFIG_MIC_COUNT])
{
#if BF_SINGLE_MIC_BRINGUP
    const int32_t single_mic = scale_single_mic(mic_audio[0]);
    mic_audio[0] = single_mic;
#if BF_SINGLE_MIC_USB_TEST_TONE
    mic_audio[1] = diagnostic_test_tone();
#elif BF_SINGLE_MIC_COMPARE_DATA1
    mic_audio[1] = scale_single_mic(mic_audio[1]);
#else
    mic_audio[1] = single_mic;
#endif
#if MIC_ARRAY_CONFIG_MIC_COUNT > 2
    mic_audio[2] = 0;
#endif
#if MIC_ARRAY_CONFIG_MIC_COUNT > 3
    mic_audio[3] = 0;
#endif
    return;
#else

#if BF_USB_RIGHT_CHANNEL_RAW_DEBUG
    const int32_t raw_mic_0 = mic_audio[0];
#endif
    const int32_t processed = beamformer_process(&beamformer, mic_audio);

    mic_audio[0] = processed;
#if BF_USB_RIGHT_CHANNEL_RAW_DEBUG
    mic_audio[1] = raw_mic_0;
#else
    mic_audio[1] = processed;
#endif
#if MIC_ARRAY_CONFIG_MIC_COUNT > 2
    mic_audio[2] = 0;
#endif
#if MIC_ARRAY_CONFIG_MIC_COUNT > 3
    mic_audio[3] = 0;
#endif
#endif
}
