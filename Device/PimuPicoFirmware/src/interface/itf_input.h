#ifndef PPF_ITF_INPUT_H_
#define PPF_ITF_INPUT_H_

#include "pimu_gamepad_input.h"
#include "pimu_device_connector.h"

extern PimuGamepadInputReport5 ppf_gamepad_input_report_5;
extern PimuGamepadInputReport9 ppf_gamepad_input_report_9;

void ppf_itf_input_init(void);
void ppf_itf_input_receive(PimuDeviceConnectorInputs* inputs);
void ppf_itf_input_update(void);

#endif