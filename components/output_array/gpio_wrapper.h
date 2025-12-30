#pragma once

#include "esphome/core/gpio.h"

namespace esphome {
namespace output_array {

class GPIOWrapper : public GPIOPin {
 public:
  void setup() override {
    assert(!this->pins_.empty());
    for (auto &pin : this->pins_) {
      pin->setup();
      pin->digital_write(false);
    }
  }

  void pin_mode(gpio::Flags flags) override {
    assert(!this->pins_.empty());
    for (auto &pin : this->pins_) {
      pin->pin_mode(flags);
    }
  }

  gpio::Flags get_flags() const override {
    assert(!this->pins_.empty());
    return this->pins_.front()->get_flags();
  }

  bool digital_read() override {
    assert(!this->pins_.empty());
    std::lock_guard lock(this->pin_mutex_);
    return this->pins_[this->active_pin_]->digital_read();
  }

  void digital_write(bool value) override {
    assert(!this->pins_.empty());
    std::lock_guard lock(this->pin_mutex_);

    if (this->active_pin_ == 0) {
      this->pins_.front()->digital_write(value);

      for (size_t i = 1; i < this->pins_.size(); ++i) {
        this->pins_[i]->digital_write(false);
      }
      return;
    }

    size_t activate_pin = this->active_pin_;
    if (!value) {
      --activate_pin;
    }

    for (size_t i = 0; i < activate_pin; ++i) {
      this->pins_[i]->digital_write(false);
    }

    this->pins_[activate_pin]->digital_write(true);

    for (size_t i = activate_pin + 1; i < this->pins_.size(); ++i) {
      this->pins_[i]->digital_write(false);
    }
  }

  std::string dump_summary() const override {
    assert(!this->pins_.empty());
    std::string summary = "[" + this->pins_.front()->dump_summary();
    for (size_t i = 1; i < this->pins_.size(); ++i) {
      summary += ", " + this->pins_[i]->dump_summary();
    }
    summary += ']';
    return summary;
  }

  void set_pins(const std::vector<GPIOPin *> &pins) {
    this->active_pin_ = 0;
    this->pins_ = pins;
  }

  void enable_pin(size_t pin) {
    assert(!this->pins_.empty());
    std::lock_guard lock(this->pin_mutex_);
    this->active_pin_ = std::clamp(pin, 0U, this->pins_.size() - 1);
  }

  [[nodiscard]] size_t size() const { return this->pins_.size(); }

 protected:
  std::mutex pin_mutex_;
  size_t active_pin_;
  std::vector<GPIOPin *> pins_;
};

}  // namespace output_array
}  // namespace esphome
