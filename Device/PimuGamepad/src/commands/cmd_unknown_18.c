#include "command.h"

// Source - 2026-08-14
// https://github.com/ndeadly/switch2_controller_research/blob/master/commands.md#command-0x12---unknown


//--------------------------------------------------------------------+
// Command setup
//--------------------------------------------------------------------+

PG_SUBCOMMAND_TABLE(unknown_18) = {
    PG_SUBCOMMAND_NONE,
};

PG_COMMAND(18, unknown_18, "Unknown 18");