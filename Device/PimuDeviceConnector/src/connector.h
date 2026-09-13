#ifndef PDC_CONNECTOR_H_
#define PDC_CONNECTOR_H_

#include "pimu_device_connector.h"

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

void pdc_debug_print(PimuDeviceConnector* connector, PDC_DEBUG_TYPE type, char* format, ...);

#endif