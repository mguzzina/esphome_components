#include "esphome/core/log.h"
#include "output_array.h"

namespace esphome {
namespace output_array {

static const char *TAG = "output_array.output";

void OutputArray::set_outputs(sigma_delta_output::SigmaDeltaOutput *output, const std::vector<GPIOPin *> &pins) {
  assert(!pins.empty());

  this->pin_wrapper_.set_pins(pins);
  this->output_ = output;
  this->output_->set_pin(&this->pin_wrapper_);
}

void OutputArray::setup() { this->pin_wrapper_.setup(); }

void OutputArray::dump_config() { ESP_LOGCONFIG(TAG, "Output array custom float output"); }

void OutputArray::write_state(float state) {
  ESP_LOGD(TAG, "state %f", state);

  if (state <= 0) {
    this->write_output(0, 0);
    return;
  }

  if (state >= 1) {
    this->write_output(this->pin_wrapper_.size() - 1, 1);
    return;
  }

  float output;
  state = std::modf(state * this->pin_wrapper_.size(), &output);
  this->write_output(static_cast<size_t>(output), state);
}

void OutputArray::write_output(size_t pin, float value) {
  ESP_LOGD(TAG, "Writing to pin %d: %f", pin, value);
  this->pin_wrapper_.enable_pin(pin);
  this->output_->write_state(value);
}

}  // namespace output_array
}  // namespace esphome
