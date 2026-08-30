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

void pimu_gamepad_pack_5_imu_data(PimuGamepadInputReport5* report, PimuGamepadIMUData* imu_data)
{
    report->motion_data.timestamp = imu_data->timestamp;
    report->motion_data.temperature = imu_data->temperature;
    report->motion_data.accel_x = imu_data->accel_x;
    report->motion_data.accel_y = imu_data->accel_y;
    report->motion_data.accel_z = imu_data->accel_z;
    report->motion_data.gyro_x = imu_data->gyro_x;
    report->motion_data.gyro_y = imu_data->gyro_y;
    report->motion_data.gyro_z = imu_data->gyro_z;
}

void pimu_gamepad_pack_9_imu_data(PimuGamepadInputReport9* report, PimuGamepadIMUData* imu_data)
{
    report->imu_data_len = 40;
    uint8_t* imu = report->imu;

    uint16_t milliseconds = imu_data->timestamp / 1000;
    uint16_t milliseconds_delta = pimu_gamepad_imu_data_get_ms_delta(imu_data);

    // i will just let the compiler take over here

    #pragma GCC diagnostic ignored "-Wshift-count-overflow"
    #pragma GCC diagnostic push

    #define BITS_MASK(_bits) (0xFF - ((0xFF << (_bits)) & 0xFF))
    #define SHIFT(_value, _shift) ((_shift) < 0 ? (_value) << -(_shift) : (_value) >> (_shift))

    #define SINGLE(_value, _size, _offset) (SHIFT(imu_data->_value, sizeof(imu_data->_value) * 8 - (_size) + (_offset)) & 0xFF)

    #define DOUBLE(_lower, _lower_size, _lower_offset, _upper, _upper_size) \
        (  (SHIFT(imu_data->_lower, sizeof(imu_data->_lower) * 8 - (_lower_size) + (_lower_offset)                ) &         BITS_MASK(_lower_size - _lower_offset)) \
         | (SHIFT(imu_data->_upper, sizeof(imu_data->_upper) * 8 - (_upper_size) + (_lower_offset) - (_lower_size)) & (0xFF - BITS_MASK(_lower_size - _lower_offset))))


    imu[0] = milliseconds & 0xFF;
    imu[1] = ((milliseconds >> 8) & 0xF) | ((milliseconds_delta << 4) & 0xF0);
    imu[2] = (milliseconds_delta >> 4) & 0xFF;
    imu[3] = imu_data->mode;

    imu[4]  = (SINGLE(quaternion_1, 21, -5) & 0xE0) | (imu_data->confidence_flags & 0x3) | ((imu_data->quaternion_omitted_index & 0x3) << 2);
    imu[5]  = SINGLE(quaternion_1, 21, 3);
    imu[6]  = SINGLE(quaternion_1, 21, 11);
    imu[7]  = DOUBLE(quaternion_1, 21, 19, quaternion_2, 21);

    imu[8]  = SINGLE(quaternion_2, 21, 6);
    imu[9]  = DOUBLE(quaternion_2, 21, 14, quaternion_3, 21);
    imu[10] = SINGLE(quaternion_3, 21, 1);
    imu[11] = SINGLE(quaternion_3, 21, 9);

    imu[12] = DOUBLE(quaternion_3, 21, 17, accel_x, 14);
    imu[13] = SINGLE(accel_x, 14, 4);
    imu[14] = DOUBLE(accel_x, 14, 12, accel_y, 14);
    imu[15] = SINGLE(accel_y, 14, 6);

    imu[16] = SINGLE(accel_z, 14, 0);
    imu[17] = DOUBLE(accel_z, 14, 8, gyro_x, 13);
    imu[18] = SINGLE(gyro_x, 13, 2);
    imu[19] = DOUBLE(gyro_x, 13, 10, gyro_y, 13);

    imu[20] = SINGLE(gyro_y, 13, 5);
    imu[21] = SINGLE(gyro_z, 13, 0);
    imu[22] = DOUBLE(gyro_z, 13, 8, accel_x, 13);
    imu[23] = SINGLE(accel_x, 13, 3);
    
    imu[24] = DOUBLE(accel_x, 13, 11, accel_y, 13);
    imu[25] = DOUBLE(accel_y, 13, 6, accel_z, 13);
    imu[26] = SINGLE(accel_z, 13, 1);
    imu[27] = DOUBLE(accel_z, 13, 9, gyro_x, 14);

    imu[28] = SINGLE(gyro_x, 14, 4);
    imu[29] = DOUBLE(gyro_x, 14, 12, gyro_y, 13);
    imu[30] = SINGLE(gyro_y, 14, 6);
    imu[31] = SINGLE(gyro_z, 14, 0);

    imu[32] = DOUBLE(gyro_z, 14, 8, accel_x, 14);
    imu[33] = SINGLE(accel_x, 14, 2);
    imu[34] = DOUBLE(accel_x, 14, 10, accel_y, 14);
    imu[35] = SINGLE(accel_y, 14, 4);
   
    imu[36] = DOUBLE(accel_y, 14, 12, accel_z, 14);
    imu[37] = SINGLE(accel_z, 14, 6);
    imu[38] = imu_data->unknown & 0xFF;
    imu[39] = (imu_data->unknown >> 8) & 0xFF;

    #undef BITS_MASK
    #undef SHIFT
    #undef SINGLE
    #undef DOUBLE

    #pragma GCC diagnostic pop
    
}