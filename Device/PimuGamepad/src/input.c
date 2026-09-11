#include "pimu_gamepad_input.h"

PG12BitVector2 pimu_gamepad_inputs_pack_vector(float x, float y)
{
    if(x < -1)
    {
        x = -1;
    }
    else if(x > 1)
    {
        x = 1;
    }

    if(y < -1)
    {
        y = -1;
    }
    else if(y > 1)
    {
        y = 1;
    }

    PG12BitVector2 result = {
        (uint16_t)((x * 0.5 + 0.5) * 0xFFF),
        (uint16_t)((y * 0.5 + 0.5) * 0xFFF),
    };

    return result;
}

void pimu_gamepad_copy_inputs_5_to_9(PimuGamepadInputReport5* report_5, PimuGamepadInputReport9* report_9)
{
    report_9->buttons.y = report_5->buttons.y;
    report_9->buttons.x = report_5->buttons.x;
    report_9->buttons.b = report_5->buttons.b;
    report_9->buttons.a = report_5->buttons.a;
    report_9->buttons.r = report_5->buttons.r;
    report_9->buttons.zr = report_5->buttons.zr;
    report_9->buttons.minus = report_5->buttons.minus;
    report_9->buttons.plus = report_5->buttons.plus;
    report_9->buttons.stick_right = report_5->buttons.stick_right;
    report_9->buttons.stick_left = report_5->buttons.stick_left;
    report_9->buttons.home = report_5->buttons.home;
    report_9->buttons.capture = report_5->buttons.capture;
    report_9->buttons.chat = report_5->buttons.chat;
    report_9->buttons.dpad_down = report_5->buttons.dpad_down;
    report_9->buttons.dpad_up = report_5->buttons.dpad_up;
    report_9->buttons.dpad_right = report_5->buttons.dpad_right;
    report_9->buttons.dpad_left = report_5->buttons.dpad_left;
    report_9->buttons.l = report_5->buttons.l;
    report_9->buttons.zl = report_5->buttons.zl;
    report_9->buttons.gr = report_5->buttons.gr;
    report_9->buttons.gl = report_5->buttons.gl;

    report_9->left_stick = report_5->left_stick;   
    report_9->right_stick = report_5->right_stick;
}

void pimu_gamepad_copy_inputs_9_to_5(PimuGamepadInputReport9* report_9, PimuGamepadInputReport5* report_5)
{
    report_5->buttons.y = report_9->buttons.y;
    report_5->buttons.x = report_9->buttons.x;
    report_5->buttons.b = report_9->buttons.b;
    report_5->buttons.a = report_9->buttons.a;
    report_5->buttons.r = report_9->buttons.r;
    report_5->buttons.zr = report_9->buttons.zr;
    report_5->buttons.minus = report_9->buttons.minus;
    report_5->buttons.plus = report_9->buttons.plus;
    report_5->buttons.stick_right = report_9->buttons.stick_right;
    report_5->buttons.stick_left = report_9->buttons.stick_left;
    report_5->buttons.home = report_9->buttons.home;
    report_5->buttons.capture = report_9->buttons.capture;
    report_5->buttons.chat = report_9->buttons.chat;
    report_5->buttons.dpad_down = report_9->buttons.dpad_down;
    report_5->buttons.dpad_up = report_9->buttons.dpad_up;
    report_5->buttons.dpad_right = report_9->buttons.dpad_right;
    report_5->buttons.dpad_left = report_9->buttons.dpad_left;
    report_5->buttons.l = report_9->buttons.l;
    report_5->buttons.zl = report_9->buttons.zl;
    report_5->buttons.gr = report_9->buttons.gr;
    report_5->buttons.gl = report_9->buttons.gl;

    report_5->left_stick = report_9->left_stick;   
    report_5->right_stick = report_9->right_stick;
}

uint16_t pimu_gamepad_imu_data_get_ms_delta(PimuGamepadIMUData* imu_data)
{
    uint16_t timestamp_prev = (uint16_t)((imu_data->timestamp - imu_data->timestamp_delta) / 1000);
    uint16_t timestamp_now = (uint16_t)(imu_data->timestamp / 1000);

    if(timestamp_prev > timestamp_now)
    {
        return timestamp_prev - timestamp_now;
    }
    else
    {
        return timestamp_now - timestamp_prev;
    }
}


#define ABSF(_v) ((_v) < 0 ? -(_v) : (_v))

