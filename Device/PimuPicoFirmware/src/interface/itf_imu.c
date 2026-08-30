#include "itf_imu.h"

#include <stdlib.h>
#include <math.h>

#include "debug.h"

#define ABSF(_v) ((_v) < 0 ? -(_v) : (_v))

static void pack_quaternion(PimuGamepadIMUData* imu_data, float* quaternion)
{
    imu_data->quaternion_omitted_index = 0;
    float abs_quat_max = ABSF(quaternion[0]);
    
    for(int i = 1; i < 4; i++)
    {
        float abs_quat = ABSF(quaternion[i]);
        if(abs_quat > abs_quat_max)
        {
            abs_quat_max = abs_quat;
            imu_data->quaternion_omitted_index = i;
        }
    }

    float quat_factor = 1 / quaternion[imu_data->quaternion_omitted_index];

    for(int i = 0, j = 0; i < 4; i++)
    {
        if(i == imu_data->quaternion_omitted_index)
        {
            continue;
        }

        ((uint32_t*)&imu_data->quaternion_1)[(j + imu_data->quaternion_omitted_index) % 3] = ((quaternion[i] * quat_factor) * 0.5f + 0.5f) * 0x1FFFFF;
        j++;
    }
}

static void calculate_accel_vector(PimuGamepadIMUData* imu_data, float* quaternion)
{
    // We calculate the accel vector by 
    // 1. Inversing our quaternion
    // 2. transforming a (0, 0, 1) vector
    // 3. Inverting the x and y components
    // The code below accounts for all these steps, with everything that is not needed optimized out

    float w = quaternion[0];
    float x = quaternion[1];
    float y = quaternion[2];
    float z = quaternion[3];

    float ax = 2 * (x * z - y * w);
    float ay = 2 * (z * y + x * w);
    float az = w * w - x * x - y * y + z * z;

    imu_data->accel_x = ax * 0x7FFF;
    imu_data->accel_y = ay * 0x7FFF;
    imu_data->accel_z = az * 0x7FFF;
}

static int16_t calculate_axis_delta(int axis_index, float* matrix)
{
    float v11 = matrix[((axis_index + 1) % 3) * 3 + (axis_index + 1) % 3];
    float v12 = matrix[((axis_index + 1) % 3) * 3 + (axis_index + 2) % 3];
    float v21 = matrix[((axis_index + 2) % 3) * 3 + (axis_index + 1) % 3];
    float v22 = matrix[((axis_index + 2) % 3) * 3 + (axis_index + 2) % 3];

    float length_1 = v11 * v11 + v12 * v12;
    float length_2 = v21 * v21 + v22 * v22;
    
    float angle_cos;
    int angle_sign;

    if(length_1 > length_2)
    {
        angle_cos = v11 / sqrtf(length_1);
        angle_sign = v12 < 0 ? -1 : 1;
    }
    else
    {
        angle_cos = v22 / sqrtf(length_2);
        angle_sign = v21 < 0 ? 1 : -1;
    }

    if(angle_cos > 1)
    {
        angle_cos = 1;
    }
    else if(angle_cos < -1)
    {
        angle_cos = -1;
    }

    float angle = acos(angle_cos) * angle_sign;
    const float imu_factor = 1.0f / 3.141593f * 180 * 4000;
    return angle * imu_factor;
}

static void calculate_gyro_delta(PimuGamepadIMUData* imu_data, float* old_quaternion, float* new_quaternion)
{
    if(new_quaternion == NULL)
    {
        imu_data->gyro_x = 0;
        imu_data->gyro_y = 0;
        imu_data->gyro_z = 0;
        return;
    }

    float w0 = old_quaternion[0];
    float x0 = -old_quaternion[1];
    float y0 = -old_quaternion[2];
    float z0 = -old_quaternion[3];

    float w1 = new_quaternion[0];
    float x1 = new_quaternion[1];
    float y1 = new_quaternion[2];
    float z1 = new_quaternion[3];

    float w = w0 * w1 - x0 * x1 - y0 * y1 - z0 * z1;
    float x = w0 * x1 + x0 * w1 + y0 * z1 - z0 * y1;
    float y = w0 * y1 - x0 * z1 + y0 * w1 + z0 * x1;
    float z = w0 * z1 + x0 * y1 - y0 * x1 + z0 * w1;

    float rotation_matrix[3 * 3] = {
        w * w + x * x - y * y - z * z,
        2 * (x * y + z * w),
        2 * (z * x - y * w),

        2 * (x * y - z * w),
        w * w - x * x + y * y - z * z,
        2 * (y * z + x * w),

        2 * (z * x + y * w),
        2 * (y * z - x * w),
        w * w - x * x - y * y + z * z,
    };

    uint16_t delta = pimu_gamepad_imu_data_get_ms_delta(imu_data);
    
    imu_data->gyro_x = calculate_axis_delta(0, rotation_matrix) * delta;
    imu_data->gyro_y = calculate_axis_delta(1, rotation_matrix) * delta;
    imu_data->gyro_z = calculate_axis_delta(2, rotation_matrix) * delta;
}

void ppf_imu_update(PimuGamepadIMUData* imu_data, float* old_quaternion, float* new_quaternion)
{
    float* quaternion = new_quaternion == NULL
        ? old_quaternion
        : new_quaternion;

    pack_quaternion(imu_data, quaternion);
    calculate_accel_vector(imu_data, quaternion);
    calculate_gyro_delta(imu_data, old_quaternion, new_quaternion);
}