#ifndef _ESP_DMX_TRANSCEIVER_H
#define _ESP_DMX_TRANSCEIVER_H
#include "driver/gpio.h"
#include "driver/uart.h"

#include "esp_dmx_receiver.h"
#include "esp_dmx_transmitter.h"

class ESP_Dmx_Transceiver
{
public:
    ESP_Dmx_Transceiver(gpio_num_t rx_pin, gpio_num_t tx_pin);
    ~ESP_Dmx_Transceiver();
    bool init();
    void run();
    void stop();
    void set_channel_value(uint16_t channel, uint8_t value);
    uint8_t get_channel_value(uint16_t channel);


private:
    friend void _esp_dmx_transceiver_task(void* pvParameters);
    void _transmit();
    void _receive();
    ESP_Dmx_Transmitter *_p_dmx_transmitter;
    ESP_Dmx_Receiver *_p_dmx_receiver;

    const uart_port_t   _dmx_uart_num_rx = UART_NUM_1;
    const uart_port_t   _dmx_uart_num_tx = UART_NUM_0;
    const gpio_num_t    _dmx_rx_pin;
    const gpio_num_t    _dmx_tx_pin;

    TaskHandle_t _task_handle = NULL;

};

#endif //_ESP_DMX_TRANSCEIVER_H