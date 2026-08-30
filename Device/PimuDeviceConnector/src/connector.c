#include "pimu_device_connector.h"

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "read_buffer.h"
#include "send_buffer.h"

#define PIMO_DEVICE_CONNECTOR_VERSION 1

struct PimuDeviceConnector
{
    const char *name;
    PDC_CONNECTION_MODE connection_mode;

    uint8_t player_leds_mask;
    bool player_leds_flashing;

    PDCDebugCB debug_cb;
    PDCGetFirmwareConfigCB firmware_get_config_cb;
    PDCSetFirmwareConfigCB firmware_set_config_cb;
    PDCGetGamepadColorsCB gamepad_get_colors_cb;
    PDCSetGamepadColorsCB gamepad_set_colors_cb;
    PDCSetGamepadInputsCB gamepad_set_inputs_cb;
    PDCConnected connected_cb;
    PDCDisconnected disconnected_cb;
    PDCLockMessageQueueCB lock_message_queue_cb;
    PDCUnlockMessageQueueCB unlock_message_queue_cb;

    PDCReadBuffer read_buffer;
    PDCSendBuffer send_buffer;
};

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

//--------------------------------------------------------------------+

#define CHECK_BEFORE_SEND(source)                                                                                            \
    if (connector->connection_mode != PDC_CONNECTION_MODE_ESTABLISHED)                                                       \
    {                                                                                                                        \
        pdc_debug_print(connector, PDC_DEBUG_TYPE_ERROR, "Attempted to send a \"" source "\" message while not connected!"); \
        return;                                                                                                              \
    }  \
    else if (connector->connection_mode == PDC_CONNECTION_MODE_DISABLED) \
    { \
        pdc_debug_print(connector, PDC_DEBUG_TYPE_ERROR, "Attempted to send a \"" source "\" message while disabled!"); \
        return; \
    }

static void queue_message(PimuDeviceConnector *connector, PDCMessage *message)
{
    if(connector->lock_message_queue_cb != NULL)
    {
        connector->lock_message_queue_cb(connector);
    }    

    pdc_send_buffer_queue_message(connector, &connector->send_buffer, message);

    if(connector->unlock_message_queue_cb != NULL)
    {
        connector->unlock_message_queue_cb(connector);
    }        
}

static void send_debug(PimuDeviceConnector *connector, uint8_t features, uint8_t data_type, uint8_t *data, uint16_t data_length)
{
    uint32_t remaining_length = data_length;
    uint8_t *data_offset = data;

    const uint16_t max_debug_size = PDC_MESSAGE_DATA_MAX_SIZE - 1;

    while (remaining_length > 0)
    {
        PDCMessage message = {
            .header.destination = PDC_MESSAGE_DESTINATION_DESKTOP_DEBUG,
            .header.data_type = data_type,
            .header.data_size = remaining_length,
        };

        if (message.header.data_size > max_debug_size)
        {
            message.header.data_size = max_debug_size;
        }

        memcpy(&message.data[1], data_offset, message.header.data_size);
        queue_message(connector, &message);

        remaining_length -= message.header.data_size;
        data_offset += message.header.data_size;

        message.data[0] = features;
        message.header.data_size++;
    }
}

void pimu_device_connector_send_debug_string(PimuDeviceConnector *connector, uint8_t features, char *string)
{
    CHECK_BEFORE_SEND("DEBUG.STRING")
    send_debug(connector, features, PDC_MESSAGE_DESKTOP_DEBUG_DATA_TYPE_STRING, string, strlen(string));
}

void pimu_device_connector_send_debug_data(PimuDeviceConnector *connector, uint8_t features, uint8_t *data, uint16_t data_length)
{
    CHECK_BEFORE_SEND("DEBUG.DATA")
    send_debug(connector, features, PDC_MESSAGE_DESKTOP_DEBUG_DATA_TYPE_DATA, data, data_length);
}

