#include "debug.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "pico/sync.h"

#include "interfaces.h"

#define DEBUG_UART uart1
#define DEBUG_BAUDRATE 115200
#define DEBUG_TX_PIN 8
#define DEBUG_RX_PIN 9

static mutex_t debug_mutex;

void ppf_debug_init(void)
{
    stdio_uart_init_full(DEBUG_UART, DEBUG_BAUDRATE, DEBUG_TX_PIN, DEBUG_RX_PIN);
    mutex_init(&debug_mutex);
}

void ppf_debug_printf_list(uint8_t features, const char *type, const char *format, va_list args)
{
    if(ppf_config_get_debug_mode() == PDC_FIRMWARE_CONFIG_DEBUG_MODE_NONE
        || (features != 0 && (ppf_config_get_debug_features() & features) == 0))
    {
        return;
    }

    mutex_enter_blocking(&debug_mutex);

    char debug_string[PDC_MESSAGE_DATA_MAX_SIZE];
    int32_t debug_string_size = vsnprintf(debug_string, sizeof(debug_string) - 1, format, args);
    debug_string[debug_string_size] = 0;

    char debug_message[PDC_MESSAGE_DATA_MAX_SIZE];
    int32_t debug_message_size = 0;
    
    if(type != NULL)
    {
        uint32_t time = time_us_32();
    
        uint32_t micro_seceonds = time % 1000000;

        time /= 1000000;
        uint8_t seconds = (uint8_t)(time % 60);
    
        time /= 60;
        uint8_t minutes = (uint8_t)(time % 60);
    
        time /= 60;
        uint8_t hours = (uint8_t)(time % 60);
    
        debug_message_size = snprintf(debug_message, sizeof(debug_message) - 1, "[%02i:%02i:%02i.%06i] [%s] %s\n", hours, minutes, seconds, micro_seceonds, type, debug_string);
    }
    else
    {
        debug_message_size = snprintf(debug_message, sizeof(debug_message) - 1, "%s\n", debug_string);
    }

    debug_message[debug_message_size] = 0;
    debug_message_size++;
    
    uart_write_blocking(DEBUG_UART, debug_message, debug_message_size);

    if((features & PDC_FIRMWARE_CONFIG_DEBUG_FEATURE_BLUETOOTH) == 0 
        && ppf_config_get_debug_mode() == PDC_FIRMWARE_CONFIG_DEBUG_MODE_CONNECTOR)
    {
        ppf_interfaces_send_debug_string(features, debug_message);
    }

    mutex_exit(&debug_mutex);
}

void ppf_debug_printf(uint8_t features, const char *type, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    ppf_debug_printf_list(features, type, format, args);
    va_end(args);
}