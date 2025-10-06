#ifndef _ESP_DMX_TRANSCEIVER_H
#define _ESP_DMX_TRANSCEIVER_H
#include "driver/gpio.h"
#include "driver/uart.h"

#include "esp_dmx_receiver.h"
#include "esp_dmx_transmitter.h"

static portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;

class ESP_Dmx_Transceiver
{
public:
    ESP_Dmx_Transceiver();
    ~ESP_Dmx_Transceiver();
    bool init();
    void receive();
    void transmit();
private:

    ESP_Dmx_Transmitter *_p_dmx_transmitter;
    ESP_Dmx_Receiver *_p_dmx_receiver;

    const uart_port_t   _dmx_uart_num = UART_NUM_1;
    const gpio_num_t    _dmx_rx_pin = GPIO_NUM_18;
    const gpio_num_t    _dmx_tx_pin = GPIO_NUM_16;
    const int           _dmx_buffer_size = 513;

    // const portMUX_TYPE _spinlock = portMUX_INITIALIZER_UNLOCKED;
};

#endif //_ESP_DMX_TRANSCEIVER_H