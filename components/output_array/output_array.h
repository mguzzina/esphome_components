#pragma once

#include "esphome/core/component.h"
#include "esphome/core/gpio.h"
#include "esphome/components/output/float_output.h"
#include "esphome/components/sigma_delta_output/sigma_delta_output.h"
#include "gpio_wrapper.h"

namespace esphome {
namespace output_array {

class OutputArray : public output::FloatOutput, public Component {
 public:
  void set_outputs(sigma_delta_output::SigmaDeltaOutput *output, const std::vector<GPIOPin *> &pins);

  void setup() override;

  void dump_config() override;

 protected:
  GPIOWrapper pin_wrapper_;
  sigma_delta_output::SigmaDeltaOutput *output_;

  void write_state(float state) override;

  void write_output(size_t pin, float value);
};

}  // namespace output_array
}  // namespace esphome
