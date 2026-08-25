#include "flash.h"

#include <string.h>

#include "pimu_gamepad_input.h"
#include "common.h"

typedef PACKED_STRUCT 
{
    uint32_t struct_offset;
    int32_t struct_length;
    uint32_t flash_offset;
} PGFlashOffsetMapping;

#define FIELD_OFFSET(_type, _member) ((char *)&(*(_type *)0)._member - (char *)&(*(_type *)0))
#define FIELD_SIZE(_type, _member) sizeof((*(_type *)0)._member)

#define OFFSET_MAPPING(_struct, _member, _offset) {\
    .struct_offset = FIELD_OFFSET(_struct, _member),\
    .struct_length = FIELD_SIZE(_struct, _member),\
    .flash_offset = _offset\
}

#define OFFSET_MAPPING_RANGE(_struct, _member_start, _member_end, _offset) {\
    .struct_offset = FIELD_OFFSET(_struct, _member_start),\
    .struct_length = FIELD_OFFSET(_struct, _member_end) - FIELD_OFFSET(_struct, _member_start),\
    .flash_offset = _offset\
}

//--------------------------------------------------------------------+
// Factory data 1
//--------------------------------------------------------------------+

typedef PACKED_STRUCT
{
    uint8_t unknown_0;
    PG12BitVector2 vectors[12];
    uint8_t unknown_1;
    uint8_t unknown_2;
    uint8_t unknown_3;
} PGFlashJoystickParameters;

typedef PACKED_STRUCT
{
    PG12BitVector2 center;
    PG12BitVector2 min;
    PG12BitVector2 max;
} PGFlashJoystickCalibration;

typedef PACKED_STRUCT
{
    float x;
    float y;
    float z;
} PGFlashBias;

typedef PACKED_STRUCT
{
    uint16_t unknown_1;
    char serial_number[16];
    uint16_t vendor_id;
    uint16_t product_id;
    uint8_t unknown_2[3];
    PimuGamepadDeviceColors device_colors;
    float motion_calibration_temperature;
    PGFlashBias gyro_bias;
    PGFlashJoystickParameters joystick_1_parameters;
    PGFlashJoystickCalibration joystick_1_calibration;
    PGFlashJoystickParameters joystick_2_parameters;
    PGFlashJoystickCalibration joystick_2_calibration;
    PGFlashBias magnetometer_bias;
    PGFlashBias accelerometer_bias;
    uint8_t end[];
} PGFlashFactoryData1;


static const PGFlashOffsetMapping factory_data_1_flash_offset_map[] = {
    OFFSET_MAPPING_RANGE(PGFlashFactoryData1, unknown_1, motion_calibration_temperature, 0),
    OFFSET_MAPPING_RANGE(PGFlashFactoryData1, motion_calibration_temperature, joystick_1_parameters, 0x40),
    OFFSET_MAPPING_RANGE(PGFlashFactoryData1, joystick_1_parameters, joystick_2_parameters, 0x80),
    OFFSET_MAPPING_RANGE(PGFlashFactoryData1, joystick_2_parameters, magnetometer_bias, 0xC0),
    OFFSET_MAPPING_RANGE(PGFlashFactoryData1, magnetometer_bias, end, 0x100),
};  

static const uint32_t factory_data_1_flash_offset = 0x13000;

//--------------------------------------------------------------------+
// Factory data 2
//--------------------------------------------------------------------+

typedef PACKED_STRUCT
{
    char serial[32];
    uint8_t unknown_1[4];
    uint8_t unknown_2[10];
    uint8_t unknown_3[2];
    uint8_t unknown_4[9];
    uint8_t unknown_5[4];
    uint8_t end[];
} PGFlashFactoryData2;

static const PGFlashOffsetMapping factory_data_2_flash_offset_map[] = {
    OFFSET_MAPPING_RANGE(PGFlashFactoryData2, serial, unknown_1, 0),
    OFFSET_MAPPING(PGFlashFactoryData2, unknown_1, 0x30),
    OFFSET_MAPPING(PGFlashFactoryData2, unknown_2, 0x60),
    OFFSET_MAPPING(PGFlashFactoryData2, unknown_3, 0x80),
    OFFSET_MAPPING(PGFlashFactoryData2, unknown_4, 0xFB),
};  

static const uint32_t factory_data_2_flash_offset = 0x13E00;

//--------------------------------------------------------------------+
// Default factory data
//--------------------------------------------------------------------+