static void send_stored_player_leds(PimuDeviceConnector *connector)
{
    PDCMessage message = {
        .header.destination = PDC_MESSAGE_DESTINATION_DESKTOP_GAMEPAD,
        .header.data_type = PDC_MESSAGE_DESKTOP_GAMEPAD_DATA_TYPE_PLAYER_LEDS,
        .header.data_size = 2,
        .data = {
            connector->player_leds_mask,
            connector->player_leds_flashing}};

    queue_message(connector, &message);
}

void pimu_device_connector_send_player_leds(PimuDeviceConnector *connector, uint8_t player_leds_mask, bool flashing)
{
    connector->player_leds_mask = player_leds_mask;
    connector->player_leds_flashing = flashing;

    CHECK_BEFORE_SEND("DESKTOP_GAMEPAD.PLAYER_LEDS")

    send_stored_player_leds(connector);
}

//--------------------------------------------------------------------+

static void handle_received_message(PimuDeviceConnector *connector)
{
    PDCMessage *message = &connector->read_buffer.deserialized_message;
    switch (message->header.destination)
    {
    case PDC_MESSAGE_DESTINATION_DEVICE_DISCONNECT:
        pdc_debug_print(connector, PDC_DEBUG_TYPE_INFO, "Received Disconnect request");
        pimu_device_connector_disconnect(connector);
        break;
    case PDC_MESSAGE_DESTINATION_DEVICE_FIRMWARE:
        switch (message->header.data_type)
        {
        case PDC_MESSAGE_DEVICE_FIRMWARE_DATA_TYPE_REQUEST_CONFIG:
            pdc_debug_print(connector, PDC_DEBUG_TYPE_INFO, "Received Firmware request config request");

            if (connector->firmware_get_config_cb != NULL)
            {
                PDCMessage message = {
                    .header.destination = PDC_MESSAGE_DESTINATION_DESKTOP_FIRMWARE,
                    .header.data_type = PDC_MESSAGE_DESKTOP_FIRMWARE_DATA_TYPE_CURRENT_CONFIG,
                    .header.data_size = sizeof(PimuDeviceConnectorFirmwareConfig),
                    .data = {0}
                };

                connector->firmware_get_config_cb(connector, (PimuDeviceConnectorFirmwareConfig *)&message.data);
                
                queue_message(connector, &message);
            }
            break;
        case PDC_MESSAGE_DEVICE_FIRMWARE_DATA_TYPE_SET_CONFIG:
            pdc_debug_print(connector, PDC_DEBUG_TYPE_INFO, "Received Firmware set config");
            if (message->header.data_size >= sizeof(PimuDeviceConnectorFirmwareConfig) && connector->firmware_set_config_cb != NULL)
            {
                connector->firmware_set_config_cb(connector, (PimuDeviceConnectorFirmwareConfig *)message->data);
            }
            break;
        default:
            pdc_debug_print(connector, PDC_DEBUG_TYPE_WARNING, "Received unknown/invalid firmware message %i", message->header.data_type);
            break;
        }
        break;

    case PDC_MESSAGE_DESTINATION_DEVICE_GAMEPAD:
        switch (message->header.data_type)
        {

        case PDC_MESSAGE_DEVICE_GAMEPAD_DATA_TYPE_INPUT:
            // May be a bit much to print all the time... I just left it here for developer testing
            // pdc_debug_print(connector, PDC_DEBUG_TYPE_INFO, "Received gamepad input");

            if (message->header.data_size >= sizeof(PimuDeviceConnectorGamepadInputs) && connector->gamepad_set_inputs_cb != NULL)
            {
                connector->gamepad_set_inputs_cb(connector, (PimuDeviceConnectorGamepadInputs *)message->data);
            }
            break;

        case PDC_MESSAGE_DEVICE_GAMEPAD_DATA_TYPE_REQUEST_COLORS:
            pdc_debug_print(connector, PDC_DEBUG_TYPE_INFO, "Received Gamepad request colors request");

            if (connector->gamepad_get_colors_cb != NULL)
            {
                PDCMessage message = {
                    .header.destination = PDC_MESSAGE_DESTINATION_DESKTOP_GAMEPAD,
                    .header.data_type = PDC_MESSAGE_DESKTOP_GAMEPAD_DATA_TYPE_CURRENT_COLORS,
                    .header.data_size = sizeof(PimuDeviceConnectorGamepadColors),
                    .data = {0}};

                connector->gamepad_get_colors_cb(connector, (PimuDeviceConnectorGamepadColors *)&message.data);

                queue_message(connector, &message);
            }
            break;

        case PDC_MESSAGE_DEVICE_GAMEPAD_DATA_TYPE_SET_COLORS:
            pdc_debug_print(connector, PDC_DEBUG_TYPE_INFO, "Received Gamepad set colors");
            if (message->header.data_size >= sizeof(PimuDeviceConnectorGamepadColors) && connector->gamepad_set_colors_cb != NULL)
            {
                connector->gamepad_set_colors_cb(connector, (PimuDeviceConnectorGamepadColors *)message->data);
            }
            break;
        case PDC_MESSAGE_DEVICE_GAMEPAD_DATA_TYPE_REQUEST_PLAYER_LEDS:
            pdc_debug_print(connector, PDC_DEBUG_TYPE_INFO, "Received Gamepad request player leds");
            send_stored_player_leds(connector);
            break;
        default:
            pdc_debug_print(connector, PDC_DEBUG_TYPE_WARNING, "Received unknown/invalid gamepad message %i", message->header.data_type);
            break;
        }

        break;
    default:
        pdc_debug_print(connector, PDC_DEBUG_TYPE_WARNING, "Received unknown/invalid message destination %i", message->header.destination);
        break;
    }
}

