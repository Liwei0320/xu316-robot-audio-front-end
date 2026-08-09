#ifndef SQ66_XUA_CONF_H_
#define SQ66_XUA_CONF_H_

#include <platform.h>

/* Capture-only UAC1 device. DATA0 and DATA1 are independent USB channels. */
#define NUM_USB_CHAN_OUT                 0
#define NUM_USB_CHAN_IN                  2
#define I2S_CHANS_DAC                    0
#define I2S_CHANS_ADC                    0

#define AUDIO_CLASS                      1
#define MIXER                            0
#define MAX_MIX_COUNT                    0

#define XUA_PDM_MIC_FREQ                 48000
#define MIN_FREQ                         XUA_PDM_MIC_FREQ
#define MAX_FREQ                         XUA_PDM_MIC_FREQ
#define DEFAULT_FREQ                     XUA_PDM_MIC_FREQ

#define MCLK_441                         (256 * 44100)
#define MCLK_48                          (256 * 48000)
#define XUA_USE_SW_PLL                   0
/* The fixed App PLL is rationally related to the 100 MHz reference clock. */
#define XUA_FB_USE_REF_CLOCK             1
#define XUA_FB_REF_MUL_48                384
#define XUA_FB_REF_DIV_48                3125
#define XUA_FB_REF_MUL_44                1764
#define XUA_FB_REF_DIV_44                15625
#define XUA_LOW_POWER_NON_STREAMING      0
/* Keep the PDM clock continuous across USB alternate-setting start/stop events. */
#define SQ66_PDM_ALWAYS_ON_CAPTURE        1
/* Tile 0 uses all eight hardware threads; XUD needs priority mode at 600 MHz. */
#define XUD_PRIORITY_HIGH                1

/* X1D11 is on tile 1 and is the PCB's physical MCLK source for X0D35. */
#define USER_MAIN_GLOBALS                extern "C" { void sq66_mclk_source(void); }
#define USER_MAIN_TASKS                  on tile[1]: sq66_mclk_source();

/*
 * DATA0..DATA3 occupy bits 4..7 of the 8-bit PDM input port. This build
 * captures DATA0 and DATA1 as two SDR microphones, one microphone per pin.
 */
#define XUA_NUM_PDM_MICS                 2
#define XUA_NUM_PDM_MICS_IN              8
#define XUA_PDM_MIC_USE_DDR              0
#define XUA_PDM_MIC_USE_PDM_ISR          0
#define XUA_PDM_MIC_NUM_DECIMATOR_SUBTASKS 1
#define XUA_PDM_MIC_SAMPLE_FALLING_EDGE  1
#define XUA_PDM_MIC_INDEX                0

/* USB, PDM capture, audiohub and the MCLK input are on tile 0. */
#define XUA_XUD_TILE_NUM                 0
#define XUA_AUDIO_IO_TILE_NUM            0
#define XUA_MIC_PDM_TILE_NUM             0
#define XUA_PLL_REF_TILE_NUM             0

#define VENDOR_STR                       "XMOS"
#define VENDOR_ID                        0x20B1
#define PRODUCT_STR_A2                   "XU316 Beamformer Audio Test 7"
#define PRODUCT_STR_A1                   "XU316 Beamformer Audio Test 7"
/* Use a fresh PID so Windows does not reuse the cached 24-bit endpoint. */
#define PID_AUDIO_2                      0x0326
#define PID_AUDIO_1                      0x0327
#define DFU_PID                          0xE326

#define STREAM_FORMAT_INPUT_1_RESOLUTION_BITS 16
#define INPUT_VOLUME_CONTROL             0
#define OUTPUT_VOLUME_CONTROL            0

/* Keep the first streaming test independent of DFU and the external flash. */
#define XUA_DFU_EN                       0
#define XUA_QUAD_SPI_FLASH               0
#define FLASH_MAX_UPGRADE_SIZE           (512 * 1024)

#define XUA_POWERMODE                    XUA_POWERMODE_BUS
#define HID_CONTROLS                     0
#define MIDI                             0
#define XUA_SPDIF_TX_EN                  0
#define XUA_SPDIF_RX_EN                  0
#define XUA_ADAT_TX_EN                   0
#define XUA_ADAT_RX_EN                  0

#endif
