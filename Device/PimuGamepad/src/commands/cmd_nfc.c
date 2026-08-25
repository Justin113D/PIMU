#include "command.h"

// Source - 2026-08-14
// https://github.com/ndeadly/switch2_controller_research/blob/master/commands.md#command-0x01---nfc

//--------------------------------------------------------------------+
// Get status
//--------------------------------------------------------------------+

PG_SUBCOMMAND_CALLBACK_DEF(nfc, get_status)
{
    return PG_SUBCOMMAND_RETURN_CODE_NOT_IMPLEMENTED;
}


//--------------------------------------------------------------------+
// Read device
//--------------------------------------------------------------------+

PG_SUBCOMMAND_CALLBACK_DEF(nfc, read_device)
{
    return PG_SUBCOMMAND_RETURN_CODE_NOT_IMPLEMENTED;
}


//--------------------------------------------------------------------+
// Write device
//--------------------------------------------------------------------+

PG_SUBCOMMAND_CALLBACK_DEF(nfc, write_device)
{
    return PG_SUBCOMMAND_RETURN_CODE_NOT_IMPLEMENTED;
}


//--------------------------------------------------------------------+
// Write device
//--------------------------------------------------------------------+

typedef PACKED_STRUCT
{
    uint8_t unknown[4];
} Unknown12Response;

PG_SUBCOMMAND_CALLBACK_DEF(nfc, unknown_12)
{
    PG_SUBCOMMAND_RESPONSE_SETUP(Unknown12Response);

    response->unknown[0] = 0x61;
    response->unknown[1] = 0x12;
    response->unknown[2] = 0x50;
    response->unknown[3] = 0x0d;

    return PG_SUBCOMMAND_RETURN_CODE_OK;
}


//--------------------------------------------------------------------+
// Write buffer
//--------------------------------------------------------------------+

PG_SUBCOMMAND_CALLBACK_DEF(nfc, write_buffer)
{
    return PG_SUBCOMMAND_RETURN_CODE_NOT_IMPLEMENTED;
}


//--------------------------------------------------------------------+
// Read buffer
//--------------------------------------------------------------------+

PG_SUBCOMMAND_CALLBACK_DEF(nfc, read_buffer)
{
    return PG_SUBCOMMAND_RETURN_CODE_NOT_IMPLEMENTED;
}


//--------------------------------------------------------------------+
// Command setup
//--------------------------------------------------------------------+

PG_SUBCOMMAND_TABLE(nfc) = {
    PG_SUBCOMMAND_NONE,
    PG_SUBCOMMAND_UNKNOWN(1),
    PG_SUBCOMMAND_UNKNOWN(2),
    PG_SUBCOMMAND_UNKNOWN(3),
    PG_SUBCOMMAND_UNKNOWN(4),
    PG_SUBCOMMAND(5, "Get status", nfc, get_status),
    PG_SUBCOMMAND(6, "Read device", nfc, read_device),
    PG_SUBCOMMAND_NONE,
    PG_SUBCOMMAND(8, "Write device", nfc, write_device),
    PG_SUBCOMMAND_NONE,
    PG_SUBCOMMAND_NONE,
    PG_SUBCOMMAND_NONE,
    PG_SUBCOMMAND_UNKNOWN_WCB(12, nfc, unknown_12),
    PG_SUBCOMMAND_NONE,
    PG_SUBCOMMAND_NONE,
    PG_SUBCOMMAND_NONE,
    PG_SUBCOMMAND_NONE,
    PG_SUBCOMMAND_NONE,
    PG_SUBCOMMAND_NONE,
    PG_SUBCOMMAND_NONE,
    PG_SUBCOMMAND(20, "Write buffer", nfc, write_buffer),
    PG_SUBCOMMAND(21, "Read buffer", nfc, read_buffer),
};

PG_COMMAND(1, nfc, "NFC");