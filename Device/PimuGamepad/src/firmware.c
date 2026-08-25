#include "firmware.h"

const PDFirmwareVersion pd_firmware_version = {
    .firmware_version = { 
        .major = 2, 
        .minor = 1, 
        .patch = 4 
    },
    .firmware_type = PD_FIRMWARE_TYPE_PRO_CONTROLLER,
    
    .bluetooth_patch_version = {
        0,
        0,
        12
    },
    .padding_1 = 0,
    
    .dsp_firmware_ersion = { 
        .major = 0, 
        .minor = 2, 
        .patch = 3 
    },
    .padding_2 = 0
};