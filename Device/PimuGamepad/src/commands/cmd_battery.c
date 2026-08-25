#include "command.h"

// Source - 2026-08-14
// https://github.com/ndeadly/switch2_controller_research/blob/master/commands.md#command-0x0b---battery

//--------------------------------------------------------------------+
// Get voltage
//--------------------------------------------------------------------+

typedef PACKED_STRUCT
{
    uint16_t voltage;
    uint8_t unknown[2];
} GetVoltageResponse;

PG_SUBCOMMAND_CALLBACK_DEF(battery, get_voltage)
{
    PG_SUBCOMMAND_RESPONSE_SETUP(GetVoltageResponse);

     // sample response
    response->voltage = 0x0ea5; // 3745 mV
    PG_RESPONSE_INIT(unknown);

    return PG_SUBCOMMAND_RETURN_CODE_OK;
}

//--------------------------------------------------------------------+
// Get charge
//--------------------------------------------------------------------+

typedef PACKED_STRUCT
{
    uint16_t charge;
    uint8_t unknown[2];
} GetChargeResponse;

PG_SUBCOMMAND_CALLBACK_DEF(battery, get_charge)
{
    PG_SUBCOMMAND_RESPONSE_SETUP(GetChargeResponse);

    // sample response
    response->charge = 0x0020; 
    response->unknown[0] = 0x84;
    response->unknown[1] = 0;

    return PG_SUBCOMMAND_RETURN_CODE_OK;
}

//--------------------------------------------------------------------+
// Unknown 6
//--------------------------------------------------------------------+

typedef PACKED_STRUCT
{
    uint8_t unknown[4];
} Unknown6Response;

PG_SUBCOMMAND_CALLBACK_DEF(battery, unknown_6)
{
    PG_SUBCOMMAND_RESPONSE_SETUP(Unknown6Response);

    // sample response
    PG_RESPONSE_INIT(unknown);
    response->unknown[0] = 0x11;

    return PG_SUBCOMMAND_RETURN_CODE_OK;
}


//--------------------------------------------------------------------+
// Unknown 7
//--------------------------------------------------------------------+

PG_SUBCOMMAND_CALLBACK_DEF(battery, unknown_7)
{
    return PG_SUBCOMMAND_RETURN_CODE_OK;
}

//--------------------------------------------------------------------+
// Command setup
//--------------------------------------------------------------------+

PG_SUBCOMMAND_TABLE(battery) = {
    PG_SUBCOMMAND_NONE,
    PG_SUBCOMMAND_NONE,
    PG_SUBCOMMAND_NONE,
    PG_SUBCOMMAND(3, "Get voltage", battery, get_voltage),
    PG_SUBCOMMAND(4, "Get charge", battery, get_charge),
    PG_SUBCOMMAND_NONE,
    PG_SUBCOMMAND_UNKNOWN_WCB(6, battery, unknown_6),
    PG_SUBCOMMAND_UNKNOWN_WCB(7, battery, unknown_7),
};

PG_COMMAND(11, battery, "Battery");