void pimu_gamepad_imu_pack_quaternion(PimuGamepadIMUData* imu_data, float* quaternion)
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
    uint32_t* destination = ((uint32_t*)&imu_data->quaternion_1);

    for(int i = 0; i < 4; i++)
    {
        if(i == imu_data->quaternion_omitted_index)
        {
            continue;
        }

        uint32_t value = ((quaternion[i] * quat_factor) * 0.5f + 0.5f) * 0xFFFFFFFFu;
        destination[(i + 3 - imu_data->quaternion_omitted_index) % 4] = value;
    }
}

#undef ABSF

void pimu_gamepad_imu_set_accel_vectors(PimuGamepadIMUData* imu_data, float x, float y, float z)
{
    imu_data->accel_x_32 = x * 0x10000000;
    imu_data->accel_y_32 = y * 0x10000000;
    imu_data->accel_z_32 = z * 0x10000000;

    #define CLAMP1(_v) (_v < -1.0f ? -1.0f : _v > 1.0f ? 1.0f : _v)

    imu_data->accel_x_norm16 = CLAMP1(x) * 0x7FFF;
    imu_data->accel_y_norm16 = CLAMP1(y) * 0x7FFF;
    imu_data->accel_z_norm16 = CLAMP1(z) * 0x7FFF;
}

void pimu_gamepad_pack_5_imu_data(PimuGamepadInputReport5* report, PimuGamepadIMUData* imu_data)
{
    report->motion_data.timestamp = imu_data->timestamp;
    report->motion_data.temperature = imu_data->temperature;
    report->motion_data.accel_x = imu_data->accel_x_norm16;
    report->motion_data.accel_y = imu_data->accel_y_norm16;
    report->motion_data.accel_z = imu_data->accel_z_norm16;
    report->motion_data.gyro_x = imu_data->gyro_x;
    report->motion_data.gyro_y = imu_data->gyro_y;
    report->motion_data.gyro_z = imu_data->gyro_z;
}

#pragma GCC diagnostic ignored "-Wshift-count-overflow"
#pragma GCC diagnostic push

#define BITS_MASK(_bits) (0xFF - ((0xFF << (_bits)) & 0xFF))
#define SHIFT(_value, _shift) ((_shift) < 0 ? (_value) << -(_shift) : (_value) >> (_shift))

#define SINGLE(_value, _size, _offset) (SHIFT(imu_data->_value, sizeof(imu_data->_value) * 8 - (_size) + (_offset)) & 0xFF)

#define DOUBLE(_lower, _lower_size, _lower_offset, _upper, _upper_size) \
    (  (SHIFT(imu_data->_lower, sizeof(imu_data->_lower) * 8 - (_lower_size) + (_lower_offset)                ) &         BITS_MASK(_lower_size - _lower_offset)) \
     | (SHIFT(imu_data->_upper, sizeof(imu_data->_upper) * 8 - (_upper_size) + (_lower_offset) - (_lower_size)) & (0xFF - BITS_MASK(_lower_size - _lower_offset))))

void pack_imu_state_12(uint8_t* dst, PimuGamepadIMUData* imu_data)
{
    // Layout:
    // omitted index (2 bits)
    // 6 bits gap
    // 26 bit quat 1
    // 5 bit gap
    // 26 bit quat 1
    // 5 bit gap
    // 26 bit quat 1
    // 32 bit accel x
    // 32 bit accel y
    // 32 bit accel z
    // 16 bit unknown

    dst[0] = imu_data->quaternion_omitted_index & 0x3;
    dst[1]  = SINGLE(quaternion_1, 26, 0);
    dst[2]  = SINGLE(quaternion_1, 26, 8);
    dst[3]  = SINGLE(quaternion_1, 26, 16);

    dst[4] = (imu_data->quaternion_1 >> 30) | ((imu_data->quaternion_2 << 1) & 0x80);
    dst[5]  = SINGLE(quaternion_2, 26, 1);
    dst[6]  = SINGLE(quaternion_2, 26, 9);
    dst[7]  = SINGLE(quaternion_2, 26, 17);

    dst[8] = (imu_data->quaternion_2 >> 31) | (imu_data->quaternion_2 & 0xC0);
    dst[9]  = SINGLE(quaternion_3, 26, 2);
    dst[10]  = SINGLE(quaternion_3, 26, 10);
    dst[11]  = SINGLE(quaternion_3, 26, 18);

    *((int32_t*)&dst[12]) = imu_data->accel_x_32; 
    *((int32_t*)&dst[16]) = imu_data->accel_y_32;
    *((int32_t*)&dst[20]) = imu_data->accel_z_32;
    *((int16_t*)&dst[24]) = imu_data->unknown;
}

