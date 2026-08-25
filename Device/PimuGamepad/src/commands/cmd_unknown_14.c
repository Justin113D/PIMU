#include "command.h"

// Source - 2026-08-14
// https://github.com/ndeadly/switch2_controller_research/blob/master/commands.md#command-0x0e---unknown


//--------------------------------------------------------------------+
// Command setup
//--------------------------------------------------------------------+

PG_SUBCOMMAND_TABLE(unknown_14) = {
    PG_SUBCOMMAND_NONE,
};

PG_COMMAND(14, unknown_14, "Unknown 14");