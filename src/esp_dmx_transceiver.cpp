#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "esp_log.h"


#include "esp_dmx_transceiver.h"


ESP_Dmx_Transceiver::ESP_Dmx_Transceiver()
{
    ESP_LOGI("ESP_Dmx_Transceiver", "ESP_Dmx_Transceiver");
}

ESP_Dmx_Transceiver::~ESP_Dmx_Transceiver()
{
    ESP_LOGI("ESP_Dmx_Transceiver", "~ESP_Dmx_Transceiver");

}

bool ESP_Dmx_Transceiver::init() {
    _p_dmx_receiver = new ESP_Dmx_Receiver();
    // _p_dmx_transmitter = new ESP_Dmx_Transmitter();

    _p_dmx_receiver->init();
    return true;
}

void ESP_Dmx_Transceiver::receive() {
    ESP_LOGI("ESP_Dmx_Transceiver", "receive");
    _p_dmx_receiver->receive();
    
}

// bool ESP_Dmx_Transceiver::_init_uart() {

// }
