#pragma GCC optimize("O0")

#include "config.h"

#include <stdint.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/flash.h"
#include "hardware/flash.h"

#include "device.h"
#include <pimu_gamepad.h>

static const uint32_t pimu_header = 0x554D4950; // "PIMU"
static const uint32_t pimu_current_version = 1;

static absolute_time_t save_timeframe_start;
static absolute_time_t save_timeframe_end;

typedef struct
{
    uint32_t header;
    uint32_t version;

    uint8_t debug_mode;
    uint8_t debug_features;
    bool bluetooth_disabled;

    PimuDeviceConnectorGamepadColors gamepad_colors;
} PPFConfig;

// storing at the end
_Static_assert(sizeof(PPFConfig) <= FLASH_PAGE_SIZE, "setting does not fit in one flash page");
const uint32_t ppf_config_flash_offset = PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE;

static PPFConfig active_config;

void ppf_config_load(void)
{
    const PPFConfig *stored = (const PPFConfig*)(XIP_BASE + ppf_config_flash_offset);

    if(stored->header != pimu_header 
        || stored->version != pimu_current_version)
    {
        active_config.header = pimu_header;
        active_config.version = pimu_current_version;
        active_config.gamepad_colors = *(PimuDeviceConnectorGamepadColors*)&default_device_colors;
    }
    else
    {
        active_config = *stored;
    }
}

void ppf_config_queue_save(void)
{
    absolute_time_t absolute_time = get_absolute_time();
    save_timeframe_start = delayed_by_ms(absolute_time, 5);
    save_timeframe_end = delayed_by_ms(absolute_time, 200);
}

static void __not_in_flash_func(write_flash_callback)(void *param)
{
    const uint8_t *write_data = (const uint8_t *)param;
    flash_range_erase(ppf_config_flash_offset, FLASH_SECTOR_SIZE);
    flash_range_program(ppf_config_flash_offset, write_data, FLASH_PAGE_SIZE);
}

void ppf_config_poll_save(void)
{
    if (save_timeframe_start == 0)
    {
        return;
    }

    absolute_time_t absolute_time = get_absolute_time();

    if (absolute_time_diff_us(absolute_time, save_timeframe_start) > 0
        || (ppf_device_check_is_writing() && absolute_time_diff_us(absolute_time, save_timeframe_end) > 0)) 
    {
        return;
    }

    save_timeframe_start = 0;
    save_timeframe_end = 0;

    uint8_t buffer[FLASH_PAGE_SIZE];
    memset(buffer, 0xFF, sizeof(buffer));
    memcpy(buffer, &active_config, sizeof(PPFConfig));

    int result = flash_safe_execute(write_flash_callback, buffer, 1000);
    if(result != PICO_OK)
    {
        result = 0;
    }
}


void ppf_config_set_debug_mode(uint8_t debug_mode)
{
    active_config.debug_mode = debug_mode;
}

uint8_t ppf_config_get_debug_mode(void)
{
    return active_config.debug_mode;
}

void ppf_config_set_debug_features(uint8_t debug_features)
{
    active_config.debug_features = debug_features;
}

uint8_t ppf_config_get_debug_features(void)
{
    return active_config.debug_features;
}

void ppf_config_set_bluetooth_disabled(bool bluetooth_disabled)
{
    active_config.bluetooth_disabled = bluetooth_disabled;
}

bool ppf_config_get_bluetooth_disabled(void)
{
    return active_config.bluetooth_disabled;
}

void ppf_config_set_gamepad_colors(PimuDeviceConnectorGamepadColors *colors)
{
    active_config.gamepad_colors = *colors;
}

PimuDeviceConnectorGamepadColors* ppf_config_get_gamepad_colors(void)
{
    return &active_config.gamepad_colors;
}
