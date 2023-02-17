
add_subdirectory(Config)
add_subdirectory(ePaper)
add_subdirectory(Fonts)
add_subdirectory(GUI)
include_directories(./Config)
include_directories(./GUI)
include_directories(./ePaper)
    add_executable(v1.0c1
            main.c 
            scd30.h
            scd30.c
            sensirion_arch_config.h
            sensirion_common.h
            sensirion_common.c
            sensirion_i2c.h 
            sensirion_i2c.c
            utils.h
            utils.c
            const.h
            )

    # pull in common dependencies
    target_link_libraries(v1.0c1 ePaper GUI Fonts Config pico_stdlib hardware_i2c hardware_uart hardware_rtc hardware_adc hardware_spi)
    

    # enable usb output, disable uart output
    pico_enable_stdio_usb(v1.0c1 1)
    pico_enable_stdio_uart(v1.0c1 1)

    # create map/bin/hex/uf2 file etc.
    pico_add_extra_outputs(v1.0c1)




