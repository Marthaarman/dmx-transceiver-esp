#ifndef _ESP_DMX_TRANSMITTER_H
#define _ESP_DMX_TRANSMITTER_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include <stdint.h>
#include "driver/rmt_tx.h"

class ESP_Dmx_Transmitter {
    public:
        ESP_Dmx_Transmitter() {};
        ~ESP_Dmx_Transmitter() {};
        
        void init();
        void transmit();
        void set_value(uint16_t channel, uint8_t value) { _dmx_buffer[channel] = value; }
    private:

        friend void _esp_dmx_transmitter_task(void*);
        uint8_t _dmx_buffer[513];
        bool _flag_transmit = false;

        rmt_copy_encoder_config_t _copy_encoder_config = {};
        rmt_encoder_handle_t _copy_encoder = NULL;

        rmt_transmit_config_t _tx_config = {
            .loop_count = 0,
            .flags = {
                .eot_level = 1,             //  at the end of transmission set value to 1
                .queue_nonblocking = 1,
            },
        };

        rmt_symbol_word_t _dmx_break_mab_symbol;
        rmt_channel_handle_t _tx_channel = NULL;
        
};

#endif