static const PGFlashJoystickParameters default_joystick_parameters = {
    .unknown_0 = 1,
    .vectors = {
        { 0x9AD, 0x9AD },
        { 0x655, 0x655 },
        { 0x0A0, 0x0A0 }, { 0x0A0, 0x0A0 },
        { 0x0E2, 0x0E2 }, { 0x0E2, 0x0E2 },
        { 0xD9A, 0xD9A }, { 0xD9A, 0xD9A },
        { 0x50A, 0x50A }, { 0x50A, 0x50A },
        { 0x62F, 0x62F }, { 0x62F, 0x62F },
    } ,
    .unknown_1 = 10,
    .unknown_2 = 0xFF,
    .unknown_3 = 0xFF
};

static const PGFlashJoystickCalibration default_jostick_calibration = {
    .center = { 0x7FF, 0x7FF },
    .min = { 0x7FF, 0x7FF },
    .max = { 0x7FF, 0x7FF },
};

static const PGFlashFactoryData1 default_factory_data_1 = {
    .unknown_1 = 1,
    .serial_number = "HEJ71001000000",
    .vendor_id = 0x57E,
    .product_id = 0x2069,
    .unknown_2 = { 1, 6, 1 },
    .device_colors = default_device_colors,
    .motion_calibration_temperature = 25.0f,
    .gyro_bias = { 0 },
    .joystick_1_parameters = default_joystick_parameters,
    .joystick_1_calibration = default_jostick_calibration,
    .joystick_1_parameters = default_joystick_parameters,
    .joystick_2_calibration = default_jostick_calibration,
    .magnetometer_bias = { 0 },
    .accelerometer_bias = { 0, 0, 9.81f }
};

static const PGFlashFactoryData2 default_factory_data_2 = {
    .serial = "10000000LD1000000",
    .unknown_1 = { 0x01, 0x02, 0x02, 0x02 },
    .unknown_2 = { 0x03, 0x02, 0x04, 0x01, 0x05, 0x02, 0x06, 0x02,0x07, 0x02 },
    .unknown_3 = { 0x11, 0x00 },
    .unknown_4 = { 0x00, 0x23, 0xE9, 0xCE, 0x00, 0x41, 0x30, 0x4B, 0x41 },
    .unknown_5 = { 0x01, 0x00, 0x15, 0x0C },
};

//--------------------------------------------------------------------+
// Simulating flash reading
//--------------------------------------------------------------------+

static void map_flash_data(
    uint8_t* output, 
    uint8_t output_length, 
    uint32_t read_address, 
    uint8_t* data, 
    const PGFlashOffsetMapping* mapping, 
    uint32_t mapping_count, 
    uint32_t mapping_flash_offset)
{
    uint32_t flash_data_length = mapping[mapping_count - 1].flash_offset + mapping[mapping_count - 1].struct_length;
    if(read_address < mapping_flash_offset
        || read_address >= mapping_flash_offset + flash_data_length)
    {
        return;
    }
    
    for(int i = 0; i < mapping_count; i++)
    {
        const PGFlashOffsetMapping* map = &mapping[i];
        int32_t dst_offset = mapping_flash_offset + map->flash_offset - read_address;
        
        if((dst_offset + map->struct_length) <= 0)
        {
            continue;
        }
        
        int8_t len = MIN( 
            map->struct_length + MIN(dst_offset, 0), 
            output_length - MAX(dst_offset, 0)
        );
        
        if(len <= 0)
        {
            return;
        }
        
        int32_t src_offset = map->struct_offset - MIN(dst_offset, 0);
        dst_offset = MAX(dst_offset, 0);
        
        memcpy(
            &output[dst_offset],
            &data[src_offset],
            len
        );
    }
}

void pg_flash_simulate_read(PGGamepadData* data, uint8_t* output, uint8_t output_length, uint32_t read_address)
{
    memset(output, 0xFF, output_length);

    PGFlashFactoryData1 factory_data_1 = default_factory_data_1;
    factory_data_1.device_colors = data->device_colors;

    map_flash_data(
        output, 
        output_length, 
        read_address, 
        (uint8_t*)&factory_data_1, 
        factory_data_1_flash_offset_map, 
        sizeof(factory_data_1_flash_offset_map) / sizeof(PGFlashOffsetMapping),
        factory_data_1_flash_offset
    );

    map_flash_data(
        output, 
        output_length, 
        read_address, 
        (uint8_t*)&default_factory_data_2, 
        factory_data_2_flash_offset_map, 
        sizeof(factory_data_2_flash_offset_map) / sizeof(PGFlashOffsetMapping),
        factory_data_2_flash_offset
    );

}