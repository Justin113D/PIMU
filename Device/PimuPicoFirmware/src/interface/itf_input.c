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
static bool previous_update_had_imu;
static PimuGamepadIMUData imu_data;


void ppf_itf_input_init(void)
{
    ppf_gamepad_input_report_5.battery_voltage = 0x0ea5;
    ppf_gamepad_input_report_5.battery_charging_state = 0x20;
    ppf_gamepad_input_report_5.unknown_3[5] = 1;

    ppf_gamepad_input_report_9.power_info = PG_INPUT_POWER_INFO_EXTERNAL_POWER | 0x24; // battery level "9"
    ppf_gamepad_input_report_9.unknown = 0x30;

    imu_data.temperature = 0x100; // default value?
    imu_data.mode = 12;
    imu_data.confidence_flags = 0x3; // always report full confidence
}

void ppf_itf_input_receive(PimuDeviceConnectorGamepadInputs* inputs)
{
    memcpy(&received_inputs, inputs, sizeof(received_inputs));
    has_received_inputs = true;
}

void ppf_itf_input_update(void)
{
    bool imu_active = (pimu_gamepad_get_feature_active(ppf_gamepad) & 0x04) != 0;
    if(!imu_active && imu_was_active)
    {
        memset(&ppf_gamepad_input_report_5.motion_data, 0, sizeof(ppf_gamepad_input_report_5.motion_data));
        memset(&ppf_gamepad_input_report_9.imu, 0, sizeof(ppf_gamepad_input_report_9.imu));
        ppf_gamepad_input_report_9.imu_data_len = 0;
        previous_update_had_imu = false;
    }

    int report_id = pimu_gamepad_get_report_id(ppf_gamepad);

    if(has_received_inputs)
    {
        ppf_gamepad_input_report_9.buttons = *(PimuGamepadInputReport9Buttons*)&received_inputs.buttons;
        ppf_gamepad_input_report_9.left_stick = *(PG12BitVector2*)&received_inputs.stick_left;
        ppf_gamepad_input_report_9.right_stick = *(PG12BitVector2*)&received_inputs.stick_right;

        if(report_id == 5)
        {
            pimu_gamepad_copy_inputs_9_to_5(&ppf_gamepad_input_report_9, &ppf_gamepad_input_report_5);
        }
    }

    absolute_time_t now_timestamp = get_absolute_time();

    if(imu_active)
    {
        int64_t time_delta = absolute_time_diff_us(last_updated_timestamp, now_timestamp);
        imu_timestamp += time_delta;
        
        imu_data.timestamp = imu_timestamp;
        imu_data.timestamp_delta = time_delta;

        if(has_received_inputs)
        {
            ppf_imu_update(
                &imu_data, 
                &received_inputs, 
                previous_update_had_imu && report_id == 5
            );
        }
        
        if(previous_update_had_imu != has_received_inputs)
        {
            imu_data.gyro_x = 0;
            imu_data.gyro_y = 0;
            imu_data.gyro_z = 0;
        }        

        switch(report_id)
        {
            case 5:
                pimu_gamepad_pack_5_imu_data(&ppf_gamepad_input_report_5, &imu_data);
                break;
            case 9:
                pimu_gamepad_pack_9_imu_data(&ppf_gamepad_input_report_9, &imu_data);
                break;
        }

        previous_update_had_imu = has_received_inputs;
    }

    has_received_inputs = false;
    last_updated_timestamp = now_timestamp;
    imu_was_active = imu_active;
}