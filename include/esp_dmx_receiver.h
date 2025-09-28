#ifndef _ESP_DMX_RECEIVER_H
#define _ESP_DMX_RECEIVER_H

#pragma once
#include "driver/uart.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"

class ESP_Dmx_Receiver {
public:
    ESP_Dmx_Receiver();
    ~ESP_Dmx_Receiver();
    void    init();
    void    receive();
    bool    has_new_data() { return _flag_new_data; };
    uint8_t get_value(uint16_t channel) { return _dmx_buffer[channel]; };

    private:
    
    friend void _esp_dmx_receiver_task(void* pvParameters);

    const uart_port_t   _dmx_uart_num = UART_NUM_1;
    const gpio_num_t    _dmx_rx_pin = GPIO_NUM_18;
    const int           _dmx_buffer_size = 513;

    uint8_t         _dmx_buffer[513];
    bool            _flag_receive = false;
    bool            _flag_new_data = false;
    QueueHandle_t   _uart_queue = NULL;

    void _read_dmx_buffer();
};


#endif

