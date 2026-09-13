#ifndef PIMU_DEVICE_CONNECTOR_H_
#define PIMU_DEVICE_CONNECTOR_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#ifndef PACKED_STRUCT
#define PACKED_STRUCT struct __attribute__((packed))
#endif

#define PDC_MESSAGE_DATA_MAX_SIZE 256

#define PDC_FIRMWARE_CONFIG_DEBUG_MODE_NONE 0
#define PDC_FIRMWARE_CONFIG_DEBUG_MODE_UART_ONLY 1
#define PDC_FIRMWARE_CONFIG_DEBUG_MODE_CONNECTOR 2

#define PDC_FIRMWARE_CONFIG_DEBUG_FEATURE_BLUETOOTH 0x01
#define PDC_FIRMWARE_CONFIG_DEBUG_FEATURE_USB 0x02
#define PDC_FIRMWARE_CONFIG_DEBUG_FEATURE_CONNECTOR 0x04
#define PDC_FIRMWARE_CONFIG_DEBUG_FEATURE_GAMEPAD 0x08

#define PDC_INPUT_FLAG_BUTTONS 0x01
#define PDC_INPUT_FLAG_STICK_LEFT 0x02
#define PDC_INPUT_FLAG_STICK_RIGHT 0x04
#define PDC_INPUT_FLAG_GYRO 0x08

typedef PACKED_STRUCT
{
    uint8_t debug_mode;
    uint8_t debug_features;
    bool bluetooth_disabled;
} PimuDeviceConnectorFirmwareConfig;

typedef PACKED_STRUCT
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} PimuDeviceConnectorGamepadColor;

typedef PACKED_STRUCT
{
    PimuDeviceConnectorGamepadColor body_color;
    PimuDeviceConnectorGamepadColor buttons_color;
    PimuDeviceConnectorGamepadColor highlight_color;
    PimuDeviceConnectorGamepadColor grip_color;
} PimuDeviceConnectorGamepadColors;

typedef PACKED_STRUCT
{
    uint8_t b : 1;
    uint8_t a : 1;
    uint8_t y : 1;
    uint8_t x : 1;
    uint8_t r : 1;
    uint8_t zr : 1;
    uint8_t plus : 1;
    uint8_t stick_right : 1;
    
    uint8_t dpad_down : 1;
    uint8_t dpad_right : 1;
    uint8_t dpad_left : 1;
    uint8_t dpad_up : 1;
    uint8_t l : 1;
    uint8_t zl : 1;
    uint8_t minus : 1;
    uint8_t stick_left : 1;
    
    uint8_t home : 1;
    uint8_t capture : 1;
    uint8_t gr : 1;
    uint8_t gl : 1;
    uint8_t chat : 1;
    uint8_t unknown_21 : 1;
    uint8_t unknown_22 : 1;
    uint8_t unknown_23 : 1;
} PimuDeviceConnectorInputButtons;

typedef PACKED_STRUCT
{
    uint16_t x : 12;
    uint16_t y : 12;
} PimuDeviceConnectorInputStick;

typedef PACKED_STRUCT
{
    uint16_t quat_1;
    uint16_t quat_2;
    uint16_t quat_3;
    uint8_t quat_extra;
} PimuDeviceConnectorInputGyro;

typedef PACKED_STRUCT
{   
    uint8_t input_flags;
    PimuDeviceConnectorInputButtons buttons;
    PimuDeviceConnectorInputStick stick_left;
    PimuDeviceConnectorInputStick stick_right;
    PimuDeviceConnectorInputGyro gyro;
} PimuDeviceConnectorInputs;

//--------------------------------------------------------------------+

typedef struct PimuDeviceConnector PimuDeviceConnector;

PimuDeviceConnector* pimu_device_connector_alloc(void);
void pimu_device_connector_free(PimuDeviceConnector* connector);

void pimu_device_connector_set_name(PimuDeviceConnector* connector, const char* name);
const char* pimu_device_connector_get_name(PimuDeviceConnector* connector);

//--------------------------------------------------------------------+

typedef enum
{
    PDC_CONNECTION_MODE_NONE,
    PDC_CONNECTION_MODE_HANDSHAKING,
    PDC_CONNECTION_MODE_ESTABLISHED,
    PDC_CONNECTION_MODE_DISABLED
} PDC_CONNECTION_MODE;

PDC_CONNECTION_MODE pimu_device_connector_get_connection_mode(PimuDeviceConnector *connector);
void pimu_device_connector_disconnect(PimuDeviceConnector *connector);

