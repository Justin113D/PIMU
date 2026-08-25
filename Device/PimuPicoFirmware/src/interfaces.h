#ifndef PPF_INTERFACE_H_
#define PPF_INTERFACE_H_

#include <pimu_gamepad.h>
#include <pimu_gamepad_input.h>
#include <pimu_device_connector.h>

extern PimuGamepad* ppf_gamepad;
extern PimuGamepadInputReport5 ppf_gamepad_input_report_5;
extern PimuGamepadInputReport9 ppf_gamepad_input_report_9;

extern PimuDeviceConnector* ppf_device_connector_bluetooth;

void ppf_interfaces_init();
void ppf_interfaces_send_debug_string(uint8_t features, char* string);
void ppf_interfaces_send_debug_data(uint8_t features, uint8_t* data, uint16_t data_size);
void ppf_interfaces_update_inputs(void);

#endif