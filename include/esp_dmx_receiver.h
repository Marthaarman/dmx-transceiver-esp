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
    ESP_Dmx_Receiver(uart_port_t uart_num);
    ~ESP_Dmx_Receiver();
    void    init();
    void    receive();
    bool    has_new_data() { return _flag_new_data; };
    uint8_t get_value(uint16_t channel) { return _dmx_buffer[channel]; };

    QueueHandle_t* get_uart_queue() {return &_uart_queue;}

    private:
    const uart_port_t _dmx_uart_num;

    friend void _esp_dmx_receiver_task(void* pvParameters);

    uint8_t         _dmx_buffer[513];
    bool            _flag_receive = false;
    bool            _flag_new_data = false;
    QueueHandle_t   _uart_queue = NULL;

    void _read_dmx_buffer();
};


#endif

