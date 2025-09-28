#ifndef _ESP_DMX_TRANSCEIVER_H
#define _ESP_DMX_TRANSCEIVER_H
#include "driver/gpio.h"
#include "driver/uart.h"

#include "esp_dmx_receiver.h"

class ESP_Dmx_Transceiver
{
public:
    ESP_Dmx_Transceiver();
    ~ESP_Dmx_Transceiver();
    bool init();
    void receive();
private:

    // ESP_Dmx_Transmitter *_p_esp_dmx_transmitter;
    ESP_Dmx_Receiver *_p_dmx_receiver;
};

#endif //_ESP_DMX_TRANSCEIVER_H