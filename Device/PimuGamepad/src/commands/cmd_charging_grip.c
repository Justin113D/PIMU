#include "command.h"

// Source - 2026-08-14
// https://github.com/ndeadly/switch2_controller_research/blob/master/commands.md#command-0x08---charging-grip

//--------------------------------------------------------------------+
// Get info 1
//--------------------------------------------------------------------+

typedef PACKED_STRUCT
{
    uint8_t size;
    uint8_t unknown[3];
} GetInfo1Request;

typedef PACKED_STRUCT
{
    uint8_t unknown[4];
    uint8_t data[32];
} GetInfo1Response;

PG_SUBCOMMAND_CALLBACK_DEF(charging_grip, get_info_1)
{
    return PG_SUBCOMMAND_RETURN_CODE_NOT_IMPLEMENTED;
}


//--------------------------------------------------------------------+
// Enable buttons
//--------------------------------------------------------------------+

typedef PACKED_STRUCT
{
    uint8_t enable;
    uint8_t unknown[3];
} EnableButtonsRequest;

PG_SUBCOMMAND_CALLBACK_DEF(charging_grip, enable_buttons)
{
    return PG_SUBCOMMAND_RETURN_CODE_NOT_IMPLEMENTED;
}


//--------------------------------------------------------------------+
// Get info 2
//--------------------------------------------------------------------+

typedef PACKED_STRUCT
{
    uint8_t size;
    uint8_t unknown[3];
} GetInfo2Request;

typedef PACKED_STRUCT
{
    uint8_t unknown[4];
    uint8_t data[64];
} GetInfo2Response;

PG_SUBCOMMAND_CALLBACK_DEF(charging_grip, get_info_2)
{
    return PG_SUBCOMMAND_RETURN_CODE_NOT_IMPLEMENTED;
}


//--------------------------------------------------------------------+
// Command setup
//--------------------------------------------------------------------+

PG_SUBCOMMAND_TABLE(charging_grip) = {
    PG_SUBCOMMAND_NONE,
    PG_SUBCOMMAND(1, "Get info 1", charging_grip, get_info_1),
    PG_SUBCOMMAND(2, "Enable buttons", charging_grip, enable_buttons),
    PG_SUBCOMMAND(3, "Get info 2", charging_grip, get_info_2),
};

PG_COMMAND(8, charging_grip, "Charging grip");
