#ifndef PPF_ITF_IMU_H_
#define PPF_ITF_IMU_H_

#include "pimu_gamepad_input.h"
#include <stdbool.h>

void ppf_imu_update(PimuGamepadIMUData* imu_data, uint8_t omitted_quaternion_index, uint16_t* quaternion, bool calculate_gyro);

#endif