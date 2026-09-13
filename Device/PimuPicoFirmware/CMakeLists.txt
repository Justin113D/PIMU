# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.3.0)
set(toolchainVersion 15_2_Rel1)
set(picotoolVersion 2.3.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================

if (NOT DEFINED PICO_BOARD)
    set(PICO_BOARD pico2_w CACHE STRING "Board type")
endif()

include(pico_sdk_import.cmake)

project(pimu C CXX ASM)

pico_sdk_init()

add_executable(pimu
    src/configs/descriptors.c

    src/wireless/bluetooth.c
    src/wireless/rfcomm.c
    src/wireless/hci.c
    src/wireless/hci_debug.c

    src/interface/itf_bluetooth.c
    src/interface/itf_uart.c
    src/interface/itf_connector.c
    src/interface/itf_input.c
    src/interface/itf_imu.c
    src/interface/itf_gamepad.c
    
    src/debug.c
    src/config.c
    src/pico_utils.c
    src/device.c
    src/main.c
)

target_include_directories(pimu PRIVATE 
    src
    src/configs
)

add_subdirectory(
    ${CMAKE_CURRENT_LIST_DIR}/../PimuDeviceConnector 
    pimu_device_connector
)

add_subdirectory(
    ${CMAKE_CURRENT_LIST_DIR}/../PimuGamepad 
    pimu_gamepad
)

target_link_libraries(pimu 
    pico_stdlib
    pico_multicore
    pico_flash

    hardware_flash

    $<$<BOOL:${PICO_CYW43_SUPPORTED}>:pico_cyw43_arch_none>
    $<$<BOOL:${PICO_CYW43_SUPPORTED}>:pico_btstack_cyw43>
    $<$<BOOL:${PICO_CYW43_SUPPORTED}>:pico_btstack_classic>

    tinyusb_device
    tinyusb_board
    
    pimu_device_connector
    pimu_gamepad
)

pico_add_extra_outputs(pimu)

if (PICO_CYW43_SUPPORTED)
add_compile_definitions(BLUETOOTH_ENABLED)
endif()