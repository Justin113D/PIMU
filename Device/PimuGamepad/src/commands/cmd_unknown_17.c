#include "command.h"

#include <string.h>

// Source - 2026-08-14
// https://github.com/ndeadly/switch2_controller_research/blob/master/commands.md#command-0x11---unknown


//--------------------------------------------------------------------+
// Unknown 1
//--------------------------------------------------------------------+

typedef PACKED_STRUCT
{
    uint8_t unknown[4];
} Unknown1Response;

PG_SUBCOMMAND_CALLBACK_DEF(unknown_17, unknown_1)
{
    PG_SUBCOMMAND_RESPONSE_SETUP(Unknown1Response);

    PG_RESPONSE_INIT(unknown);
    response->unknown[0] = 1;
    
    return PG_SUBCOMMAND_RETURN_CODE_OK;
}


//--------------------------------------------------------------------+
// Unknown 3
//--------------------------------------------------------------------+

typedef PACKED_STRUCT
{
    uint8_t unknown[29];
} Unknown3Response;

PG_SUBCOMMAND_CALLBACK_DEF(unknown_17, unknown_3)
{
    PG_SUBCOMMAND_RESPONSE_SETUP(Unknown3Response);

    static const uint8_t defaultResponse[] = {
        0x01, 0x20, 0x03, 0x00, 
        0x00, 0x0a, 0xe8, 0x1c, 
        0x3b, 0x79, 0x7d, 0x8b, 
        0x3a, 0x0a, 0xe8, 0x9c, 
        0x42, 0x58, 0xa0, 0x0b, 
        0x42, 0x0a, 0xe8, 0x9c, 
        0x41, 0x58, 0xa0, 0x0b, 
        0x41, 
    };

    memcpy(response->unknown, defaultResponse, sizeof(response->unknown));
    
    return PG_SUBCOMMAND_RETURN_CODE_OK;
}


//--------------------------------------------------------------------+
// Command setup
//--------------------------------------------------------------------+

PG_SUBCOMMAND_TABLE(unknown_17) = {
    PG_SUBCOMMAND_NONE,
    PG_SUBCOMMAND_UNKNOWN_WCB(1, unknown_17, unknown_1),
    PG_SUBCOMMAND_NONE,
    PG_SUBCOMMAND_UNKNOWN_WCB(3, unknown_17, unknown_3),
    PG_SUBCOMMAND_UNKNOWN(4),
};

PG_COMMAND(17, unknown_17, "Unknown 17");