#include "esp_dmx_transmitter.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include <rom/ets_sys.h>
#include <esp_log.h>
#include "soc/gpio_reg.h"

#define TAG "ESP_DMX_TRANSMITTER"

void _esp_dmx_transmitter_task(void *pvParameters) {
    ESP_Dmx_Transmitter* p_dmx_transmitter = static_cast<ESP_Dmx_Transmitter*>(pvParameters);

    while(true) {
        vTaskDelay(pdMS_TO_TICKS(25));
        if(!p_dmx_transmitter->_flag_transmit) continue;
        p_dmx_transmitter->_flag_transmit = false;        

        size_t num_symbols = 1 + 513 * 5;
        rmt_symbol_word_t *items = (rmt_symbol_word_t*)malloc(num_symbols * sizeof(rmt_symbol_word_t));


        items[0].duration0 = 25;        //  break 100us > 88us
        items[0].level0 = 0;            //  break = 0
        items[0].duration1 = 3;         //  MAB 12us > 8us
        items[0].level1 = 1;            //  MAB = 1


        for(int i = 0; i < 513; i++) {
            uint16_t base_word = i * 5 + 1;
            //  each bit is 4us
            //  each message has 1 start bit (low)
            //  each message has 2 stop bits (high)
            //  each message (8 bits) goes from LSB to MSB

            //  first word has start bit and bit 7 - 0
            //  2nd word has bit 7 - 1 and bit 7 - 2
            //  3rd word has bit 7 - 3 and bit 7 - 4
            //  4th word has bit 7 - 5 and bit 7 - 6
            //  5th word has bit 7 - 7 and two stop bits

            uint8_t byte = p_dmx_transmitter->_dmx_buffer[i];
            items[base_word].duration0 = 1;     //  1 start bit -> 4us
            items[base_word].level0 = 0;        //  start bit = 0
            items[base_word].duration1 = 1;     //  LSB = 1 bit -> 4 us
            items[base_word].level1 = (byte & 0x01) ? 1 : 0; 
            for(int j = 1; j < 4; j++) {
                byte >>= 1;
                items[base_word + j].duration0 = 1;
                items[base_word + j].level0 = (byte & 0x01) ? 1 : 0;
                byte >>= 1;
                items[base_word + j].duration1 = 1;
                items[base_word + j].level1 = (byte & 0x01) ? 1 : 0;
            }

            byte >>= 1;
            items[base_word + 4].duration0 = 1;                     //  MSB = 1 bit -> 4 us
            items[base_word + 4].level0 = (byte & 0x01) ? 1 : 0;    
            items[base_word + 4].duration1 = 2;                     //  2 stop bits -> 8 us
            items[base_word + 4].level1 = 1;                        //  stop bit = 1
            
        }

        ESP_ERROR_CHECK(
            rmt_transmit(
                p_dmx_transmitter->_tx_channel,
                p_dmx_transmitter->_copy_encoder,
                items,
                num_symbols * sizeof(rmt_symbol_word_t),    // 4 bytes per symbol
                &p_dmx_transmitter->_tx_config
            )
        );
        rmt_tx_wait_all_done(p_dmx_transmitter->_tx_channel, portMAX_DELAY);
        free(items);

        // vTaskDelay(pdMS_TO_TICKS(25)); //   About 44Hz -> 40Hz
    }
}

void ESP_Dmx_Transmitter::init() {

    rmt_tx_channel_config_t tx_chan_config = {
        .gpio_num = GPIO_NUM_16,
        .clk_src = RMT_CLK_SRC_APB, // Use the default clock source 
        .resolution_hz = 250000,        // 1 tick, 4 us
        .mem_block_symbols = 2046,      // Max possible, not sure how to reduce this yet
        .trans_queue_depth = 4,         // set the number of transactions that can pend in the background
        .intr_priority = 0,
        .flags = {
            .invert_out = false,
            .with_dma = true,  
            .io_loop_back = false,
            .io_od_mode = false,
            .allow_pd = false       
        }
    };

    ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_chan_config, &_tx_channel));

    ESP_ERROR_CHECK(rmt_enable(_tx_channel));

    ESP_ERROR_CHECK(rmt_new_copy_encoder(&_copy_encoder_config, &_copy_encoder));

    for(int i = 0; i < 513; i++) {
        _dmx_buffer[i] = 0; // or fill with channel values
    }

    
    // while(true) {
    //     size_t num_symbols = 1 + 513 * 5;
    //     rmt_symbol_word_t *items = (rmt_symbol_word_t*)malloc(num_symbols * sizeof(rmt_symbol_word_t));


    //     items[0].duration0 = 25;        //  break 100us > 88us
    //     items[0].level0 = 0;            //  break = 0
    //     items[0].duration1 = 3;         //  MAB 12us > 8us
    //     items[0].level1 = 1;            //  MAB = 1


    //     for(int i = 0; i < 513; i++) {
    //         uint16_t base_word = i * 5 + 1;
    //         //  each bit is 4us
    //         //  each message has 1 start bit (low)
    //         //  each message has 2 stop bits (high)
    //         //  each message (8 bits) goes from LSB to MSB

    //         //  first word has start bit and bit 7 - 0
    //         //  2nd word has bit 7 - 1 and bit 7 - 2
    //         //  3rd word has bit 7 - 3 and bit 7 - 4
    //         //  4th word has bit 7 - 5 and bit 7 - 6
    //         //  5th word has bit 7 - 7 and two stop bits

    //         uint8_t byte = _dmx_buffer[i];
    //         items[base_word].duration0 = 1;     //  1 start bit -> 4us
    //         items[base_word].level0 = 0;        //  start bit = 0
    //         items[base_word].duration1 = 1;     //  LSB = 1 bit -> 4 us
    //         items[base_word].level1 = (byte & 0x01) ? 1 : 0; 
    //         for(int j = 1; j < 4; j++) {
    //             byte >>= 1;
    //             items[base_word + j].duration0 = 1;
    //             items[base_word + j].level0 = (byte & 0x01) ? 1 : 0;
    //             byte >>= 1;
    //             items[base_word + j].duration1 = 1;
    //             items[base_word + j].level1 = (byte & 0x01) ? 1 : 0;
    //         }

    //         byte >>= 1;
    //         items[base_word + 4].duration0 = 1;                     //  MSB = 1 bit -> 4 us
    //         items[base_word + 4].level0 = (byte & 0x01) ? 1 : 0;    
    //         items[base_word + 4].duration1 = 2;                     //  2 stop bits -> 8 us
    //         items[base_word + 4].level1 = 1;                        //  stop bit = 1
            
    //     }

    //     ESP_ERROR_CHECK(
    //         rmt_transmit(
    //             _tx_channel,
    //             _copy_encoder,
    //             items,
    //             num_symbols * sizeof(rmt_symbol_word_t),    // 4 bytes per symbol
    //             &_tx_config
    //         )
    //     );
    //     rmt_tx_wait_all_done(_tx_channel, portMAX_DELAY);
    //     free(items);
    //     vTaskDelay(pdMS_TO_TICKS(1000));
    // }

    xTaskCreate(
        _esp_dmx_transmitter_task, 
        "_esp_dmx_transmitter_task", 
        4096, 
        this,
        10,
        NULL
    );
    
}

void ESP_Dmx_Transmitter::transmit() {
    while(_flag_transmit == true) {
        vTaskDelay(pdMS_TO_TICKS(25));
    }
    _flag_transmit = true;
}