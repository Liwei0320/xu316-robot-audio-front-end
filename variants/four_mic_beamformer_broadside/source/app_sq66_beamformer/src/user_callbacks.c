#include <stdint.h>

#include "beamformer.h"
#include "xua.h"
#include "xua_pdm_mic.h"

#if MIC_ARRAY_CONFIG_MIC_COUNT != BF_MIC_COUNT
#error Beamforming requires all BF_MIC_COUNT microphone channels.
#endif

static beamformer_state_t beamformer;

void xua_user_pdm_init(unsigned channel_map[MIC_ARRAY_CONFIG_MIC_COUNT])
{
    /* DATA0..DATA3/X0D40..43 are P8D bits 4..7. */
    channel_map[0] = 4;
    channel_map[1] = 5;
    channel_map[2] = 6;
    channel_map[3] = 7;
    beamformer_init(&beamformer);
}

void xua_user_pdm_process(int32_t mic_audio[MIC_ARRAY_CONFIG_MIC_COUNT])
{
    const int32_t processed = beamformer_process(&beamformer, mic_audio);

    /* Present the processed mono beam as conventional USB stereo. */
    mic_audio[0] = processed;
    mic_audio[1] = processed;
    mic_audio[2] = 0;
    mic_audio[3] = 0;
}