static void handle_handshake(PimuDeviceConnector *connector)
{
    if (connector->read_buffer.deserialized_message.header.data_type == 0)
    {
        connector->connection_mode = PDC_CONNECTION_MODE_ESTABLISHED;

        if (connector->connected_cb != NULL)
        {
            connector->connected_cb(connector);
        }

        pdc_debug_print(connector, PDC_DEBUG_TYPE_INFO, "Connected");
    }
    else
    {
        connector->connection_mode = PDC_CONNECTION_MODE_HANDSHAKING;
        pdc_debug_print(connector, PDC_DEBUG_TYPE_INFO, "Received Handshake");

        PDCMessage message = {
            .header.destination = PDC_MESSAGE_DESTINATION_DESKTOP_HANDSHAKE,
            .header.data_type = PIMO_DEVICE_CONNECTOR_VERSION,
            .header.data_size = connector->read_buffer.deserialized_message.header.data_size,
            .data = {0}};

        memcpy(message.data, connector->read_buffer.deserialized_message.data, message.header.data_size);

        // Something is weird with the serial reader being used in the desktop app;
        // It completely ignores the first batch of data coming in, so we just send 2 handshakes for good measure
        queue_message(connector, &message);
        queue_message(connector, &message);
    }
}

bool pimu_device_connector_read_bytes(PimuDeviceConnector *connector, uint8_t* data, uint16_t size)
{
    if(connector->connection_mode == PDC_CONNECTION_MODE_DISABLED)
    {
        return false;
    }

    bool result = false;

    for(int i = 0; i < size; i++)
    {
        if (pdc_read_buffer_deserialize_byte(&connector->read_buffer, data[i]))
        {
            if (connector->connection_mode == PDC_CONNECTION_MODE_ESTABLISHED)
            {
                handle_received_message(connector);
            }
            else if (connector->read_buffer.deserialized_message.header.destination == PDC_MESSAGE_DESTINATION_DEVICE_HANDSHAKE)
            {
                handle_handshake(connector);
            }
    
            result |= true;
        }
    }


    return result;
}

void pimu_device_connector_poll(PimuDeviceConnector *connector)
{
    if(connector->connection_mode == PDC_CONNECTION_MODE_DISABLED)
    {
        return;
    }

    pdc_send_buffer_poll(connector, &connector->send_buffer);
}

void pimu_device_connector_confirm_message_sent(PimuDeviceConnector *connector)
{
    if(connector->connection_mode == PDC_CONNECTION_MODE_DISABLED)
    {
        return;
    }

    pdc_send_buffer_confirm_message_sent(&connector->send_buffer);
}
