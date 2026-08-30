#ifndef PPF_DEBUG_H_
#define PPF_DEBUG_H_

#include <stdarg.h>

#include "pimu_device_connector.h"

#include "config.h"

void ppf_debug_init();
void ppf_debug_printf_list(uint8_t features, const char *type, const char *format, va_list args);
void ppf_debug_printf(uint8_t features, const char *type, const char *format, ...);

#define VA_ARGS(...) , ##__VA_ARGS__

#define PPF_DEBUG_INFO(_features, _format, ...) ppf_debug_printf(_features, " INFO  ", _format VA_ARGS(__VA_ARGS__))
#define PPF_DEBUG_WARNING(_features, _format, ...) ppf_debug_printf(_features, "WARNING", _format VA_ARGS(__VA_ARGS__))
#define PPF_DEBUG_ERROR(_features, _format, ...) ppf_debug_printf(_features, " ERROR ", _format VA_ARGS(__VA_ARGS__))
#define PPF_DEBUG_BLANK(_features, _format, ...) ppf_debug_printf(_features, NULL, _format VA_ARGS(__VA_ARGS__))

#define PPF_DEBUG_INFO_BLUETOOTH(_format, ...) PPF_DEBUG_INFO(PDC_FIRMWARE_CONFIG_DEBUG_FEATURE_BLUETOOTH, _format, __VA_ARGS__)
#define PPF_DEBUG_WARNING_BLUETOOTH(_format, ...) PPF_DEBUG_WARNING(PDC_FIRMWARE_CONFIG_DEBUG_FEATURE_BLUETOOTH, _format, __VA_ARGS__)
#define PPF_DEBUG_ERROR_BLUETOOTH(_format, ...) PPF_DEBUG_ERROR(PDC_FIRMWARE_CONFIG_DEBUG_FEATURE_BLUETOOTH, _format, __VA_ARGS__)
#define PPF_DEBUG_BLANK_BLUETOOTH(_format, ...) PPF_DEBUG_BLANK(PDC_FIRMWARE_CONFIG_DEBUG_FEATURE_BLUETOOTH, _format, __VA_ARGS__)

#define PPF_DEBUG_INFO_USB(_format, ...) PPF_DEBUG_INFO(PDC_FIRMWARE_CONFIG_DEBUG_FEATURE_USB, _format, __VA_ARGS__)
#define PPF_DEBUG_WARNING_USB(_format, ...) PPF_DEBUG_WARNING(PDC_FIRMWARE_CONFIG_DEBUG_FEATURE_USB, _format, __VA_ARGS__)
#define PPF_DEBUG_ERROR_USB(_format, ...) PPF_DEBUG_ERROR(PDC_FIRMWARE_CONFIG_DEBUG_FEATURE_USB, _format, __VA_ARGS__)
#define PPF_DEBUG_BLANK_USB(_format, ...) PPF_DEBUG_BLANK(PDC_FIRMWARE_CONFIG_DEBUG_FEATURE_USB, _format, __VA_ARGS__)

#endif