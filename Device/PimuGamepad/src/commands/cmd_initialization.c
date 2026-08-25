#include "command.h"

// Source - 2026-08-14
// https://github.com/ndeadly/switch2_controller_research/blob/master/commands.md#command-0x03---initialisation

//--------------------------------------------------------------------+
// Bluetooth wake
//--------------------------------------------------------------------+

typedef PACKED_STRUCT
{
    uint8_t unknown[4];
} BluetoothWakeRequest;

PG_SUBCOMMAND_CALLBACK_DEF(initialization, bluetooth_wake)
{
    PG_SUBCOMMAND_REQUEST_SETUP(BluetoothWakeRequest);

    return PG_SUBCOMMAND_RETURN_CODE_OK;
}


//--------------------------------------------------------------------+
// Bluetooth cancel
//--------------------------------------------------------------------+

PG_SUBCOMMAND_CALLBACK_DEF(initialization, bluetooth_cancel)
{
    return PG_SUBCOMMAND_RETURN_CODE_OK;
}


//--------------------------------------------------------------------+
// Enable USB HID reports
//--------------------------------------------------------------------+

typedef PACKED_STRUCT
{
    uint8_t enable;
    uint8_t reserved[3];
} EnableUSBRequest;

typedef PACKED_STRUCT
{
    uint8_t unknown[4];
} EnableUSBResponse;

PG_SUBCOMMAND_CALLBACK_DEF(initialization, enable_usb)
{
    PG_SUBCOMMAND_REQUEST_SETUP(EnableUSBRequest);
    PG_SUBCOMMAND_RESPONSE_SETUP(EnableUSBResponse);

    pg_status_set_hid_reports_enabled(&data->status, request->enable);

    // Default response of { 0x01, 0x00, 0x00, 0x00 }
    PG_RESPONSE_INIT(unknown);
    response->unknown[0] = 1;

    return PG_SUBCOMMAND_RETURN_CODE_OK;
}


//--------------------------------------------------------------------+
// Send pairing info
//--------------------------------------------------------------------+

typedef PACKED_STRUCT
{
    uint8_t host_address[6];
    uint8_t ltk[16];
} SendPairingRequest;

PG_SUBCOMMAND_CALLBACK_DEF(initialization, send_pairing_info)
{
    PG_SUBCOMMAND_REQUEST_SETUP(SendPairingRequest);

    pg_connection_set_host_address_rev(&data->connection, request->host_address);
    pg_connection_set_ltk_bluetooth_rev(&data->connection, request->ltk);

    return PG_SUBCOMMAND_RETURN_CODE_OK;
}


//--------------------------------------------------------------------+
// Clear pairing info
//--------------------------------------------------------------------+

PG_SUBCOMMAND_CALLBACK_DEF(initialization, clear_pairing_info)
{
    return PG_SUBCOMMAND_RETURN_CODE_OK;
}


//--------------------------------------------------------------------+
// Store pairing info
//--------------------------------------------------------------------+

PG_SUBCOMMAND_CALLBACK_DEF(initialization, store_pairing_info)
{
    return PG_SUBCOMMAND_RETURN_CODE_OK;
}


//--------------------------------------------------------------------+
// Select input report
//--------------------------------------------------------------------+

typedef PACKED_STRUCT
{
    uint8_t report_id;
    uint8_t unknown[3];
} SelectInputReportRequest;

PG_SUBCOMMAND_CALLBACK_DEF(initialization, select_input_report)
{
    PG_SUBCOMMAND_REQUEST_SETUP(SelectInputReportRequest);

    pg_status_set_report_id(&data->status, request->report_id);

    return PG_SUBCOMMAND_RETURN_CODE_OK;
}


//--------------------------------------------------------------------+
// Initialise USB
//--------------------------------------------------------------------+

typedef PACKED_STRUCT
{
    uint8_t unknown[2];
    uint8_t host_address[6];
} InitializeUSBRequest;

typedef PACKED_STRUCT
{
    uint8_t unknown[4];
} InitializeUSBResponse;

PG_SUBCOMMAND_CALLBACK_DEF(initialization, initialize_usb)
{
    PG_SUBCOMMAND_REQUEST_SETUP(InitializeUSBRequest);
    PG_SUBCOMMAND_RESPONSE_SETUP(InitializeUSBResponse);

    pg_connection_set_host_address_rev(&data->connection, request->host_address);
    pg_status_set_usb_enabled(&data->status, true);
    
    // Default response of { 0x01, 0x00, 0x00, 0x00 }
    PG_RESPONSE_INIT(unknown);
    response->unknown[0] = 1;

    return PG_SUBCOMMAND_RETURN_CODE_OK;
}

//--------------------------------------------------------------------+
// Unknown 15
//--------------------------------------------------------------------+

typedef PACKED_STRUCT
{
    uint8_t unknown[4];
} Unknown15Response;

PG_SUBCOMMAND_CALLBACK_DEF(initialization, unknown_15)
{
    PG_SUBCOMMAND_RESPONSE_SETUP(Unknown15Response);
    
    // Default response of { 0x05, 0x00, 0x00, 0x00 }
    PG_RESPONSE_INIT(unknown);
    response->unknown[0] = 5;

    return PG_SUBCOMMAND_RETURN_CODE_OK;
}

//--------------------------------------------------------------------+
// Command setup
//--------------------------------------------------------------------+

PG_SUBCOMMAND_TABLE(initialization) = {
    PG_SUBCOMMAND_NONE,
    PG_SUBCOMMAND(1, "Bluetooth wake", initialization, bluetooth_wake),
    PG_SUBCOMMAND(2, "Bluetooth cancel", initialization, bluetooth_cancel),
    PG_SUBCOMMAND(3, "Enable USB HID reports", initialization, enable_usb),
    PG_SUBCOMMAND_UNKNOWN(4),
    PG_SUBCOMMAND_UNKNOWN(5),
    PG_SUBCOMMAND_UNKNOWN(6),
    PG_SUBCOMMAND(7, "Send pairing info", initialization, send_pairing_info),
    PG_SUBCOMMAND(8, "Clear pairing info", initialization, clear_pairing_info),
    PG_SUBCOMMAND(9, "Store pairing info", initialization, store_pairing_info),
    PG_SUBCOMMAND(10, "Select input report", initialization, select_input_report),
    PG_SUBCOMMAND_NONE,
    PG_SUBCOMMAND_UNKNOWN(12),
    PG_SUBCOMMAND(13, "Initialise USB", initialization, initialize_usb),
    PG_SUBCOMMAND_NONE,
    PG_SUBCOMMAND_UNKNOWN_WCB(15, initialization, unknown_15),
};

PG_COMMAND(3, initialization, "Initialization");
