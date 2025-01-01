/**
 * interface for wastewater lifting control called "Kessel Aqualift Comfort"
 * by pretending to be a GSM modem called "Kessel TeleControl" 
 * 
 * inspired by:
 * https://github.com/jesserockz/esphome-external-component-examples
 * https://github.com/tfyoung/esphome-junctek_kgf
 * 
 */


#include "esphome.h"
#include "esphome/core/log.h"

#include "kessel_telecontrol.h"

#include <string.h>


namespace esphome {
namespace kessel_telecontrol {

static const char *TAG = "Kessel Telecontrol";

void KesselTelecontrol::setup() {
    ESP_LOGCONFIG(TAG, "Setup Kessel Telecontrol - nothing to do");
}

// from https://esphome.io/cookbook/lambda_magic.html#custom-uart-text-sensor

bool KesselTelecontrol::readLine (void) {
    while (available()) {
        const char readch = read();
        if (readch > 0) {
            if (readch == this->term_char_) {
                if (this->term_char_ == '\r') {
                    // add the line break but skip the "end of SMS"
                    this->line_buffer_[this->line_pos_++] = readch;
                }
                this->line_buffer_[this->line_pos_] = 0;
                this->line_pos_ = 0;  // start next line at the beginnning
                return true;
            } else {
                if (this->line_pos_ < MAX_LINE_LEN - 1) {
                    this->line_buffer_[this->line_pos_++] = readch;
                    this->line_buffer_[this->line_pos_] = 0;
                }
            }
        }
    }
    return false;
}

void KesselTelecontrol::handleLine (void) {
    const char* buffer = &this->line_buffer_[0];

    // TODO: optimize code

    if (strncmp(buffer, "AT\r", 3) == 0) {
        write_str("OK\r\n");
        ESP_LOGD(TAG, "received '%s', responded 'OK'", &buffer[0]);

    } else if (strncmp(buffer, "ATE0V0X0", 8) == 0) {
        write_str("0\r");
        ESP_LOGD(TAG, "received '%s', responded '0'", &buffer[0]);

    } else if (strncmp(buffer, "ATE0V0X0S0=0&K=0", 16) == 0) {
        write_str("0\r");
        ESP_LOGD(TAG, "received '%s', responded '0'", &buffer[0]);

    } else if (strncmp(buffer, "AT+IPR=9600", 11) == 0) {
        write_str("0\r");
        ESP_LOGD(TAG, "received '%s', responded '0'", &buffer[0]);

    } else if (strncmp(buffer, "AT+CPIN?", 8) == 0) {
        write_str("+CPIN: READY\r");
        ESP_LOGD(TAG, "received '%s', responded '+CPIN: READY'", &buffer[0]);

    } else if (strncmp(buffer, "AT+CREG?", 8) == 0) {
        write_str("+CREG: 1,1\r");
        ESP_LOGD(TAG, "received '%s', responded '+CREG: 1,1'", &buffer[0]);

    } else if (strncmp(buffer, "ATS0=0+CMGF=1", 13) == 0) {
        write_str("0\r");
        ESP_LOGD(TAG, "received '%s', responded '0'", &buffer[0]);

    } else if (strncmp(buffer, "AT+COPS=3,0", 11) == 0) {
        write_str("0\r");
        ESP_LOGD(TAG, "received '%s', responded '0'", &buffer[0]);

    } else if (strncmp(buffer, "AT+COPS?", 8) == 0) {
        write_str("+COPS: 2,0,0\r");
        ESP_LOGD(TAG, "received '%s', responded '+COPS: 2,0,0'", &buffer[0]);

    } else if (strncmp(buffer, "AT+CSQ", 6) == 0) {
        write_str("+CSQ: 15,2\r");
        ESP_LOGD(TAG, "received '%s', responded '+CSQ: 15,2'", &buffer[0]);

    } else if (strncmp(buffer, "AT+CPMS?", 8) == 0) {
        write_str("+CPMS: 2,10,2,10,2,10\r");
        ESP_LOGD(TAG, "received '%s', responded '+CPMS: 2,10,2,10,2,10'", &buffer[0]);

    } else if (strncmp(buffer, "AT+CMGF=1", 9) == 0) {
        write_str("0\r");
        ESP_LOGD(TAG, "received '%s', responded '0'", &buffer[0]);

    } else if (strncmp(buffer, "AT+CMEE=1", 9) == 0) {
        write_str("0\r");
        ESP_LOGD(TAG, "received '%s', responded '0'", &buffer[0]);

    } else if (strncmp(buffer, "AT+CMEE=0", 9) == 0) {
        write_str("0\r");
        ESP_LOGD(TAG, "received '%s', responded '0'", &buffer[0]);

    } else if (strncmp(buffer, "at+csca=", 8) == 0) {
        write_str("0\r");
        ESP_LOGD(TAG, "received '%s', responded '0'", &buffer[0]);

    } else if (strncmp(buffer, "at+cmgs=", 8) == 0) {
        write_str("\r\n ");
        this->term_char_ = 0x1A;
        this-> armed_for_message_ = true;
        ESP_LOGD(TAG, "received '%s', responded '\\r\\n ', waiting for message", &buffer[0]);

    } else if (this->armed_for_message_) {
        write_str("+CMGS: 42\r");
        this-> term_char_ = '\r';
        this-> armed_for_message_ = false;
        ESP_LOGD(TAG, "message '%s'", &buffer[0]);
        // publish the received message text
        this->publish_state(&buffer[0]);

    } else {
        write_str("0\r");
        ESP_LOGD(TAG, "UNKNOWN COMMAND '%s'responded '0'", &buffer[0]);

    }
}


void KesselTelecontrol::loop() {

    if (readLine()) {
        handleLine();
    }

}

void KesselTelecontrol::dump_config(){
    ESP_LOGCONFIG(TAG, "no configuration");
}

}  // namespace kessel_telecontrol
}  // namespace esphome