#include "command.h"

// Source - 2026-08-14
// https://github.com/ndeadly/switch2_controller_research/blob/master/commands.md#command-0x04---unknown


//--------------------------------------------------------------------+
// Command setup
//--------------------------------------------------------------------+

PG_SUBCOMMAND_TABLE(unknown_4) = {
    PG_SUBCOMMAND_NONE,
};

PG_COMMAND(4, unknown_4, "Unknown 4");