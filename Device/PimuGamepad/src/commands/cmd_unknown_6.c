#include "command.h"

// Source - 2026-08-14
// https://github.com/ndeadly/switch2_controller_research/blob/master/commands.md#command-0x06---unknown


//--------------------------------------------------------------------+
// Command setup
//--------------------------------------------------------------------+

PG_SUBCOMMAND_TABLE(unknown_6) = {
    PG_SUBCOMMAND_NONE,
    PG_SUBCOMMAND_UNKNOWN(1),
    PG_SUBCOMMAND_NOCB(2, "Shutdown controller?"),
    PG_SUBCOMMAND_NOCB(3, "Reboot controller?"),
};

PG_COMMAND(6, unknown_6, "Unknown 6");