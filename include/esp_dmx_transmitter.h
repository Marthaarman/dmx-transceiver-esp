#ifndef _ESP_DMX_TRANSMITTER_H
#define _ESP_DMX_TRANSMITTER_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

class ESP_Dmx_Transmitter {
    public:
        ESP_Dmx_Transmitter();
        ~ESP_Dmx_Transmitter();
        
        void init();
        void transmit();
        void set_value(uint16_t channel, uint8_t value) { _dmx_buffer[channel] = value; }
    private:
        uint8_t _dmx_buffer[513];
};

#endif