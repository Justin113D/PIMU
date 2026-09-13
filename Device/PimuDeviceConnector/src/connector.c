#include "connector.h"

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

PimuDeviceConnector *pimu_device_connector_alloc(void)
{
    return calloc(1, sizeof(PimuDeviceConnector));
}

void pimu_device_connector_free(PimuDeviceConnector *connector)
{
    free(connector);
}

void pimu_device_connector_set_name(PimuDeviceConnector *connector, const char *name)
{
    connector->name = name;
}

const char *pimu_device_connector_get_name(PimuDeviceConnector *connector)
{
    return connector->name;
}

void pdc_debug_print(PimuDeviceConnector *connector, PDC_DEBUG_TYPE type, char *format, ...)
{
    if (connector->debug_cb != NULL)
    {
        va_list args;
        va_start(args, format);
        connector->debug_cb(connector, type, format, args);
        va_end(args);
    }
}

//--------------------------------------------------------------------+

void pimu_device_connector_set_connected_cb(PimuDeviceConnector *connector, PDCConnected callback)
{
    connector->connected_cb = callback;
}

void pimu_device_connector_set_disconnected_cb(PimuDeviceConnector *connector, PDCDisconnected callback)
{
    connector->disconnected_cb = callback;
}

PDC_CONNECTION_MODE pimu_device_connector_get_connection_mode(PimuDeviceConnector *connector)
{
    return connector->connection_mode;
}

void pimu_device_connector_disconnect(PimuDeviceConnector *connector)
{
    if (connector->connection_mode == PDC_CONNECTION_MODE_NONE || connector->connection_mode == PDC_CONNECTION_MODE_DISABLED)
    {
        return;
    }

    connector->connection_mode = PDC_CONNECTION_MODE_NONE;

    pdc_send_buffer_reset(&connector->send_buffer);
    pdc_read_buffer_reset(&connector->read_buffer);

    if (connector->disconnected_cb != NULL)
    {
        connector->disconnected_cb(connector);
    }

    pdc_debug_print(connector, PDC_DEBUG_TYPE_INFO, "Disconnected");
}

void pimu_device_connector_set_disabled(PimuDeviceConnector *connector, bool disabled)
{
    if((connector->connection_mode == PDC_CONNECTION_MODE_DISABLED) == disabled)
    {
        return;
    }

    if(disabled)
    {
        if(connector->connection_mode != PDC_CONNECTION_MODE_NONE)
        {
            pimu_device_connector_disconnect(connector);
        }

        connector->connection_mode = PDC_CONNECTION_MODE_DISABLED;
        pdc_debug_print(connector, PDC_DEBUG_TYPE_INFO, "Disabled");
    }
    else
    {
        connector->connection_mode = PDC_CONNECTION_MODE_NONE;
        pdc_debug_print(connector, PDC_DEBUG_TYPE_INFO, "Enabled");
    }
}

//--------------------------------------------------------------------+

void pimu_device_connector_set_debug_cb(PimuDeviceConnector *connector, PDCDebugCB callback)
{
    connector->debug_cb = callback;
}

void pimu_device_connector_set_send_message_data_cb(PimuDeviceConnector *connector, PDCSendMessageDataCB callback)
{
    connector->send_buffer.send_message_data_cb = callback;
}

void pimu_device_connector_set_lock_message_queue_cb(PimuDeviceConnector* connector, PDCLockMessageQueueCB callback)
{
    connector->lock_message_queue_cb = callback;
}

void pimu_device_connector_set_unlock_message_queue_cb(PimuDeviceConnector* connector, PDCUnlockMessageQueueCB callback)
{
    connector->unlock_message_queue_cb = callback;
}

//--------------------------------------------------------------------+

void pimu_device_connector_set_firmware_set_config_cb(PimuDeviceConnector *connector, PDCSetFirmwareConfigCB callback)
{
    connector->firmware_set_config_cb = callback;
}

void pimu_device_connector_set_firmware_get_config_cb(PimuDeviceConnector* connector, PDCGetFirmwareConfigCB callback)
{
    connector->firmware_get_config_cb = callback;
}

void pimu_device_connector_set_gamepad_set_colors_cb(PimuDeviceConnector *connector, PDCGetGamepadColorsCB callback)
{
    connector->gamepad_set_colors_cb = callback;
}

void pimu_device_connector_set_gamepad_get_colors_cb(PimuDeviceConnector *connector, PDCSetGamepadColorsCB callback)
{
    connector->gamepad_get_colors_cb = callback;
}

void pimu_device_connector_set_gamepad_set_inputs_cb(PimuDeviceConnector *connector, PDCSetGamepadInputsCB callback)
{
    connector->gamepad_set_inputs_cb = callback;
}