typedef void (*PDCConnected)(PimuDeviceConnector* source);
void pimu_device_connector_set_connected_cb(PimuDeviceConnector *connector, PDCConnected callback);

typedef void (*PDCDisconnected)(PimuDeviceConnector* source);
void pimu_device_connector_set_disconnected_cb(PimuDeviceConnector *connector, PDCDisconnected callback);

void pimu_device_connector_set_disabled(PimuDeviceConnector *connector, bool disabled);

//--------------------------------------------------------------------+

typedef enum
{
    PDC_DEBUG_TYPE_NONE,
    PDC_DEBUG_TYPE_INFO,
    PDC_DEBUG_TYPE_WARNING,
    PDC_DEBUG_TYPE_ERROR,
} PDC_DEBUG_TYPE;

typedef void (*PDCDebugCB)(PimuDeviceConnector* source, PDC_DEBUG_TYPE type, const char* format, va_list args);
void pimu_device_connector_set_debug_cb(PimuDeviceConnector* connector, PDCDebugCB callback);

typedef enum
{
  PDC_SEND_MESSAGE_DATA_RETURN_CODE_FAILED,
  PDC_SEND_MESSAGE_DATA_RETURN_CODE_SUCCESS,
  PDC_SEND_MESSAGE_DATA_RETURN_CODE_SUCCESS_WAIT
} PDC_SEND_MESSAGE_DATA_RETURN_CODE;

typedef PDC_SEND_MESSAGE_DATA_RETURN_CODE (*PDCSendMessageDataCB)(PimuDeviceConnector* source, uint8_t* data, uint16_t data_length);
void pimu_device_connector_set_send_message_data_cb(PimuDeviceConnector* connector, PDCSendMessageDataCB callback);

typedef void (*PDCLockMessageQueueCB)(PimuDeviceConnector* source);
void pimu_device_connector_set_lock_message_queue_cb(PimuDeviceConnector* connector, PDCLockMessageQueueCB callback);

typedef void (*PDCUnlockMessageQueueCB)(PimuDeviceConnector* source);
void pimu_device_connector_set_unlock_message_queue_cb(PimuDeviceConnector* connector, PDCUnlockMessageQueueCB callback);

//--------------------------------------------------------------------+

typedef void (*PDCSetFirmwareConfigCB)(PimuDeviceConnector* source, PimuDeviceConnectorFirmwareConfig* config);
void pimu_device_connector_set_firmware_set_config_cb(PimuDeviceConnector* connector, PDCSetFirmwareConfigCB callback);

typedef void (*PDCGetFirmwareConfigCB)(PimuDeviceConnector* source, PimuDeviceConnectorFirmwareConfig* config);
void pimu_device_connector_set_firmware_get_config_cb(PimuDeviceConnector* connector, PDCGetFirmwareConfigCB callback);

typedef void (*PDCGetGamepadColorsCB)(PimuDeviceConnector* source, PimuDeviceConnectorGamepadColors* destination);
void pimu_device_connector_set_gamepad_set_colors_cb(PimuDeviceConnector* connector, PDCGetGamepadColorsCB callback);

typedef void (*PDCSetGamepadColorsCB)(PimuDeviceConnector* source, PimuDeviceConnectorGamepadColors* colors);
void pimu_device_connector_set_gamepad_get_colors_cb(PimuDeviceConnector* connector, PDCSetGamepadColorsCB callback);

typedef void (*PDCSetGamepadInputsCB)(PimuDeviceConnector* source, PimuDeviceConnectorInputs* inputs);
void pimu_device_connector_set_gamepad_set_inputs_cb(PimuDeviceConnector* connector, PDCSetGamepadInputsCB callback);

//--------------------------------------------------------------------+

bool pimu_device_connector_read_bytes(PimuDeviceConnector *connector, uint8_t* data, uint16_t size);
void pimu_device_connector_poll(PimuDeviceConnector *connector);
void pimu_device_connector_confirm_message_sent(PimuDeviceConnector *connector);

void pimu_device_connector_send_debug_string(PimuDeviceConnector *connector, uint8_t features, char* string);
void pimu_device_connector_send_debug_data(PimuDeviceConnector *connector, uint8_t features, uint8_t* data, uint16_t data_length);
void pimu_device_connector_send_player_leds(PimuDeviceConnector *connector, uint8_t player_leds_mask, bool flashing);

#endif