void pack_imu_state_14(uint8_t* dst, PimuGamepadIMUData* imu_data)
{
    dst[0]  = (SINGLE(quaternion_1, 21, -5) & 0xE0) | (imu_data->confidence_flags & 0x3) | ((imu_data->quaternion_omitted_index & 0x3) << 2);
    dst[1]  = SINGLE(quaternion_1, 21, 3);
    dst[2]  = SINGLE(quaternion_1, 21, 11);
    dst[3]  = DOUBLE(quaternion_1, 21, 19, quaternion_2, 21);

    dst[4]  = SINGLE(quaternion_2, 21, 6);
    dst[5]  = DOUBLE(quaternion_2, 21, 14, quaternion_3, 21);
    dst[6] = SINGLE(quaternion_3, 21, 1);
    dst[7] = SINGLE(quaternion_3, 21, 9);

    dst[8] = DOUBLE(quaternion_3, 21, 17, accel_x_norm16, 14);
    dst[9] = SINGLE(accel_x_norm16, 14, 4);
    dst[10] = DOUBLE(accel_x_norm16, 14, 12, accel_y_norm16, 14);
    dst[11] = SINGLE(accel_y_norm16, 14, 6);

    dst[12] = SINGLE(accel_z_norm16, 14, 0);
    dst[13] = DOUBLE(accel_z_norm16, 14, 8, gyro_x, 13);
    dst[14] = SINGLE(gyro_x, 13, 2);
    dst[15] = DOUBLE(gyro_x, 13, 10, gyro_y, 13);

    dst[16] = SINGLE(gyro_y, 13, 5);
    dst[17] = SINGLE(gyro_z, 13, 0);
    dst[18] = DOUBLE(gyro_z, 13, 8, accel_x_norm16, 13);
    dst[19] = SINGLE(accel_x_norm16, 13, 3);
    
    dst[20] = DOUBLE(accel_x_norm16, 13, 11, accel_y_norm16, 13);
    dst[21] = DOUBLE(accel_y_norm16, 13, 6, accel_z_norm16, 13);
    dst[22] = SINGLE(accel_z_norm16, 13, 1);
    dst[27] = DOUBLE(accel_z_norm16, 13, 9, gyro_x, 14);

    dst[24] = SINGLE(gyro_x, 14, 4);
    dst[25] = DOUBLE(gyro_x, 14, 12, gyro_y, 13);
    dst[26] = SINGLE(gyro_y, 14, 6);
    dst[27] = SINGLE(gyro_z, 14, 0);

    dst[28] = DOUBLE(gyro_z, 14, 8, accel_x_norm16, 14);
    dst[29] = SINGLE(accel_x_norm16, 14, 2);
    dst[30] = DOUBLE(accel_x_norm16, 14, 10, accel_y_norm16, 14);
    dst[31] = SINGLE(accel_y_norm16, 14, 4);
   
    dst[32] = DOUBLE(accel_y_norm16, 14, 12, accel_z_norm16, 14);
    dst[33] = SINGLE(accel_z_norm16, 14, 6);
    dst[34] = imu_data->unknown & 0xFF;
    dst[35] = (imu_data->unknown >> 8) & 0xFF;
}

#undef BITS_MASK
#undef SHIFT
#undef SINGLE
#undef DOUBLE

#pragma GCC diagnostic pop

void pimu_gamepad_pack_9_imu_data(PimuGamepadInputReport9* report, PimuGamepadIMUData* imu_data)
{
    uint8_t* imu = report->imu;

    uint16_t sample_number = (uint16_t)(imu_data->timestamp / 1250);
    uint16_t sample_number_prev = (uint16_t)((imu_data->timestamp - imu_data->timestamp_delta) / 1250);

    uint16_t sample_number_delta = sample_number_prev > sample_number
        ? 0xFFFF - (sample_number_prev - sample_number)
        : sample_number - sample_number_prev;
    
    imu[0] = sample_number & 0xFF;
    imu[1] = ((sample_number >> 8) & 0xF) | ((sample_number_delta << 4) & 0xF0);
    imu[2] = (sample_number_delta >> 4) & 0xFF;
    imu[3] = imu_data->mode;

    switch(imu_data->mode)
    {
        case 12:
            report->imu_data_len = 30;
            pack_imu_state_12(&imu[4], imu_data);
            break;
        case 14:
            report->imu_data_len = 40;
            pack_imu_state_14(&imu[4], imu_data);
            break;
    }
}