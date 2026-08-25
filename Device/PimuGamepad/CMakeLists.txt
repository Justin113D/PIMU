cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(lib_name pimu_gamepad)
project(${lib_name} C CXX ASM)

add_library(${lib_name} 
    src/commands/cmd_battery.c
    src/commands/cmd_bluetooth_pairing.c
    src/commands/cmd_charging_grip.c
    src/commands/cmd_feature_select.c
    src/commands/cmd_firmware_info.c
    src/commands/cmd_firmware_update.c
    src/commands/cmd_flash_memory.c
    src/commands/cmd_initialization.c
    src/commands/cmd_nfc.c
    src/commands/cmd_player_leds.c
    src/commands/cmd_vibration.c
    src/commands/cmd_unknown_4.c
    src/commands/cmd_unknown_5.c
    src/commands/cmd_unknown_6.c
    src/commands/cmd_unknown_7.c
    src/commands/cmd_unknown_14.c
    src/commands/cmd_unknown_15.c
    src/commands/cmd_unknown_17.c
    src/commands/cmd_unknown_18.c
    src/commands/cmd_unknown_19.c
    src/commands/cmd_unknown_20.c
    src/commands/cmd_unknown_22.c
    src/commands/cmd_unknown_23.c
    src/commands/cmd_unknown_24.c

    src/status.c
    src/connection.c
    src/firmware.c
    src/flash.c
    src/gamepad.c
    src/input.c
)

target_include_directories(${lib_name} PRIVATE
    src
)

target_include_directories(${lib_name} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
