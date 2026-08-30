#include "itf_input.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "pico/stdlib.h"

#include "itf_imu.h"
#include "itf_gamepad.h"

PimuGamepadInputReport5 ppf_gamepad_input_report_5;
PimuGamepadInputReport9 ppf_gamepad_input_report_9;

static PimuDeviceConnectorGamepadInputs received_inputs;
static bool has_received_inputs;

static absolute_time_t last_updated_timestamp;
static uint32_t imu_timestamp;
static bool imu_was_active;
static float imu_quaternion[4];


void ppf_itf_input_init(void)
{
    ppf_gamepad_input_report_5.battery_voltage = 0x0ea5;
    ppf_gamepad_input_report_5.battery_charging_state = 0x20;
    ppf_gamepad_input_report_5.unknown_3[5] = 1;

    ppf_gamepad_input_report_9.unknown = 0x30;
}

void ppf_itf_input_receive(PimuDeviceConnectorGamepadInputs* inputs)
{
    memcpy(&received_inputs, inputs, sizeof(received_inputs));
    has_received_inputs = true;
}

void ppf_itf_input_update(void)
{
    bool imu_active = (pimu_gamepad_get_feature_active(ppf_gamepad) & 0x04) != 0;
    if(imu_active && !imu_was_active)
    {
        memset(imu_quaternion, 0, sizeof(imu_quaternion));
        imu_quaternion[0] = 1;
        ppf_gamepad_input_report_9.imu_data_len = 40;
    }
    else if(!imu_active && imu_was_active)
    {
        memset(imu_quaternion, 0, sizeof(imu_quaternion));
        memset(&ppf_gamepad_input_report_5.motion_data, 0, sizeof(ppf_gamepad_input_report_5.motion_data));
        memset(&ppf_gamepad_input_report_9.imu, 0, sizeof(ppf_gamepad_input_report_9.imu));
        ppf_gamepad_input_report_9.imu_data_len = 0;
    }

    float* new_imu_quaternion = NULL;

    if(has_received_inputs)
    {
        memcpy(
            &ppf_gamepad_input_report_5.buttons,
            &received_inputs.buttons,
            sizeof(ppf_gamepad_input_report_5.buttons)
        );
    
        ppf_gamepad_input_report_5.left_stick = pimu_gamepad_inputs_pack_vector(
            received_inputs.stick_left_x,
            received_inputs.stick_left_y
        );
    
        ppf_gamepad_input_report_5.right_stick = pimu_gamepad_inputs_pack_vector(
            received_inputs.stick_right_x,
            received_inputs.stick_right_y
        );

        pimu_gamepad_copy_inputs_5_to_9(
            &ppf_gamepad_input_report_5,
            &ppf_gamepad_input_report_9
        );

        new_imu_quaternion = &received_inputs.quaternion_w;

        has_received_inputs = false;
    }

    absolute_time_t now_timestamp = get_absolute_time();

    if(imu_active)
    {
        int64_t time_delta = absolute_time_diff_us(last_updated_timestamp, now_timestamp);
        imu_timestamp += time_delta;
        
        PimuGamepadIMUData imu_data = {0};
        imu_data.timestamp = imu_timestamp;
        imu_data.timestamp_delta = time_delta;
        imu_data.temperature = 0x100; // default value?
        imu_data.mode = 0xE; // running mode, we do not simulate bootup
        imu_data.confidence_flags = 0x3; // always report full confidence

        ppf_imu_update(&imu_data, imu_quaternion, new_imu_quaternion);

        pimu_gamepad_pack_5_imu_data(&ppf_gamepad_input_report_5, &imu_data);
        pimu_gamepad_pack_9_imu_data(&ppf_gamepad_input_report_9, &imu_data);

        if(new_imu_quaternion != NULL)
        {
            memcpy(imu_quaternion, new_imu_quaternion, sizeof(imu_quaternion));
        }
    }

    last_updated_timestamp = now_timestamp;
    imu_was_active = imu_active;
}