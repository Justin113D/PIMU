#include "interfaces.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "pico/sync.h"

#include "debug.h"
#include "wireless/rfcomm.h"
#include "device.h"

//--------------------------------------------------------------------+
// Gamepad callbacks
//--------------------------------------------------------------------+

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
    pimu_device_connector_send_player_leds(ppf_device_connector_bluetooth, mask, flashing);
}

//--------------------------------------------------------------------+
// Connector callbacks
//--------------------------------------------------------------------+

PimuGamepadInputReport5 ppf_gamepad_input_report_5;
PimuGamepadInputReport9 ppf_gamepad_input_report_9;

static PimuDeviceConnectorGamepadInputs received_inputs;
static bool has_received_inputs;
static uint32_t previous_timestamp;

#ifdef BLUETOOTH_ENABLED

mutex_t ppf_bluetooth_mutex;

static void bluetooth_lock(PimuDeviceConnector *connector)
{
    mutex_enter_blocking(&ppf_bluetooth_mutex);
}

static void bluetooth_unlock(PimuDeviceConnector *connector)
{
    mutex_exit(&ppf_bluetooth_mutex);
}

static void bluetooth_debug_cb(PimuDeviceConnector *connector, PDC_DEBUG_TYPE type, const char *format, va_list args)
{
    const char *type_label = NULL;
    switch (type)
    {
    case PG_DEBUG_TYPE_INFO:
        type_label = " INFO  ] [PDC-BT";
        break;
    case PG_DEBUG_TYPE_WARNING:
        type_label = "WARNING] [PDC-BT";
        break;
    case PG_DEBUG_TYPE_ERROR:
        type_label = " ERROR ] [PDC-BT";
        break;
    }

    ppf_debug_printf_list(PDC_FIRMWARE_CONFIG_DEBUG_FEATURE_CONNECTOR, type_label, format, args);
}

static PDC_SEND_MESSAGE_DATA_RETURN_CODE bluetooth_send_message_data_cb(PimuDeviceConnector *connector, uint8_t *data, uint16_t data_length)
{
    if (ppf_rfcomm_send_data(data, data_length))
    {
        return PDC_SEND_MESSAGE_DATA_RETURN_CODE_SUCCESS_WAIT;
    }

    return PDC_SEND_MESSAGE_DATA_RETURN_CODE_FAILED;
}

static void bluetooth_set_config_cb(PimuDeviceConnector *connector, PimuDeviceConnectorFirmwareConfig *config)
{
    // Ignoring "bluetooth_disabled"; It should not be possible to disable bluetooth using bluetooth
    ppf_config_set_debug_mode(config->debug_mode);
    ppf_config_set_debug_features(config->debug_features);
    ppf_config_queue_save();
}

#endif

static void connector_connected_cb(PimuDeviceConnector *connector)
{
    ppf_device_start();
}

static void connector_get_config_cb(PimuDeviceConnector *connector, PimuDeviceConnectorFirmwareConfig *config)
{
    config->debug_mode = ppf_config_get_debug_mode();
    config->debug_features = ppf_config_get_debug_features();
    config->bluetooth_disabled = ppf_config_get_bluetooth_disabled();
}


static void connector_get_device_colors_cb(PimuDeviceConnector *connector, PimuDeviceConnectorGamepadColors *colors)
{
    *colors = *(PimuDeviceConnectorGamepadColors *)pimu_gamepad_get_device_colors(ppf_gamepad);
}

static void connector_set_device_colors_cb(PimuDeviceConnector *connector, PimuDeviceConnectorGamepadColors *colors)
{
    pimu_gamepad_set_device_colors(ppf_gamepad, (PimuGamepadDeviceColors *)colors);
    
    ppf_config_set_gamepad_colors(colors);
    ppf_config_queue_save();
}

