#include <stdint.h>

#include "xcore/parallel.h"

#include "parallel_decimator.h"
#include "xua_conf.h"

#if XUA_PDM_MIC_NUM_DECIMATOR_SUBTASKS > 1

#if XUA_PDM_MIC_NUM_DECIMATOR_SUBTASKS != 2
#error This implementation supports exactly two decimator subtasks.
#endif

DECLARE_JOB(sq66_decimator_subtask, (
    unsigned,
    unsigned,
    unsigned,
    uint32_t *,
    const uint32_t *,
    unsigned,
    filter_fir_s32_t *,
    uint32_t *,
    int32_t *));

static inline void shift_history(uint32_t *history)
{
  uint32_t *source = &history[-1];
  asm volatile("vldd %0[0]; vstd %1[0];"
               :
               : "r"(source), "r"(history)
               : "memory");
}

void sq66_decimator_subtask(
    unsigned first_mic,
    unsigned mic_stride,
    unsigned num_mics,
    uint32_t *stage1_history,
    const uint32_t *stage1_coefficients,
    unsigned stage2_decimation_factor,
    filter_fir_s32_t *stage2_filters,
    uint32_t *pdm_block,
    int32_t *sample_out)
{
  uint32_t (*pdm_data)[stage2_decimation_factor] =
      (uint32_t (*)[stage2_decimation_factor])pdm_block;

  for (unsigned mic = first_mic; mic < num_mics; mic += mic_stride) {
    uint32_t *history = &stage1_history[mic * 8];

    for (unsigned word = 0; word < stage2_decimation_factor; ++word) {
      history[0] = pdm_data[mic][word];
      int32_t stage1_sample = fir_1x16_bit(history, stage1_coefficients);
      shift_history(history);

      if (word + 1 < stage2_decimation_factor) {
        filter_fir_s32_add_sample(&stage2_filters[mic], stage1_sample);
      } else {
        sample_out[mic] = filter_fir_s32(&stage2_filters[mic], stage1_sample);
      }
    }
  }
}

void sq66_parallel_decimator_run(
    unsigned num_mics,
    uint32_t *stage1_history,
    const uint32_t *stage1_coefficients,
    unsigned stage2_decimation_factor,
    filter_fir_s32_t *stage2_filters,
    uint32_t *pdm_block,
    int32_t *sample_out)
{
  PAR_JOBS(
      PJOB(sq66_decimator_subtask,
           (0, 2, num_mics,
            stage1_history, stage1_coefficients,
            stage2_decimation_factor, stage2_filters,
            pdm_block, sample_out)),
      PJOB(sq66_decimator_subtask,
           (1, 2, num_mics,
            stage1_history, stage1_coefficients,
            stage2_decimation_factor, stage2_filters,
            pdm_block, sample_out)));
}

#endif
