#ifndef PIMU_GAMEPAD_INPUTS_H_
#define PIMU_GAMEPAD_INPUTS_H_

#ifndef PACKED_STRUCT
#define PACKED_STRUCT struct __attribute__((packed))
#endif

#include <stdint.h>

typedef PACKED_STRUCT
{
    uint16_t x : 12;
    uint16_t y : 12;
} PG12BitVector2;

typedef PACKED_STRUCT
{
    uint8_t y : 1;
        uint8_t x : 1;
        uint8_t b : 1;
        uint8_t a : 1;
        uint8_t sr_right : 1;
        uint8_t sl_right : 1;
        uint8_t r : 1;
        uint8_t zr : 1;

        uint8_t minus : 1;
        uint8_t plus : 1;
        uint8_t stick_right : 1;
        uint8_t stick_left : 1;
        uint8_t home : 1;
        uint8_t capture : 1;
        uint8_t chat : 1;
        uint8_t unknown_15 : 1;

        uint8_t dpad_down : 1;
        uint8_t dpad_up : 1;
        uint8_t dpad_right : 1;
        uint8_t dpad_left : 1;
        uint8_t sr_left : 1;
        uint8_t sl_left : 1;
        uint8_t l : 1;
        uint8_t zl : 1;
        
        uint8_t gr : 1;
        uint8_t gl : 1;
        uint8_t unknown_26 : 1;
        uint8_t unknown_27 : 1;
        uint8_t headset : 1;
        uint8_t unknown_29 : 1;
        uint8_t unknown_30 : 1;
        uint8_t unknown_31 : 1;
} PimuGamepadInputReport5Buttons;

typedef PACKED_STRUCT
{
    uint32_t counter;
    PimuGamepadInputReport5Buttons buttons;
    uint8_t unknown_1[2];
    PG12BitVector2 left_stick;
    PG12BitVector2 right_stick;
    PACKED_STRUCT
    {
        uint16_t position_x;
        uint16_t position_y;
        uint16_t unknown_1;
        uint16_t unknown_2;
    } mouse_data;
    uint8_t unknown_2;
    PACKED_STRUCT
    {
        uint16_t x;
        uint16_t y;
        uint16_t z;
    } magnetometer_data;
    uint16_t battery_voltage;
    uint8_t battery_charging_state;
    uint16_t battery_current;
    uint8_t unknown_3[6];
    PACKED_STRUCT {
        uint32_t timestamp;
        uint16_t temperature;
        int16_t accel_x;
        int16_t accel_y;
        int16_t accel_z;
        int16_t gyro_x;
        int16_t gyro_y;
        int16_t gyro_z;
    } motion_data;
    uint8_t left_analog_trigger;
    uint8_t right_analog_trigger;
    uint8_t reserved;
}
PimuGamepadInputReport5;

#define PG_INPUT_POWER_INFO_EXTERNAL_POWER 0x01
#define PG_INPUT_POWER_INFO_CHARGING 0x02
#define PG_INPUT_POWER_INFO_BATTERY_0 0x04
#define PG_INPUT_POWER_INFO_BATTERY_1 0x08
#define PG_INPUT_POWER_INFO_BATTERY_2 0x10
#define PG_INPUT_POWER_INFO_BATTERY_3 0x20

typedef PACKED_STRUCT
{
    uint8_t b : 1;
    uint8_t a : 1;
    uint8_t y : 1;
    uint8_t x : 1;
    uint8_t r : 1;
    uint8_t zr : 1;
    uint8_t plus : 1;
    uint8_t stick_right : 1;
    
    uint8_t dpad_down : 1;
    uint8_t dpad_right : 1;
    uint8_t dpad_left : 1;
    uint8_t dpad_up : 1;
    uint8_t l : 1;
    uint8_t zl : 1;
    uint8_t minus : 1;
    uint8_t stick_left : 1;
    
    uint8_t home : 1;
    uint8_t capture : 1;
    uint8_t gr : 1;
    uint8_t gl : 1;
    uint8_t chat : 1;
    uint8_t unknown_21 : 1;
    uint8_t unknown_22 : 1;
    uint8_t unknown_23 : 1;
} PimuGamepadInputReport9Buttons;

typedef PACKED_STRUCT
{
    uint8_t counter;
    uint8_t power_info;
    PimuGamepadInputReport9Buttons buttons;
    PG12BitVector2 left_stick;
    PG12BitVector2 right_stick;
    uint8_t unknown;
    uint8_t nfc_state;
    uint8_t headset_flag;
    uint8_t imu_data_len;
    uint8_t imu[40];
    uint8_t reserved[8];
}
PimuGamepadInputReport9;

typedef struct
{
    uint32_t timestamp;
    uint32_t timestamp_delta;
    uint16_t temperature;
    uint8_t mode;
    uint8_t confidence_flags;
    uint8_t quaternion_omitted_index;
    uint32_t quaternion_1;
    uint32_t quaternion_2;
    uint32_t quaternion_3;

    float quaternion_w;
    float quaternion_x;
    float quaternion_y;
    float quaternion_z;

    int32_t accel_x_32;
    int32_t accel_y_32;
    int32_t accel_z_32;

    int16_t accel_x_norm16;
    int16_t accel_y_norm16;
    int16_t accel_z_norm16;

    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;
    int16_t unknown;
} PimuGamepadIMUData;


PG12BitVector2 pimu_gamepad_inputs_pack_vector(float x, float y);
uint16_t pimu_gamepad_imu_data_get_ms_delta(PimuGamepadIMUData* imu_data);

void pimu_gamepad_imu_pack_quaternion(PimuGamepadIMUData* imu_data, float* quaternion);
void pimu_gamepad_imu_set_accel_vectors(PimuGamepadIMUData* imu_data, float x, float y, float z);

void pimu_gamepad_copy_inputs_5_to_9(PimuGamepadInputReport5* report_5, PimuGamepadInputReport9* report_9);
void pimu_gamepad_copy_inputs_9_to_5(PimuGamepadInputReport9* report_9, PimuGamepadInputReport5* report_5);


void pimu_gamepad_pack_5_imu_data(PimuGamepadInputReport5* report, PimuGamepadIMUData* imu_data);
void pimu_gamepad_pack_9_imu_data(PimuGamepadInputReport9* report, PimuGamepadIMUData* imu_data);

#endif