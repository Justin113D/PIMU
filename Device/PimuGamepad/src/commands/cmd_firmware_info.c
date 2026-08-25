#include "command.h"

// Source - 2026-08-14
// https://github.com/ndeadly/switch2_controller_research/blob/master/commands.md#command-0x10---firmware-info

//--------------------------------------------------------------------+
// Get version
//--------------------------------------------------------------------+

#include "firmware.h"

PG_SUBCOMMAND_CALLBACK_DEF(firmware_info, get_version)
{
    PG_SUBCOMMAND_RESPONSE_SETUP(PDFirmwareVersion);

    *response = pd_firmware_version;

    return PG_SUBCOMMAND_RETURN_CODE_OK;
}


//--------------------------------------------------------------------+
// Command setup
//--------------------------------------------------------------------+

PG_SUBCOMMAND_TABLE(firmware_info) = {
    PG_SUBCOMMAND_NONE,
    PG_SUBCOMMAND(1, "Get version", firmware_info, get_version),
};

PG_COMMAND(16, firmware_info, "Firmware info");