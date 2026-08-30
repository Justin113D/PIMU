#include "itf_uart.h"

#include "pico/stdlib.h"
#include "pico/sync.h"

#include "itf_connector.h"
#include "debug.h"

#define CONNECTOR_UART uart0
#define CONNECTOR_BAUDRATE 115200
#define CONNECTOR_TX_PIN 0
#define CONNECTOR_RX_PIN 1

PimuDeviceConnector* ppf_device_connector_uart;
mutex_t mutex_uart;

static void uart_lock(PimuDeviceConnector *connector)
{
    mutex_enter_blocking(&mutex_uart);
}

static void uart_unlock(PimuDeviceConnector *connector)
{
    mutex_exit(&mutex_uart);
}

static void uart_debug_cb(PimuDeviceConnector *connector, PDC_DEBUG_TYPE type, const char *format, va_list args)
{
    const char *type_label = NULL;
    switch (type)
    {
    case PDC_DEBUG_TYPE_INFO:
        type_label = " INFO  ] [PDC-UART";
        break;
    case PDC_DEBUG_TYPE_WARNING:
        type_label = "WARNING] [PDC-UART";
        break;
    case PDC_DEBUG_TYPE_ERROR:
        type_label = " ERROR ] [PDC-UART";
        break;
    }

    ppf_debug_printf_list(PDC_FIRMWARE_CONFIG_DEBUG_FEATURE_CONNECTOR, type_label, format, args);
}

static PDC_SEND_MESSAGE_DATA_RETURN_CODE uart_send_message_data_cb(PimuDeviceConnector *connector, uint8_t *data, uint16_t data_length)
{
    uart_write_blocking(CONNECTOR_UART, data, data_length);
    return PDC_SEND_MESSAGE_DATA_RETURN_CODE_SUCCESS;
}

static void uart_set_config_cb(PimuDeviceConnector *connector, PimuDeviceConnectorFirmwareConfig *config)
{
    ppf_config_set_debug_mode(config->debug_mode);
    ppf_config_set_debug_features(config->debug_features);
    ppf_config_set_bluetooth_disabled(config->bluetooth_disabled);
    ppf_config_queue_save();
}

void ppf_itf_uart_init(void)
{
    stdio_uart_init_full(CONNECTOR_UART, CONNECTOR_BAUDRATE, CONNECTOR_TX_PIN, CONNECTOR_RX_PIN);
    mutex_init(&mutex_uart);

    ppf_device_connector_uart = pimu_device_connector_alloc();
    pimu_device_connector_set_name(ppf_device_connector_uart, "uart");
    pimu_device_connector_set_debug_cb(ppf_device_connector_uart, uart_debug_cb);
    pimu_device_connector_set_firmware_set_config_cb(ppf_device_connector_uart, uart_set_config_cb);
    pimu_device_connector_set_send_message_data_cb(ppf_device_connector_uart, uart_send_message_data_cb);

    ppf_itf_connector_set_common_callbacks(ppf_device_connector_uart);
}

void ppf_itf_uart_poll(void)
{
    static uint8_t read_buffer[512];
    uint16_t read_buffer_size = 0;

    while(uart_is_readable(CONNECTOR_UART))
    {
        uart_read_blocking(CONNECTOR_UART, &read_buffer[read_buffer_size], 1);
        read_buffer_size++;

        if(read_buffer_size == sizeof(read_buffer))
        {
            ppf_itf_connector_read_bytes(PPF_CONNECTOR_TYPE_UART, read_buffer, read_buffer_size);
            read_buffer_size = 0;
        }
    }

    if(read_buffer_size > 0)
    {
        ppf_itf_connector_read_bytes(PPF_CONNECTOR_TYPE_UART, read_buffer, read_buffer_size);
    }

    pimu_device_connector_poll(ppf_device_connector_uart);
}