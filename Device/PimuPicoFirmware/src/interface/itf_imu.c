#include "itf_imu.h"

#include <stdlib.h>
#include <math.h>

#include "debug.h"

static void calculate_accel_vector(PimuGamepadIMUData* imu_data, float* quaternion)
{
    // We calculate the accel vector by 
    // 1. Inversing our quaternion
    // 2. transforming a (0, 1, 0) vector
    // 3. Inverting the x and y components
    // The code below accounts for all these steps, with everything that is not needed optimized out

    float w = quaternion[0];
    float x = quaternion[1];
    float y = quaternion[2];
    float z = quaternion[3];

    float ax = 2 * (x * y + z * w);
    float ay = w * w - x * x + y * y - z * z;
    float az = 2 * (y * z - x * w);

    pimu_gamepad_imu_set_accel_vectors(imu_data, ax, ay, az);
}

static void unpack_quaternion(uint8_t omitted_quaternion_index, uint16_t* quaternion, float* destination)
{
    float quat1 = (quaternion[0] / (float)0x7FFF - 1.0f);
    float quat2 = (quaternion[1] / (float)0x7FFF - 1.0f);
    float quat3 = (quaternion[2] / (float)0x7FFF - 1.0f);

    float factor = 1.0f / sqrtf(quat1*quat1 + quat2*quat2 + quat3*quat3 + 1);
    quat1 *= factor;
    quat2 *= factor;
    quat3 *= factor;

    switch (omitted_quaternion_index)
    {
    case 0:
        destination[0] = factor;
        destination[1] = quat1;
        destination[2] = quat2;
        destination[3] = quat3;
        break;
    case 1:
        destination[0] = quat3;
        destination[1] = factor;
        destination[2] = quat1;
        destination[3] = quat2;
        break;
    case 2:
        destination[0] = quat2;
        destination[1] = quat3;
        destination[2] = factor;
        destination[3] = quat1;
        break;
    case 3:
        destination[0] = quat1;
        destination[1] = quat2;
        destination[2] = quat3;
        destination[3] = factor;
        break;
    }
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

static void calculate_gyro_deltas(PimuGamepadIMUData* imu_data, float* new_quaternion)
{
    float w0 = imu_data->quaternion_w;
    float x0 = -imu_data->quaternion_x;
    float y0 = -imu_data->quaternion_y;
    float z0 = -imu_data->quaternion_z;

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

void ppf_imu_update(PimuGamepadIMUData* imu_data, uint8_t omitted_quaternion_index, uint16_t* quaternion, bool calculate_gyro)
{
    imu_data->quaternion_omitted_index = omitted_quaternion_index;
    imu_data->quaternion_1 = quaternion[0] << 16 | ((quaternion[0] & 0x7F) << 1);
    imu_data->quaternion_2 = quaternion[1] << 16 | ((quaternion[1] & 0x7F) << 1);
    imu_data->quaternion_3 = quaternion[2] << 16 | ((quaternion[2] & 0x7F) << 1);

    float new_quaternion[4];
    unpack_quaternion(omitted_quaternion_index, quaternion, new_quaternion);

    calculate_accel_vector(imu_data, new_quaternion);

    if(calculate_gyro)
    {
        calculate_gyro_deltas(imu_data, new_quaternion);
    }

    imu_data->quaternion_w = new_quaternion[0];
    imu_data->quaternion_x = new_quaternion[1];
    imu_data->quaternion_y = new_quaternion[2];
    imu_data->quaternion_z = new_quaternion[3];
}