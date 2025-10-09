idf_component_register(SRCS "src/esp_dmx_transmitter.cpp"
    INCLUDE_DIRS "include/"
    REQUIRES driver esp_timer soc
)