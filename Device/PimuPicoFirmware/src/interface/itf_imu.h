#ifndef PPF_ITF_IMU_H_
#define PPF_ITF_IMU_H_

#include "pimu_gamepad_input.h"

void ppf_imu_update(PimuGamepadIMUData* imu_data, float* old_quaternion, float* new_quaternion);

#endif