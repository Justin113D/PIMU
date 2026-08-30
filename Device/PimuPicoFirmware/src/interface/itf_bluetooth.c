#include "itf_bluetooth.h"

#ifdef BLUETOOTH_ENABLED

#include "pico/sync.h"

#include "wireless/rfcomm.h"
#include "itf_connector.h"
#include "debug.h"

PimuDeviceConnector* ppf_device_connector_bluetooth;
mutex_t mutex_bluetooth;

static void bluetooth_lock(PimuDeviceConnector *connector)
{
    mutex_enter_blocking(&mutex_bluetooth);
}

static void bluetooth_unlock(PimuDeviceConnector *connector)
{
    mutex_exit(&mutex_bluetooth);
}

static void bluetooth_debug_cb(PimuDeviceConnector *connector, PDC_DEBUG_TYPE type, const char *format, va_list args)
{
    const char *type_label = NULL;
    switch (type)
    {
    case PDC_DEBUG_TYPE_INFO:
        type_label = " INFO  ] [PDC-BT";
        break;
    case PDC_DEBUG_TYPE_WARNING:
        type_label = "WARNING] [PDC-BT";
        break;
    case PDC_DEBUG_TYPE_ERROR:
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

void ppf_itf_bluetooth_init(void)
{
    mutex_init(&mutex_bluetooth);

    ppf_device_connector_bluetooth = pimu_device_connector_alloc();
    pimu_device_connector_set_name(ppf_device_connector_bluetooth, "Bluetooth");
    pimu_device_connector_set_debug_cb(ppf_device_connector_bluetooth, bluetooth_debug_cb);
    pimu_device_connector_set_firmware_set_config_cb(ppf_device_connector_bluetooth, bluetooth_set_config_cb);
    pimu_device_connector_set_send_message_data_cb(ppf_device_connector_bluetooth, bluetooth_send_message_data_cb);

    ppf_itf_connector_set_common_callbacks(ppf_device_connector_bluetooth);
}

#else

void ppf_itf_bluetooth_init(void)
{
    ppf_device_connector_bluetooth = NULL;
}

#endif