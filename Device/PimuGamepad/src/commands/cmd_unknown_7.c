#include "command.h"

// Source - 2026-08-14
// https://github.com/ndeadly/switch2_controller_research/blob/master/commands.md#command-0x07---unknown

//--------------------------------------------------------------------+
// Unknown 1
// Unknown. This is the first command sent during initialisation.
//--------------------------------------------------------------------+

typedef PACKED_STRUCT
{
    uint8_t unknown;
} Unknown1Response;

PG_SUBCOMMAND_CALLBACK_DEF(unknown_7, unknown_1)
{
    PG_SUBCOMMAND_RESPONSE_SETUP(Unknown1Response);

    response->unknown = 0;
    
    return PG_SUBCOMMAND_RETURN_CODE_OK;
}


//--------------------------------------------------------------------+
// Command setup
//--------------------------------------------------------------------+

PG_SUBCOMMAND_TABLE(unknown_7) = {
    PG_SUBCOMMAND_NONE,
    PG_SUBCOMMAND_UNKNOWN_WCB(1, unknown_7, unknown_1),
    PG_SUBCOMMAND_UNKNOWN(2)
};

PG_COMMAND(7, unknown_7, "Unknown 7");