static void connector_set_inputs_cb(PimuDeviceConnector *connector, PimuDeviceConnectorGamepadInputs *inputs)
{
    memcpy(&received_inputs, inputs, sizeof(received_inputs));
    has_received_inputs = true;

    memcpy(
        &ppf_gamepad_input_report_5.buttons,
        &inputs->buttons,
        sizeof(ppf_gamepad_input_report_5.buttons));

    ppf_gamepad_input_report_5.left_stick = pimu_gamepad_inputs_pack_vector(
        inputs->stick_left_x,
        inputs->stick_left_y);

    ppf_gamepad_input_report_5.right_stick = pimu_gamepad_inputs_pack_vector(
        inputs->stick_right_x,
        inputs->stick_right_y);

    pimu_gamepad_copy_inputs_5_to_9(
        &ppf_gamepad_input_report_5,
        &ppf_gamepad_input_report_9);

    uint32_t timestamp = time_us_32();

    if((pimu_gamepad_get_feature_active(ppf_gamepad) & 0x04) != 0)
    {
        if(previous_timestamp > 0)
        {
            ppf_gamepad_input_report_5.motion_data.timestamp += timestamp - previous_timestamp;
        }
        ppf_gamepad_input_report_5.motion_data.temperature = 0x100;
        ppf_gamepad_input_report_5.motion_data.accel_x = (int16_t)(inputs->accel_x * 0x1000);
        ppf_gamepad_input_report_5.motion_data.accel_y = (int16_t)(inputs->accel_y * 0x1000);
        ppf_gamepad_input_report_5.motion_data.accel_z = (int16_t)(inputs->accel_z * 0x1000);

        float gyro_z = inputs->gyro_z;
        if(gyro_z > 2000.0f)
        {
            gyro_z = 2000.0f;
        }
        else if(gyro_z < -2000.0f)
        {
            gyro_z = -2000.0f;
        }

        ppf_gamepad_input_report_5.motion_data.gyro_z = (int16_t)((gyro_z / 2000.0f) * 0x7FFF);
    }
    else
    {
        memset(&ppf_gamepad_input_report_5.motion_data, 0, sizeof(ppf_gamepad_input_report_5.motion_data));
    }

    previous_timestamp = timestamp;
}

//--------------------------------------------------------------------+
// Setup
//--------------------------------------------------------------------+

PimuGamepad *ppf_gamepad;

#ifdef BLUETOOTH_ENABLED
PimuDeviceConnector *ppf_device_connector_bluetooth;
#endif

void ppf_interfaces_init()
{
    ppf_gamepad_input_report_5.battery_voltage = 0x0ea5;
    ppf_gamepad_input_report_5.battery_charging_state = 0x20;
    ppf_gamepad_input_report_5.unknown_3[5] = 1;

    ppf_gamepad_input_report_9.unknown = 0x30;

    ppf_gamepad = pimu_gamepad_alloc();
    
    pimu_gamepad_set_name(ppf_gamepad, "Gamepad");
    pimu_gamepad_set_device_colors(ppf_gamepad, (PimuGamepadDeviceColors*)ppf_config_get_gamepad_colors());

    pimu_gamepad_set_debug_cb(ppf_gamepad, gamepad_debug_callback);
    pimu_gamepad_set_player_leds_changed_cb(ppf_gamepad, gamepad_player_leds_changed_cb);

#ifdef BLUETOOTH_ENABLED
    mutex_init(&ppf_bluetooth_mutex);

    ppf_device_connector_bluetooth = pimu_device_connector_alloc();
    pimu_device_connector_set_name(ppf_device_connector_bluetooth, "Bluetooth");
    pimu_device_connector_set_debug_cb(ppf_device_connector_bluetooth, bluetooth_debug_cb);
    pimu_device_connector_set_firmware_set_config_cb(ppf_device_connector_bluetooth, bluetooth_set_config_cb);
    pimu_device_connector_set_firmware_get_config_cb(ppf_device_connector_bluetooth, connector_get_config_cb);
    pimu_device_connector_set_send_message_data_cb(ppf_device_connector_bluetooth, bluetooth_send_message_data_cb);

    pimu_device_connector_set_connected_cb(ppf_device_connector_bluetooth, connector_connected_cb);
    pimu_device_connector_set_gamepad_get_colors_cb(ppf_device_connector_bluetooth, connector_get_device_colors_cb);
    pimu_device_connector_set_gamepad_set_colors_cb(ppf_device_connector_bluetooth, connector_set_device_colors_cb);
    pimu_device_connector_set_gamepad_set_inputs_cb(ppf_device_connector_bluetooth, connector_set_inputs_cb);
#endif
}

