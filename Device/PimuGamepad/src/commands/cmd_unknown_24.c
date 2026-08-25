#include "command.h"

#include <string.h>

// Source - 2026-08-14
// https://github.com/ndeadly/switch2_controller_research/blob/master/commands.md#command-0x18---unknown


//--------------------------------------------------------------------+
// Unknown 1
//--------------------------------------------------------------------+

typedef PACKED_STRUCT
{
    uint8_t unknown[8];
} Unknown1Response;

PG_SUBCOMMAND_CALLBACK_DEF(unknown_24, unknown_1)
{
    PG_SUBCOMMAND_RESPONSE_SETUP(Unknown1Response);

    static const uint8_t defaultResponse[] = {
        0x00, 0x00, 0x40, 0xf0, 
        0x00, 0x00, 0x60, 0x00
    };

    memcpy(response->unknown, defaultResponse, sizeof(response->unknown));

    return PG_SUBCOMMAND_RETURN_CODE_OK;
}


//--------------------------------------------------------------------+
// Unknown 3
//--------------------------------------------------------------------+

typedef PACKED_STRUCT
{
    uint8_t unknown;
} Unknown3Request;

typedef PACKED_STRUCT
{
    uint8_t unknown;
} Unknown3Response;

PG_SUBCOMMAND_CALLBACK_DEF(unknown_24, unknown_3)
{
    PG_SUBCOMMAND_REQUEST_SETUP(Unknown3Request);
    PG_SUBCOMMAND_RESPONSE_SETUP(Unknown3Response);

    response->unknown = request->unknown;
    
    return PG_SUBCOMMAND_RETURN_CODE_OK;
}


//--------------------------------------------------------------------+
// Command setup
//--------------------------------------------------------------------+

PG_SUBCOMMAND_TABLE(unknown_24) = {
    PG_SUBCOMMAND_NONE,
    PG_SUBCOMMAND_UNKNOWN_WCB(1, unknown_24, unknown_1),
    PG_SUBCOMMAND_UNKNOWN(2),
    PG_SUBCOMMAND_UNKNOWN_WCB(3, unknown_24, unknown_3),
    PG_SUBCOMMAND_UNKNOWN(4),
};

PG_COMMAND(24, unknown_24, "Unknown 24");