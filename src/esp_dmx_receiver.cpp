#include "esp_dmx_receiver.h"


const char* TAG = "ESP_DMX_RECEIVER";

ESP_Dmx_Receiver::ESP_Dmx_Receiver(uart_port_t uart_num) : _dmx_uart_num(uart_num){}
ESP_Dmx_Receiver::~ESP_Dmx_Receiver() {
    if (_uart_queue) {
        vQueueDelete(_uart_queue);
    }
}

void _esp_dmx_receiver_task(void* pvParameters) {
    ESP_Dmx_Receiver* p_dmx_receiver = static_cast<ESP_Dmx_Receiver*>(pvParameters);
    uart_event_t event;

    while (1) {
        if (xQueueReceive(p_dmx_receiver->_uart_queue, &event, portMAX_DELAY) != pdTRUE) {
            continue;
        }

        if(!p_dmx_receiver->_flag_receive) continue;

        if (event.type != UART_BREAK) {
            continue;
        }
        
        p_dmx_receiver->_flag_receive = false;
        p_dmx_receiver->_read_dmx_buffer();
    }
}

void ESP_Dmx_Receiver::_read_dmx_buffer() {
    uint8_t dmx_data[513] = {0x00};

    uart_flush(_dmx_uart_num);
    esp_rom_delay_us(88);

    int length = uart_read_bytes(
        _dmx_uart_num,
        dmx_data,
        513,
        50 / portTICK_PERIOD_MS
    );

    if (length > 1 && dmx_data[0] == 0x00) {
        for(int i = 1; i < length; i++) {
            _dmx_buffer[i] = dmx_data[i];
        }
        _flag_new_data = true;
    }
}

void ESP_Dmx_Receiver::init() {
    uart_config_t uart_config = {
        .baud_rate = 250000,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_2,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 122,
        .source_clk = UART_SCLK_DEFAULT,
        .flags = {},
    };

    uart_param_config(_dmx_uart_num, &uart_config);

    uart_set_pin(
        _dmx_uart_num,
        UART_PIN_NO_CHANGE,  // TX unused
        _dmx_rx_pin,
        UART_PIN_NO_CHANGE,  // RTS unused
        UART_PIN_NO_CHANGE   // CTS unused
    );
    uart_driver_install(
        _dmx_uart_num,
        _dmx_buffer_size * 2,               // RX buffer
        0,               // TX buffer unused
        10,                                 // Queue size for events
        _p_dmx_receiver->get_uart_queue(),  // Event queue handle
        0                                   // No special interrupt flags
    );

    _dmx_buffer[0] = 0xFF;
    for(int i = 1; i < 513; i++) {
        _dmx_buffer[i] = 0x00;
    }
   

    xTaskCreate(
        _esp_dmx_receiver_task, 
        "_esp_dmx_receiver_task", 
        4096, 
        this,
        10,
        NULL
    );
}

void ESP_Dmx_Receiver::receive() {
    _flag_receive = true;
    _flag_new_data = false;
}

