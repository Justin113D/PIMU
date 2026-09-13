#include "itf_connector.h"

#include <stdlib.h>

#include "itf_bluetooth.h"
#include "itf_uart.h"
#include "itf_gamepad.h"
#include "itf_input.h"
#include "config.h"
#include "device.h"

static void connected_cb(PimuDeviceConnector *connector)
{
    ppf_device_start();
}

static void get_config_cb(PimuDeviceConnector *connector, PimuDeviceConnectorFirmwareConfig *config)
{
    config->debug_mode = ppf_config_get_debug_mode();
    config->debug_features = ppf_config_get_debug_features();
    config->bluetooth_disabled = ppf_config_get_bluetooth_disabled();
}

static void get_device_colors_cb(PimuDeviceConnector *connector, PimuDeviceConnectorGamepadColors *colors)
{
    *colors = *(PimuDeviceConnectorGamepadColors *)pimu_gamepad_get_device_colors(ppf_gamepad);
}

static void set_device_colors_cb(PimuDeviceConnector *connector, PimuDeviceConnectorGamepadColors *colors)
{
    pimu_gamepad_set_device_colors(ppf_gamepad, (PimuGamepadDeviceColors *)colors);

    ppf_config_set_gamepad_colors(colors);
    ppf_config_queue_save();
}

static void set_inputs_cb(PimuDeviceConnector *connector, PimuDeviceConnectorInputs *inputs)
{
    ppf_itf_input_receive(inputs);
}

void ppf_itf_connector_set_common_callbacks(PimuDeviceConnector *connector)
{
    pimu_device_connector_set_firmware_get_config_cb(connector, get_config_cb);
    pimu_device_connector_set_connected_cb(connector, connected_cb);
    pimu_device_connector_set_gamepad_get_colors_cb(connector, get_device_colors_cb);
    pimu_device_connector_set_gamepad_set_colors_cb(connector, set_device_colors_cb);
    pimu_device_connector_set_gamepad_set_inputs_cb(connector, set_inputs_cb);
}

static PimuDeviceConnector *get_connected_device(void)
{
    if(ppf_device_connector_bluetooth != NULL && pimu_device_connector_get_connection_mode(ppf_device_connector_bluetooth) == PDC_CONNECTION_MODE_ESTABLISHED)
    {
        return ppf_device_connector_bluetooth;
    }

    if(ppf_device_connector_uart != NULL && pimu_device_connector_get_connection_mode(ppf_device_connector_uart) == PDC_CONNECTION_MODE_ESTABLISHED)
    {
        return ppf_device_connector_uart;
    }

    return NULL;
}

void ppf_itf_connector_send_player_leds(uint8_t player_leds_mask, bool flashing)
{
    PimuDeviceConnector *connector = get_connected_device();
    if (connector == NULL)
    {
        return;
    }

    pimu_device_connector_send_player_leds(connector, player_leds_mask, flashing);
}

void ppf_itf_connector_send_debug_string(uint8_t features, char *string)
{
    PimuDeviceConnector *connector = get_connected_device();

    if (connector == NULL || (connector == ppf_device_connector_bluetooth && (features & PDC_FIRMWARE_CONFIG_DEBUG_FEATURE_BLUETOOTH) != 0))
    {
        return;
    }

    pimu_device_connector_send_debug_string(connector, features, string);
}

void ppf_itf_connector_send_debug_data(uint8_t features, uint8_t *data, uint16_t data_size)
{
    PimuDeviceConnector *connector = get_connected_device();
    if (connector == NULL || (connector == ppf_device_connector_bluetooth && (features & PDC_FIRMWARE_CONFIG_DEBUG_FEATURE_BLUETOOTH) != 0))
    {
        return;
    }

    pimu_device_connector_send_debug_data(connector, features, data, data_size);
}

void ppf_itf_connector_read_bytes(PPF_CONNECTOR_TYPE type, uint8_t *data, uint16_t data_size)
{
    PimuDeviceConnector *connector = NULL;
    switch (type)
    {
    case PPF_CONNECTOR_TYPE_BLUETOOTH:
        connector = ppf_device_connector_bluetooth;
        break;
    case PPF_CONNECTOR_TYPE_UART:
        connector = ppf_device_connector_uart;
        break;
    default:
        return; 
    }

    if(connector == NULL)
    {
        return;
    }
    
    PDC_CONNECTION_MODE old_mode =  pimu_device_connector_get_connection_mode(connector);
    if(old_mode == PDC_CONNECTION_MODE_DISABLED)
    {
        return;
    }

    if(!pimu_device_connector_read_bytes(connector, data, data_size))
    {
        return;
    }

    PDC_CONNECTION_MODE new_mode = pimu_device_connector_get_connection_mode(connector);
    if(old_mode == new_mode)
    {
        return;
    }

    bool change_disabled = false;
    bool new_disabled = false;

    if(new_mode == PDC_CONNECTION_MODE_NONE)
    {
        change_disabled = true;
        new_disabled = false;
    }
    else if(old_mode == PDC_CONNECTION_MODE_NONE)
    {
        change_disabled = true;
        new_disabled = true;
    }

    if(change_disabled)
    {
        PimuDeviceConnector *other_connector = NULL;
        switch (type)
        {
        case PPF_CONNECTOR_TYPE_BLUETOOTH:
            other_connector = ppf_device_connector_uart;
            break;
        case PPF_CONNECTOR_TYPE_UART:
            other_connector = ppf_device_connector_bluetooth;
            break;
        }

        if(other_connector != NULL)
        {
            pimu_device_connector_set_disabled(other_connector, new_disabled);
        }
    }
}

bool ppf_itf_connector_check_is_connected(void)
{
    return get_connected_device() != NULL;
}
