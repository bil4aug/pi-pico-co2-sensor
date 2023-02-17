cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)
include(pico_extras_import.cmake)

project(pico-scd30-i2c)

pico_sdk_init()

add_subdirectory(V1.00.00b)
add_subdirectory(V1.00.00c)
add_subdirectory(powerManagement)

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
        )