#include "pimu_device_connector.h"
#include "connector.h"

#include <stdlib.h>
#include <string.h>

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
    case PDC_MESSAGE_DESTINATION_DEVICE_INPUT:

        if (connector->gamepad_set_inputs_cb == NULL)
        {
            break;
        }
        
        uint16_t input_size = 0;
        uint8_t input_flags = message->header.data_type;

        if(input_flags & PDC_INPUT_FLAG_BUTTONS)
        {
            input_size += sizeof(PimuDeviceConnectorInputButtons);
        }

        if(input_flags & PDC_INPUT_FLAG_STICK_LEFT)
        {
            input_size += sizeof(PimuDeviceConnectorInputStick);
        }

        if(input_flags & PDC_INPUT_FLAG_STICK_RIGHT)
        {
            input_size += sizeof(PimuDeviceConnectorInputStick);
        }

        if(input_flags & PDC_INPUT_FLAG_GYRO)
        {
            input_size += sizeof(PimuDeviceConnectorInputGyro);
        }        
        
        if (message->header.data_size < input_size)
        {
            break;
        }

        PimuDeviceConnectorInputs inputs = {
            .input_flags = input_flags
        };
        
        uint8_t* input_data = message->data;

        if(input_flags & PDC_INPUT_FLAG_BUTTONS)
        {
            inputs.buttons = *(PimuDeviceConnectorInputButtons*)input_data;
            input_data += sizeof(PimuDeviceConnectorInputButtons);
        }

        if(input_flags & PDC_INPUT_FLAG_STICK_LEFT)
        {
            inputs.stick_left = *(PimuDeviceConnectorInputStick*)input_data;
            input_data += sizeof(PimuDeviceConnectorInputStick);
        }

        if(input_flags & PDC_INPUT_FLAG_STICK_RIGHT)
        {
            inputs.stick_right = *(PimuDeviceConnectorInputStick*)input_data;
            input_data += sizeof(PimuDeviceConnectorInputStick);
        }

        if(input_flags & PDC_INPUT_FLAG_GYRO)
        {
            inputs.gyro = *(PimuDeviceConnectorInputGyro*)input_data;
            input_data += sizeof(PimuDeviceConnectorInputGyro);
        } 

        connector->gamepad_set_inputs_cb(connector, &inputs);
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
