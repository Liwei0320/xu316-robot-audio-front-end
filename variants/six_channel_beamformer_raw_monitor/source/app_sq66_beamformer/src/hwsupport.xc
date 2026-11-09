#include <platform.h>
#include <timer.h>
#include <xs1.h>

#include "xua.h"

extern "C" {
void sw_pll_fixed_clock(const unsigned frequency);
}

on tile[1]: out port p_sq66_pll_pad = PORT_PLL_MONITOR;
on tile[0]: out port p_sq66_led_rg = PORT_LED_RG;

extern "C" {
void sq66_record_led_write(unsigned lit)
{
    /* X0D32 is bit 2, active low; bit 3 (X0D33/LED_G) stays high. */
    p_sq66_led_rg <: (lit ? 0x8 : 0xC);
}
}

void sq66_mclk_source(void)
{
    p_sq66_pll_pad <: 0;
    asm volatile ("setc res[%0], %1"
                  :: "r" (p_sq66_pll_pad), "r" (0x200006));
    sw_pll_fixed_clock(MCLK_48);

    while (1) {
        delay_milliseconds(1000);
    }
}

/* No external codec or clock generator is required for USB microphone mode. */
void AudioHwInit(void)
{
    sq66_record_led_write(0);
}

void AudioHwConfig(unsigned sample_rate,
                   unsigned master_clock,
                   unsigned dsd_mode,
                   unsigned dac_sample_resolution,
                   unsigned adc_sample_resolution)
{
    (void) sample_rate;
    (void) master_clock;
    (void) dsd_mode;
    (void) dac_sample_resolution;
    (void) adc_sample_resolution;
}
