#ifndef PD_FIRMWARE_H_
#define PD_FIRMWARE_H_

#include <stdint.h>

#include "common.h"

#define PD_FIRMWARE_TYPE_JOYCON_L 0
#define PD_FIRMWARE_TYPE_JOYCON_R 1
#define PD_FIRMWARE_TYPE_PRO_CONTROLLER 2
#define PD_FIRMWARE_TYPE_GAMECUBE 3

typedef PACKED_STRUCT
{
    struct
    {
        uint8_t major;
        uint8_t minor;
        uint8_t patch;
    } firmware_version;
    uint8_t firmware_type;
    struct
    {
        uint8_t major;
        uint8_t minor;
        uint8_t patch;
    } bluetooth_patch_version;
    uint8_t padding_1;
    struct
    {
        uint8_t major;
        uint8_t minor;
        uint8_t patch;
    } dsp_firmware_ersion;
    uint8_t padding_2;
} PDFirmwareVersion;

extern const PDFirmwareVersion pd_firmware_version;

#endif