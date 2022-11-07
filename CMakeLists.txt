cmake_minimum_required(VERSION 3.13)

#include build functions from SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include($ENV{PICO_SDK_PATH}/tools/CMakeLists.txt)

#
project(e131 C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

#
set(PICO_BOARD pico_w)

pico_sdk_init()

add_executable(e131
	e131.c
	ws2812.c
	receiver.c
	artnet.c
)

#
pico_generate_pio_header(${PROJECT_NAME}
    ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio
)

pico_enable_stdio_usb(e131 1)
pico_enable_stdio_uart(e131 1)

pico_add_extra_outputs(e131)

target_compile_definitions(e131 PRIVATE
	WIFI_SSID=\"${WIFI_SSID}\"
	WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
	)

target_include_directories(e131 PRIVATE 
	${CMAKE_CURRENT_LIST_DIR}
	)

target_link_libraries(e131
       	pico_stdlib 
	pico_util 
	hardware_spi
	hardware_clocks
	# pico_cyw43_arch_lwip_poll
	pico_cyw43_arch_lwip_threadsafe_background
)


