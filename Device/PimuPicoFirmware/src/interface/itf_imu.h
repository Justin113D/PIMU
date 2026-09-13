#ifndef PPF_ITF_IMU_H_
#define PPF_ITF_IMU_H_

#include "pimu_gamepad_input.h"
#include "pimu_device_connector.h"
#include <stdbool.h>

void ppf_imu_update(PimuGamepadIMUData* imu_data, PimuDeviceConnectorInputGyro* received_inputs, bool calculate_gyro);

#endif