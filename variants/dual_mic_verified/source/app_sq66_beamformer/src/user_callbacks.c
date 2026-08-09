#include <limits.h>
#include <stdint.h>

#include "beamformer.h"
#include "xua.h"
#include "xua_pdm_mic.h"

#if BF_DUAL_MIC_BRINGUP && (MIC_ARRAY_CONFIG_MIC_COUNT < 2)
#error BF_DUAL_MIC_BRINGUP requires DATA0 and DATA1.
#endif

#if !BF_DUAL_MIC_BRINGUP && (MIC_ARRAY_CONFIG_MIC_COUNT < BF_MIC_COUNT)
#error Beamforming requires all BF_MIC_COUNT microphone channels.
#endif

#if !BF_DUAL_MIC_BRINGUP
static beamformer_state_t beamformer;
#endif

static int32_t scale_mic(int32_t sample, int32_t gain)
{
    const int64_t scaled = (int64_t)sample * gain;
    if (scaled > INT32_MAX) {
        return INT32_MAX;
    }
    if (scaled < INT32_MIN) {
        return INT32_MIN;
    }
    return (int32_t)scaled;
}

void xua_user_pdm_init(unsigned channel_map[MIC_ARRAY_CONFIG_MIC_COUNT])
{
    /* DATA0/X0D40 and DATA1/X0D41 are P8D bits 4 and 5. */
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
#if !BF_DUAL_MIC_BRINGUP
    beamformer_init(&beamformer);
#endif
}

void xua_user_pdm_process(int32_t mic_audio[MIC_ARRAY_CONFIG_MIC_COUNT])
{
#if BF_DUAL_MIC_BRINGUP
    mic_audio[0] = scale_mic(mic_audio[0], BF_DUAL_MIC_GAIN_DATA0);
    mic_audio[1] = scale_mic(mic_audio[1], BF_DUAL_MIC_GAIN_DATA1);
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
