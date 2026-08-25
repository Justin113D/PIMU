#include "command.h"

// Source - 2026-08-14
// https://github.com/ndeadly/switch2_controller_research/blob/master/commands.md#command-0x0d---firmware-update

//--------------------------------------------------------------------+
// Initialize update?
//--------------------------------------------------------------------+

PG_SUBCOMMAND_CALLBACK_DEF(firmware_update, initialize)
{
    return PG_SUBCOMMAND_RETURN_CODE_OK;
}


//--------------------------------------------------------------------+
// Set failsafe address?
//--------------------------------------------------------------------+

typedef PACKED_STRUCT
{
    uint8_t unknown;
    uint32_t address;
} SetFailsafeAddressRequest;

PG_SUBCOMMAND_CALLBACK_DEF(firmware_update, set_failsafe_address)
{
    return PG_SUBCOMMAND_RETURN_CODE_OK;
}


//--------------------------------------------------------------------+
// Set image size?
//--------------------------------------------------------------------+

typedef PACKED_STRUCT
{
    uint8_t unknown[5];
    uint32_t size;
} SetImageSizeRequest;

PG_SUBCOMMAND_CALLBACK_DEF(firmware_update, set_image_size)
{
    return PG_SUBCOMMAND_RETURN_CODE_OK;
}


//--------------------------------------------------------------------+
// Transfer update data
//--------------------------------------------------------------------+

typedef PACKED_STRUCT
{
    uint32_t length;
    uint8_t data[76];
} TransferUpdateDataRequest;

PG_SUBCOMMAND_CALLBACK_DEF(firmware_update, transfer_update_data)
{
    return PG_SUBCOMMAND_RETURN_CODE_OK;
}


//--------------------------------------------------------------------+
// End data transfer?
//--------------------------------------------------------------------+

PG_SUBCOMMAND_CALLBACK_DEF(firmware_update, end_data_transfer)
{
    return PG_SUBCOMMAND_RETURN_CODE_OK;
}


//--------------------------------------------------------------------+
// Verify udpate?
//--------------------------------------------------------------------+

typedef PACKED_STRUCT
{
    uint8_t unknown[5];
    uint32_t size;
    uint32_t checksum;
} VerifyUpdateRequest;

PG_SUBCOMMAND_CALLBACK_DEF(firmware_update, verify_update)
{
    return PG_SUBCOMMAND_RETURN_CODE_NOT_IMPLEMENTED;
}


//--------------------------------------------------------------------+
// Finalize update?
//--------------------------------------------------------------------+

PG_SUBCOMMAND_CALLBACK_DEF(firmware_update, finalize)
{
    return PG_SUBCOMMAND_RETURN_CODE_NOT_IMPLEMENTED;
}


//--------------------------------------------------------------------+
// Command setup
//--------------------------------------------------------------------+

PG_SUBCOMMAND_TABLE(firmware_update) = {
    PG_SUBCOMMAND_NONE,
    PG_SUBCOMMAND(1, "Initialize update?", firmware_update, initialize),
    PG_SUBCOMMAND(2, "Set failsafe address?", firmware_update, set_failsafe_address),
    PG_SUBCOMMAND(3, "Set image size?", firmware_update, set_image_size),
    PG_SUBCOMMAND(4, "Transfer update data", firmware_update, transfer_update_data),
    PG_SUBCOMMAND(5, "End data transfer?", firmware_update, end_data_transfer),
    PG_SUBCOMMAND(6, "Verify udpate?", firmware_update, verify_update),
    PG_SUBCOMMAND(7, "Finalize update?", firmware_update, finalize),
};

PG_COMMAND(13, firmware_update, "Firmware update");