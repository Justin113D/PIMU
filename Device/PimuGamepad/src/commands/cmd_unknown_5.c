#include "command.h"

// Source - 2026-08-14
// https://github.com/ndeadly/switch2_controller_research/blob/master/commands.md#command-0x05---unknown


//--------------------------------------------------------------------+
// Command setup
//--------------------------------------------------------------------+

PG_SUBCOMMAND_TABLE(unknown_5) = {
    PG_SUBCOMMAND_NONE,
    PG_SUBCOMMAND_UNKNOWN(1)
};

PG_COMMAND(5, unknown_5, "Unknown 5");