void ppf_interfaces_send_debug_string(uint8_t features, char* string)
{
    if(pimu_device_connector_get_connection_mode(ppf_device_connector_bluetooth) == PDC_CONNECTION_MODE_ESTABLISHED)
    {
        pimu_device_connector_send_debug_string(ppf_device_connector_bluetooth, features, string);
    }
}

void ppf_interfaces_send_debug_data(uint8_t features, uint8_t* data, uint16_t data_size)
{
    if(pimu_device_connector_get_connection_mode(ppf_device_connector_bluetooth) == PDC_CONNECTION_MODE_ESTABLISHED)
    {
        pimu_device_connector_send_debug_data(ppf_device_connector_bluetooth, features, data, data_size);
    }
}

void ppf_interfaces_update_inputs(void)
{
    bool imu_active = (pimu_gamepad_get_feature_active(ppf_gamepad) & 0x04) != 0;
    uint32_t timestamp = time_us_32();
    uint32_t timestamp_diff = timestamp - previous_timestamp;
    
    if(imu_active && previous_timestamp > 0)
    {
        ppf_gamepad_input_report_5.motion_data.timestamp += timestamp_diff;
        ppf_gamepad_input_report_5.motion_data.gyro_x = 0;
        ppf_gamepad_input_report_5.motion_data.gyro_y = 0;
        ppf_gamepad_input_report_5.motion_data.gyro_z = 0;
    }

    previous_timestamp = timestamp;

    if(!has_received_inputs)
    {
        return;
    }
    has_received_inputs = false;

    memcpy(
        &ppf_gamepad_input_report_5.buttons,
        &received_inputs.buttons,
        sizeof(ppf_gamepad_input_report_5.buttons)
    );

    ppf_gamepad_input_report_5.left_stick = pimu_gamepad_inputs_pack_vector(
        received_inputs.stick_left_x,
        received_inputs.stick_left_y
    );

    ppf_gamepad_input_report_5.right_stick = pimu_gamepad_inputs_pack_vector(
        received_inputs.stick_right_x,
        received_inputs.stick_right_y
    );

    if(imu_active)
    {
        ppf_gamepad_input_report_5.motion_data.temperature = 0x100;
        ppf_gamepad_input_report_5.motion_data.accel_x = (int16_t)(received_inputs.accel_x * 0x1000);
        ppf_gamepad_input_report_5.motion_data.accel_y = (int16_t)(received_inputs.accel_y * 0x1000);
        ppf_gamepad_input_report_5.motion_data.accel_z = (int16_t)(received_inputs.accel_z * 0x1000);

        float gyro_z = received_inputs.gyro_z;
        if(gyro_z > 2000.0f)
        {
            gyro_z = 2000.0f;
        }
        else if(gyro_z < -2000.0f)
        {
            gyro_z = -2000.0f;
        }

        ppf_gamepad_input_report_5.motion_data.gyro_z = (int16_t)((gyro_z / 2000.0f) * 0x7FFF);
        if(ppf_gamepad_input_report_5.motion_data.gyro_z != 0)
        {
            PPF_DEBUG_BLANK_BLUETOOTH("%04x", ppf_gamepad_input_report_5.motion_data.gyro_z);
        }
    }
    else
    {
        memset(&ppf_gamepad_input_report_5.motion_data, 0, sizeof(ppf_gamepad_input_report_5.motion_data));
    }

    pimu_gamepad_copy_inputs_5_to_9(
        &ppf_gamepad_input_report_5,
        &ppf_gamepad_input_report_9
    );
}