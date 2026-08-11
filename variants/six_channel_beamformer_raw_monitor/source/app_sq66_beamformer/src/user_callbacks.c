#include <stdint.h>

#include "beamformer.h"
#include "xua_audiohub.h"
#include "xua.h"
#include "xua_pdm_mic.h"

#if MIC_ARRAY_CONFIG_MIC_COUNT != BF_MIC_COUNT
#error Beamforming requires all BF_MIC_COUNT microphone channels.
#endif

#if NUM_USB_CHAN_IN < 6
#error The beam-plus-raw monitor requires six USB input channels.
#endif

static beamformer_state_t beamformer;
static unsigned raw_monitor[BF_MIC_COUNT];

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
    /* Preserve the four PDM PCM samples before replacing the USB-facing slots. */
    raw_monitor[0] = (unsigned) mic_audio[0];
    raw_monitor[1] = (unsigned) mic_audio[1];
    raw_monitor[2] = (unsigned) mic_audio[2];
    raw_monitor[3] = (unsigned) mic_audio[3];
    const int32_t processed = beamformer_process(&beamformer, mic_audio);

    /* USB 1-2 are beam stereo; UserBufferManagement supplies USB 3-6. */
    mic_audio[0] = processed;
    mic_audio[1] = processed;
}

void UserBufferManagement(unsigned samps_from_usb_to_audio[],
                          unsigned samps_from_audio_to_usb[])
{
    (void) samps_from_usb_to_audio;

    /* Slots 3-6 are outside the four-element PDM callback API. */
    samps_from_audio_to_usb[2] = raw_monitor[0];
    samps_from_audio_to_usb[3] = raw_monitor[1];
    samps_from_audio_to_usb[4] = raw_monitor[2];
    samps_from_audio_to_usb[5] = raw_monitor[3];
}
