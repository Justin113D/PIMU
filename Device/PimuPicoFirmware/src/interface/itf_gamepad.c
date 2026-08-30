#include "itf_gamepad.h"

#include <stdlib.h>

#include "itf_connector.h"
#include "debug.h"

PimuGamepad* ppf_gamepad;

static void gamepad_debug_callback(PimuGamepad *gamepad, PG_DEBUG_TYPE type, const char *format, va_list args)
{
    const char *type_label = NULL;
    switch (type)
    {
    case PG_DEBUG_TYPE_INFO:
        type_label = " INFO  ] [GAMEPAD";
        break;
    case PG_DEBUG_TYPE_WARNING:
        type_label = "WARNING] [GAMEPAD";
        break;
    case PG_DEBUG_TYPE_ERROR:
        type_label = " ERROR ] [GAMEPAD";
        break;
    }

    ppf_debug_printf_list(PDC_FIRMWARE_CONFIG_DEBUG_FEATURE_GAMEPAD, type_label, format, args);
}

static void gamepad_player_leds_changed_cb(PimuGamepad *source, uint8_t mask, bool flashing)
{
    ppf_itf_connector_send_player_leds(mask, flashing);
}

void ppf_itf_gamepad_init(void)
{
    ppf_gamepad = pimu_gamepad_alloc();    
    pimu_gamepad_set_name(ppf_gamepad, "Gamepad");
    pimu_gamepad_set_device_colors(ppf_gamepad, (PimuGamepadDeviceColors*)ppf_config_get_gamepad_colors());

    pimu_gamepad_set_debug_cb(ppf_gamepad, gamepad_debug_callback);
    pimu_gamepad_set_player_leds_changed_cb(ppf_gamepad, gamepad_player_leds_changed_cb);
}