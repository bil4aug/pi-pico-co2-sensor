    add_executable(powerManagement
            main.c
            )

    # pull in common dependencies
    target_link_libraries(powerManagement pico_stdlib hardware_rtc hardware_sleep)

    # create map/bin/hex/uf2 file etc.
    pico_add_extra_outputs(powerManagement)


