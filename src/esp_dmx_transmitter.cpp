#include "esp_dmx_transmitter.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "soc/periph_defs.h"

#define DMX_UART_NUM      UART_NUM_1
#define DMX_TX_PIN        17   // Set to your GPIO
#define DMX_PACKET_SIZE   513  // 1 start code + 512 channels


void dmx_send_packet(uint8_t *data) {
    // --- Send BREAK (low for at least 88 µs) ---
    uart_set_line_inverse(DMX_UART_NUM, UART_SIGNAL_TXD_INV); // invert TX so idle is low
    esp_rom_delay_us(88); // ~88 µs break
    uart_set_line_inverse(DMX_UART_NUM, UART_SIGNAL_INV_DISABLE); // restore

    // --- Send MAB (high for at least 8 µs) ---
    esp_rom_delay_us(8); // ~8 µs mark after break

    // --- Send DMX data (start code + 512 slots) ---
    uart_write_bytes(DMX_UART_NUM, (const char*)data, DMX_PACKET_SIZE);
    uart_wait_tx_done(DMX_UART_NUM, pdMS_TO_TICKS(20));
}

void ESP_Dmx_Transmitter::init() {
    uart_config_t uart_config = {
        .baud_rate = 250000,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_2,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };

    ESP_ERROR_CHECK(uart_param_config(DMX_UART_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(DMX_UART_NUM, DMX_TX_PIN, UART_PIN_NO_CHANGE,
                                 UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(DMX_UART_NUM, 1024, 0, 0, NULL, 0));

    
}

void ESP_Dmx_Transmitter::transmit() {
    // Build DMX buffer
    uint8_t dmx_data[DMX_PACKET_SIZE];
    dmx_data[0] = 0x00; // start code
    for (int i = 1; i < DMX_PACKET_SIZE; i++) {
        dmx_data[i] = 0x00; // or fill with channel values
    }

    while (1) {
        dmx_send_packet(dmx_data);
        vTaskDelay(pdMS_TO_TICKS(25)); // DMX refresh ~44 Hz (minimum spacing ~23 ms)
    }
}