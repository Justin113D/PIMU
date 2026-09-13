#include "command.h"

// Source - 2026-08-14
// https://github.com/ndeadly/switch2_controller_research/blob/master/commands.md#command-0x0c---feature-select

#define PG_FEATURE_SELECT_FEATURE_BUTTONS 0x01
#define PG_FEATURE_SELECT_FEATURE_STICKS 0x02
#define PG_FEATURE_SELECT_FEATURE_IMU 0x04
#define PG_FEATURE_SELECT_FEATURE_MOUSE 0x10
#define PG_FEATURE_SELECT_FEATURE_RUMBLE 0x20
#define PG_FEATURE_SELECT_FEATURE_MAGNETOMETER 0x80

//--------------------------------------------------------------------+
// Get info
//--------------------------------------------------------------------+

typedef PACKED_STRUCT
{
    uint8_t flags;
    uint8_t unknown[3];
} GetInfoRequest;

typedef PACKED_STRUCT
{
    uint8_t unknown[4];
    uint8_t info[8];
} GetInfoResponse;

PG_SUBCOMMAND_CALLBACK_DEF(feature_select, get_info)
{
    PG_SUBCOMMAND_REQUEST_SETUP(GetInfoRequest);
    PG_SUBCOMMAND_RESPONSE_SETUP(GetInfoResponse);

    PG_RESPONSE_INIT(unknown);

    response->info[0] = request->flags & PG_FEATURE_SELECT_FEATURE_BUTTONS ? 0x07 : 0x00;
    response->info[1] = request->flags & PG_FEATURE_SELECT_FEATURE_STICKS ? 0x07 : 0x00;
    response->info[2] = request->flags & PG_FEATURE_SELECT_FEATURE_IMU ? 0x01 : 0x00;
    response->info[3] = request->flags & PG_FEATURE_SELECT_FEATURE_MAGNETOMETER ? 0x01 : 0x00;
    response->info[4] = request->flags & PG_FEATURE_SELECT_FEATURE_MOUSE ? 0x01 : 0x00;
    response->info[5] = request->flags & PG_FEATURE_SELECT_FEATURE_RUMBLE ? 0x03 : 0x00;
    response->info[6] = 0;
    response->info[7] = 0;

    return PG_SUBCOMMAND_RETURN_CODE_OK;
}


//--------------------------------------------------------------------+
// Set mask
//--------------------------------------------------------------------+

typedef PACKED_STRUCT
{
    uint8_t flags;
    uint8_t unknown[3];
} SetMaskRequest;

typedef PACKED_STRUCT
{
    uint8_t unknown[4];
} SetMaskResponse;

PG_SUBCOMMAND_CALLBACK_DEF(feature_select, set_mask)
{
    PG_SUBCOMMAND_REQUEST_SETUP(SetMaskRequest);
    PG_SUBCOMMAND_RESPONSE_SETUP(SetMaskResponse);

    pg_status_set_feature_mask(&data->status, request->flags);

    PG_RESPONSE_INIT(unknown);

    return PG_SUBCOMMAND_RETURN_CODE_OK;
}


//--------------------------------------------------------------------+
// Clear mask
//--------------------------------------------------------------------+

typedef PACKED_STRUCT
{
    uint8_t unknown[4];
} ClearMaskRequest;

typedef PACKED_STRUCT
{
    uint8_t unknown[4];
} ClearMaskResponse;

PG_SUBCOMMAND_CALLBACK_DEF(feature_select, clear_mask)
{
    PG_SUBCOMMAND_REQUEST_SETUP(ClearMaskRequest);
    PG_SUBCOMMAND_RESPONSE_SETUP(ClearMaskResponse);

    pg_status_set_feature_mask(&data->status, 0);

    PG_RESPONSE_INIT(unknown);

    return PG_SUBCOMMAND_RETURN_CODE_OK;
}


//--------------------------------------------------------------------+
// Enable
//--------------------------------------------------------------------+

typedef PACKED_STRUCT
{
    uint8_t flags;
    uint8_t unknown[3];
} EnableRequest;

typedef PACKED_STRUCT
{
    uint8_t unknown[4];
} EnableResponse;

PG_SUBCOMMAND_CALLBACK_DEF(feature_select, enable)
{
    PG_SUBCOMMAND_REQUEST_SETUP(EnableRequest);
    PG_SUBCOMMAND_RESPONSE_SETUP(EnableResponse);

    pg_status_set_feature_enabled(&data->status, pg_status_get_feature_enabled(&data->status) | request->flags);

    PG_RESPONSE_INIT(unknown);

    return PG_SUBCOMMAND_RETURN_CODE_OK;
}


//--------------------------------------------------------------------+
// Disable
//--------------------------------------------------------------------+

typedef PACKED_STRUCT
{
    uint8_t flags;
    uint8_t unknown[3];
} DisableRequest;

typedef PACKED_STRUCT
{
    uint8_t unknown[4];
} DisableResponse;

PG_SUBCOMMAND_CALLBACK_DEF(feature_select, disable)
{
    PG_SUBCOMMAND_REQUEST_SETUP(DisableRequest);
    PG_SUBCOMMAND_RESPONSE_SETUP(DisableResponse);

    pg_status_set_feature_enabled(&data->status, pg_status_get_feature_enabled(&data->status) & ~request->flags);

    PG_RESPONSE_INIT(unknown);

    return PG_SUBCOMMAND_RETURN_CODE_OK;
}


//--------------------------------------------------------------------+
// Configure
//--------------------------------------------------------------------+

typedef PACKED_STRUCT
{
    uint8_t flags;
    uint8_t unknown[3];
} configureRequest;

typedef PACKED_STRUCT
{
    uint8_t unknown[4];
    uint32_t length;
    uint8_t data[32];
} configureResponse;

PG_SUBCOMMAND_CALLBACK_DEF(feature_select, configure)
{
    return PG_SUBCOMMAND_RETURN_CODE_NOT_IMPLEMENTED;
}


//--------------------------------------------------------------------+
// Command setup
//--------------------------------------------------------------------+

PG_SUBCOMMAND_TABLE(feature_select) = {
    PG_SUBCOMMAND_NONE,
    PG_SUBCOMMAND(1, "Get info", feature_select, get_info),
    PG_SUBCOMMAND(2, "Set mask", feature_select, set_mask),
    PG_SUBCOMMAND(3, "Clear mask", feature_select, clear_mask),
    PG_SUBCOMMAND(4, "Enable", feature_select, enable),
    PG_SUBCOMMAND(5, "Disable", feature_select, disable),
    PG_SUBCOMMAND(6, "Configure", feature_select, configure),
};

PG_COMMAND(12, feature_select, "Feature select");