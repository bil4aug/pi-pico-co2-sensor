add_subdirectory(pico-ssd1306)

    add_executable(v1.0a1
            main.cpp 
            scd30.h
            scd30.c
            sensirion_arch_config.h
            sensirion_common.h
            sensirion_common.c
            sensirion_i2c.h 
            sensirion_i2c.c
            utils.h
            utils.c
            )

    # pull in common dependencies
    target_link_libraries(v1.0a1 pico_stdlib hardware_i2c hardware_uart pico_ssd1306 hardware_rtc pico_multicore hardware_adc)

    # enable usb output, disable uart output
    pico_enable_stdio_usb(v1.0a1 1)
    pico_enable_stdio_uart(v1.0a1 0)

    # create map/bin/hex/uf2 file etc.
    pico_add_extra_outputs(v1.0a1)


