#include "command.h"

// Source - 2026-08-14
// https://github.com/ndeadly/switch2_controller_research/blob/master/commands.md#command-0x13---unknown


//--------------------------------------------------------------------+
// Command setup
//--------------------------------------------------------------------+

PG_SUBCOMMAND_TABLE(unknown_19) = {
    PG_SUBCOMMAND_NONE,
    PG_SUBCOMMAND_UNKNOWN(1),
    PG_SUBCOMMAND_UNKNOWN(2),
    PG_SUBCOMMAND_UNKNOWN(3),
};

PG_COMMAND(19, unknown_19, "Unknown 19");