#ifndef PIMU_GAMEPAD_H_
#define PIMU_GAMEPAD_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#ifndef PACKED_STRUCT
#define PACKED_STRUCT struct __attribute__((packed))
#endif

typedef PACKED_STRUCT
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} PimuGamepadColor;

typedef PACKED_STRUCT
{
    PimuGamepadColor body_color;
    PimuGamepadColor buttons_color;
    PimuGamepadColor highlight_color;
    PimuGamepadColor grip_color;
} PimuGamepadDeviceColors;

static const PimuGamepadDeviceColors default_device_colors = {
    .body_color = { 0x23, 0x23, 0x23 },
    .buttons_color = { 0xA0, 0xA0, 0xA0 },
    .highlight_color = { 0xE6, 0xE6, 0xE6 },
    .grip_color = { 0x32, 0x32, 0x32 },
};

//--------------------------------------------------------------------+

typedef struct PimuGamepad PimuGamepad;

PimuGamepad* pimu_gamepad_alloc();
void pimu_gamepad_free(PimuGamepad* gamepad);

void pimu_gamepad_set_name(PimuGamepad* gamepad, const char* name);
const char* pimu_gamepad_get_name(PimuGamepad* gamepad);

typedef enum
{
    PG_DEBUG_TYPE_NONE,
    PG_DEBUG_TYPE_INFO,
    PG_DEBUG_TYPE_WARNING,
    PG_DEBUG_TYPE_ERROR
} PG_DEBUG_TYPE;

typedef void (*PGDebugCB)(PimuGamepad* source, PG_DEBUG_TYPE type, const char* format, va_list args);
void pimu_gamepad_set_debug_cb(PimuGamepad* gamepad, PGDebugCB callback);

void pimu_gamepad_set_device_colors(PimuGamepad* gamepad, PimuGamepadDeviceColors* colors);
PimuGamepadDeviceColors* pimu_gamepad_get_device_colors(PimuGamepad* gamepad);

void pimu_gamepad_set_device_address(PimuGamepad* gamepad, uint8_t* address);

uint8_t pimu_gamepad_get_player_leds_mask(PimuGamepad* gamepad);
bool pimu_gamepad_get_player_leds_flashing(PimuGamepad* gamepad);

typedef void (*PGPlayerLEDsChangedCB)(PimuGamepad* source, uint8_t mask, bool flashing);
void pimu_gamepad_set_player_leds_changed_cb(PimuGamepad* connector, PGPlayerLEDsChangedCB callback);

//--------------------------------------------------------------------+

typedef struct
{
    uint8_t stage;
    uint8_t direction;
    uint8_t type;
    uint8_t recipient;
    uint8_t request_id;
} PGControlTransferRequest;

typedef struct 
{
    uint8_t data[64];
    uint8_t data_size;
} PGControlTransferResponse;

typedef enum
{
    PG_CONTROL_TRANSFER_RETURN_CODE_FALSE,
    PG_CONTROL_TRANSFER_RETURN_CODE_TRUE,
    PG_CONTROL_TRANSFER_RETURN_CODE_TRUE_WITH_DATA
} PG_CONTROL_TRANSFER_RETURN_CODE;

PG_CONTROL_TRANSFER_RETURN_CODE pimu_gamepad_control_transfer_request(PimuGamepad* gamepad, PGControlTransferRequest* request, PGControlTransferResponse* response);

//--------------------------------------------------------------------+

#define PG_COMMAND_RESPONSE_MAX_SIZE 128

typedef struct
{
    uint16_t data_size;
    uint8_t* data;
} PGCommandRequest;

typedef struct
{
    uint16_t data_size;
    uint8_t data[PG_COMMAND_RESPONSE_MAX_SIZE];
} PGCommandResponse;

bool pimu_gamepad_command_request(PimuGamepad* gamepad, PGCommandRequest* request, PGCommandResponse* response);

//--------------------------------------------------------------------+

bool pimu_gamepad_get_usb_enabled(PimuGamepad* gamepad);

uint8_t pimu_gamepad_get_player_leds_mask(PimuGamepad* gamepad);
bool pimu_gamepad_get_player_leds_flashing(PimuGamepad* gamepad);

uint8_t pimu_gamepad_get_feature_mask(PimuGamepad* gamepad);
uint8_t pimu_gamepad_get_feature_enabled(PimuGamepad* gamepad);
uint8_t pimu_gamepad_get_feature_active(PimuGamepad* gamepad);

bool pimu_gamepad_get_hid_reports_enabled(PimuGamepad* gamepad);
uint8_t pimu_gamepad_get_report_id(PimuGamepad* gamepad);

#endif