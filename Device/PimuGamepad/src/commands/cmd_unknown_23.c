#include "command.h"

// Source - 2026-08-14
// https://github.com/ndeadly/switch2_controller_research/blob/master/commands.md#command-0x17---unknown


//--------------------------------------------------------------------+
// Command setup
//--------------------------------------------------------------------+

PG_SUBCOMMAND_TABLE(unknown_23) = {
    PG_SUBCOMMAND_NONE,
    PG_SUBCOMMAND_NONE,
    PG_SUBCOMMAND_UNKNOWN(2),
};

PG_COMMAND(23, unknown_23, "Unknown 23");