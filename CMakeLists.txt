cmake_minimum_required(VERSION 3.13)

#include build functions from SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include($ENV{PICO_SDK_PATH}/tools/CMakeLists.txt)

#
project(picopixelz C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

#
set(PICO_BOARD pico_w)

pico_sdk_init()

add_subdirectory(lib/FatFs_SPI build)
add_subdirectory(lib/ly)

add_executable(picopixelz
	ppixelz.c
	e131.c
	ws2812.c
	artnet.c
	web.c
	hw_config.c
	config.c
)

#
pico_generate_pio_header(${PROJECT_NAME}
    ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio
)

pico_enable_stdio_usb(picopixelz 1)
pico_enable_stdio_uart(picopixelz 1)

pico_add_extra_outputs(picopixelz)

target_include_directories(picopixelz PRIVATE 
	${CMAKE_CURRENT_LIST_DIR}
	)

target_link_libraries(picopixelz
  	pico_stdlib 
	pico_util
	FatFs_SPI
	ly
	hardware_spi
	hardware_clocks
	# pico_cyw43_arch_lwip_poll
	pico_cyw43_arch_lwip_threadsafe_background
	pico_lwip_http
)


