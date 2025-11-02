#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "esp_log.h"


#include "esp_dmx_transceiver.h"


void _esp_dmx_transceiver_task(void* pvParameters) {
    ESP_Dmx_Transceiver* p_dmx_transceiver = static_cast<ESP_Dmx_Transceiver*>(pvParameters);
    uart_event_t event;

    while (1) {
        p_dmx_transceiver->_receive();
        p_dmx_transceiver->_transmit();
        vTaskDelay(25 / portTICK_PERIOD_MS);
    }
}

ESP_Dmx_Transceiver::ESP_Dmx_Transceiver(gpio_num_t rx_pin, gpio_num_t tx_pin) :
    _dmx_rx_pin(rx_pin),
    _dmx_tx_pin(tx_pin) 
{
    ESP_LOGI("ESP_Dmx_Transceiver", "ESP_Dmx_Transceiver");
}

ESP_Dmx_Transceiver::~ESP_Dmx_Transceiver()
{
    ESP_LOGI("ESP_Dmx_Transceiver", "~ESP_Dmx_Transceiver");

}

bool ESP_Dmx_Transceiver::init() {
    _p_dmx_receiver = new ESP_Dmx_Receiver(_dmx_rx_pin);
    _p_dmx_transmitter = new ESP_Dmx_Transmitter(_dmx_tx_pin);

    _p_dmx_receiver->init();
    _p_dmx_transmitter->init();
    return true;
}

void ESP_Dmx_Transceiver::run() {
    xTaskCreate(
        _esp_dmx_transceiver_task, 
        "_esp_dmx_transceiver_task", 
        4096, 
        this,
        10,
        &_task_handle
    );
    
}

void ESP_Dmx_Transceiver::stop() {
    if(_task_handle == NULL) return;
    vTaskDelete(_task_handle);
}

void ESP_Dmx_Transceiver::set_channel_value(uint16_t channel, uint8_t value) {
    _p_dmx_transmitter->set_channel_value(channel, value);
}

uint8_t ESP_Dmx_Transceiver::get_channel_value(uint16_t channel) {
    return _p_dmx_receiver->get_channel_value(channel);
}

void ESP_Dmx_Transceiver::_transmit() {
    _p_dmx_transmitter->transmit();
}

void ESP_Dmx_Transceiver::_receive() {
    _p_dmx_receiver->receive();
}

