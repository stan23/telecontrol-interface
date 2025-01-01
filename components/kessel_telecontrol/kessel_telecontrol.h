#pragma once

#include "esphome/core/component.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace kessel_telecontrol {

class KesselTelecontrol : public text_sensor::TextSensor, public Component, public uart::UARTDevice {
  public:
    void setup() override;
    void loop() override;
    void dump_config() override;

  protected:
    bool readLine(void);
    void handleLine(void);

    static constexpr int MAX_LINE_LEN = 200;
    std::array<char, MAX_LINE_LEN> line_buffer_;
    size_t line_pos_ = 0;
    char term_char_ = '\r'; 
    bool armed_for_message_ = false;
};


}  // namespace kessel_telecontrol
}  // namespace esphome