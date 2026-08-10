#ifndef SQ66_PARALLEL_DECIMATOR_HPP_
#define SQ66_PARALLEL_DECIMATOR_HPP_

#include <cassert>
#include <cstdint>
#include <cstring>

#include "mic_array.h"
#include "parallel_decimator.h"

namespace mic_array {

template <unsigned MIC_COUNT, unsigned S2_DEC_FACTOR, unsigned S2_TAP_COUNT>
class ParallelTwoStageDecimator
{
  private:
    struct {
      const uint32_t *filter_coefficients;
      uint32_t history[MIC_COUNT][8]
#ifndef __DOXYGEN__
          = {[0 ... (MIC_COUNT - 1)] = {[0 ... 7] = 0x55555555}}
#endif
          ;
    } stage1;

    struct {
      filter_fir_s32_t filters[MIC_COUNT];
      int32_t filter_state[MIC_COUNT][S2_TAP_COUNT] = {{0}};
    } stage2;

  public:
    constexpr ParallelTwoStageDecimator() noexcept { }

    void Init(mic_array_decimator_conf_t &config)
    {
      assert(config.num_filter_stages == 2);
      assert(config.filter_conf[0].state_words_per_channel == 8);
      assert(config.filter_conf[1].decimation_factor == S2_DEC_FACTOR);
      assert(config.filter_conf[1].num_taps == S2_TAP_COUNT);

      stage1.filter_coefficients =
          reinterpret_cast<const uint32_t *>(config.filter_conf[0].coef);
      std::memset(stage1.history, 0x55, sizeof(stage1.history));
      std::memset(stage2.filter_state, 0, sizeof(stage2.filter_state));

      for (unsigned mic = 0; mic < MIC_COUNT; ++mic) {
        mic_array_filter_conf_t &filter_config = config.filter_conf[1];
        filter_fir_s32_init(
            &stage2.filters[mic],
            stage2.filter_state[mic],
            S2_TAP_COUNT,
            filter_config.coef,
            filter_config.shr);
      }
    }

    void ProcessBlock(int32_t sample_out[MIC_COUNT], uint32_t *pdm_block)
    {
      sq66_parallel_decimator_run(
          MIC_COUNT,
          reinterpret_cast<uint32_t *>(stage1.history),
          stage1.filter_coefficients,
          S2_DEC_FACTOR,
          stage2.filters,
          pdm_block,
          sample_out);
    }
};

} // namespace mic_array

#endif
