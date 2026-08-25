#ifndef PPF_CONFIG_H_
#define PPF_CONFIG_H_

#include <stdbool.h>

#include <pimu_device_connector.h>

void ppf_config_load(void);
void ppf_config_queue_save(void);
void ppf_config_poll_save(void);

void ppf_config_set_debug_mode(uint8_t debug_mode);
uint8_t ppf_config_get_debug_mode(void);

void ppf_config_set_debug_features(uint8_t debug_features);
uint8_t ppf_config_get_debug_features(void);

void ppf_config_set_bluetooth_disabled(bool bluetooth_disabled);
bool ppf_config_get_bluetooth_disabled(void);

void ppf_config_set_gamepad_colors(PimuDeviceConnectorGamepadColors *colors);
PimuDeviceConnectorGamepadColors* ppf_config_get_gamepad_colors(void);

#endif