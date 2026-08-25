#include "command.h"

#include <string.h>

// Source - 2026-08-14
// https://github.com/ndeadly/switch2_controller_research/blob/master/commands.md#command-0x16---unknown

//--------------------------------------------------------------------+
// Unknown 1
//--------------------------------------------------------------------+

typedef PACKED_STRUCT
{
    uint8_t unknown[24];
} Unknown1Response;

PG_SUBCOMMAND_CALLBACK_DEF(unknown_22, unknown_1)
{
    PG_SUBCOMMAND_RESPONSE_SETUP(Unknown1Response);

    PG_RESPONSE_INIT(unknown);
    
    return PG_SUBCOMMAND_RETURN_CODE_OK;
}


//--------------------------------------------------------------------+
// Command setup
//--------------------------------------------------------------------+

PG_SUBCOMMAND_TABLE(unknown_22) = {
    PG_SUBCOMMAND_NONE,
    PG_SUBCOMMAND_UNKNOWN_WCB(1, unknown_22, unknown_1),
};

PG_COMMAND(22, unknown_22, "Unknown 22");