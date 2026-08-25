#pragma GCC optimize("O0")

#include <stdarg.h>
#include <stdlib.h>

#include "pimu_gamepad.h"
#include "command.h"
#include "connection.h"
#include "status.h"
#include "firmware.h"
#include "flash.h"
#include "commands/command_table.h"

struct PimuGamepad
{
    const char *name;
    PGDebugCB debug_cb;

    PGGamepadData data;
};

PimuGamepad* pimu_gamepad_alloc(void)
{
    PimuGamepad* result = calloc(1, sizeof(PimuGamepad));
    result->data.device_colors = default_device_colors;
    return result;
}

void pimu_gamepad_free(PimuGamepad* gamepad)
{
    free(gamepad);
}

static void debug_print(PimuGamepad *gamepad, PG_DEBUG_TYPE type, char *format, ...)
{
    if (gamepad->debug_cb != NULL)
    {
        va_list args;
        va_start(args, format);
        gamepad->debug_cb(gamepad, type, format, args);
        va_end(args);
    }
}


//--------------------------------------------------------------------+

void pimu_gamepad_set_name(PimuGamepad *gamepad, const char *name)
{
    gamepad->name = name;
}

const char *pimu_gamepad_get_name(PimuGamepad *gamepad)
{
    return gamepad->name;
}

void pimu_gamepad_set_debug_cb(PimuGamepad *gamepad, PGDebugCB callback)
{
    gamepad->debug_cb = callback;
}

void pimu_gamepad_set_device_colors(PimuGamepad *gamepad, PimuGamepadDeviceColors *colors)
{
    gamepad->data.device_colors = *colors;
}

void pimu_gamepad_set_device_address(PimuGamepad* gamepad, uint8_t* address)
{
    pg_connection_set_device_address(&gamepad->data.connection, address);
}

PimuGamepadDeviceColors *pimu_gamepad_get_device_colors(PimuGamepad *gamepad)
{
    return &gamepad->data.device_colors;
}

void pimu_gamepad_set_player_leds_changed_cb(PimuGamepad *connector, PGPlayerLEDsChangedCB callback)
{
    pg_status_set_player_leds_changed_cb(&connector->data.status, callback);
}

//--------------------------------------------------------------------+

#define CONTROL_STAGE_SETUP 1

#define REQUEST_DIR_OUT 0
#define REQUEST_DIR_IN 1

#define REQUEST_TYPE_STANDARD 0
#define REQUEST_TYPE_VENDOR 2

#define REQUEST_RECIPIENT_DEVICE 0
#define REQUEST_RECIPIENT_UNKNOWN_4 4

#define REQUEST_ID_DEVICE_INFO 2
#define REQUEST_ID_FACTORY_DATA 3

typedef PACKED_STRUCT
{
    PDFirmwareVersion firmware_version;
    uint8_t bluetooth_address[6];
}
DeviceInfo;

PG_CONTROL_TRANSFER_RETURN_CODE pimu_gamepad_control_transfer_request(PimuGamepad *gamepad, PGControlTransferRequest *request, PGControlTransferResponse *response)
{
    if (request->stage != CONTROL_STAGE_SETUP)
    {
        return PG_CONTROL_TRANSFER_RETURN_CODE_TRUE;
    }

    if (request->direction == REQUEST_DIR_IN)
    {
        switch (request->request_id)
        {
        case REQUEST_ID_DEVICE_INFO:
            debug_print(gamepad, PG_DEBUG_TYPE_INFO, "Requesting device info");
            DeviceInfo *device_info = (DeviceInfo *)response->data;
            device_info->firmware_version = pd_firmware_version;
            pg_connection_get_device_address_rev(&gamepad->data.connection, device_info->bluetooth_address);
            response->data_size = sizeof(DeviceInfo);
            return PG_CONTROL_TRANSFER_RETURN_CODE_TRUE_WITH_DATA;

        case REQUEST_ID_FACTORY_DATA:
            debug_print(gamepad, PG_DEBUG_TYPE_INFO, "Requesting factory data");
            pg_flash_simulate_read(&gamepad->data, response->data, 64, 0x13000);
            response->data_size = 64;
            return PG_CONTROL_TRANSFER_RETURN_CODE_TRUE_WITH_DATA;

        default:
            debug_print(gamepad, PG_DEBUG_TYPE_WARNING, "Requesting unknown; Ignoring");
            return PG_CONTROL_TRANSFER_RETURN_CODE_FALSE;
        }
    }
    else if (request->direction == REQUEST_DIR_OUT 
        && request->type == REQUEST_TYPE_STANDARD 
        && request->recipient == REQUEST_RECIPIENT_UNKNOWN_4)
    {
        debug_print(gamepad, PG_DEBUG_TYPE_INFO, "Requesting standard unknown 4?");
        response->data_size = 0;
        return PG_CONTROL_TRANSFER_RETURN_CODE_TRUE_WITH_DATA;
    }
    else if (request->direction == REQUEST_DIR_OUT 
        && request->type == REQUEST_TYPE_VENDOR 
        && request->recipient == REQUEST_RECIPIENT_DEVICE)
    {
        debug_print(gamepad, PG_DEBUG_TYPE_INFO, "Requesting vendor device?");
        response->data_size = 0;
        return PG_CONTROL_TRANSFER_RETURN_CODE_TRUE_WITH_DATA;
    }
    else
    {
        debug_print(gamepad, PG_DEBUG_TYPE_WARNING, "Requesting unknown; Ignoring");
        return PG_CONTROL_TRANSFER_RETURN_CODE_FALSE;
    }
}

static void process_command(PimuGamepad *gamepad, uint16_t request_size, PGCommandHead* command_request, PGCommandInterface *command_response_interface)
{
    debug_print(gamepad, PG_DEBUG_TYPE_INFO, "Received command %i.%i", command_request->command_id, command_request->subcommand_id);

    if (command_request->data_length > request_size - sizeof(PGCommandHead))
    {
        debug_print(gamepad, PG_DEBUG_TYPE_INFO, "Request data size (%i) is larger than buffer size (%i)!", command_request->data_length, request_size - sizeof(PGCommandHead));
        return;
    }

    const PGCommand *command =
        command_request->command_id < sizeof(pg_command_table)
            ? pg_command_table[command_request->command_id]
            : PG_COMMAND_NONE;

    if (command == PG_COMMAND_NONE)
    {
        debug_print(gamepad, PG_DEBUG_TYPE_ERROR, "\tCommand not implemented", command_request->command_id);
        return;
    }

    if (command->id != command_request->command_id)
    {
        debug_print(gamepad, PG_DEBUG_TYPE_ERROR, "\tCommand ID mismatch (%i)", command->id);
        return;
    }

    debug_print(gamepad, PG_DEBUG_TYPE_INFO, "\tCommand Name: %s", command->name);

    if (command_request->subcommand_id >= command->subcommand_count)
    {
        debug_print(gamepad, PG_DEBUG_TYPE_ERROR, "\tSubcommand ID out of range (subcommand count: %i)", command->name);
        return;
    }

    const PGSubcommand *subcommand = &command->subcommand_table[command_request->subcommand_id];

    if (subcommand->name == NULL)
    {
        debug_print(gamepad, PG_DEBUG_TYPE_ERROR, "\tSubcommand not available", command_request->subcommand_id, command->name);
        return;
    }

    if (subcommand->id != command_request->subcommand_id)
    {
        debug_print(gamepad, PG_DEBUG_TYPE_ERROR, "\tSubommand ID mismatch (%i)", subcommand->id);
        return;
    }

    debug_print(gamepad, PG_DEBUG_TYPE_INFO, "\tSubcommand Name: %s", subcommand->name);

    if (subcommand->callback == NULL)
    {
        debug_print(gamepad, PG_DEBUG_TYPE_ERROR, "\tSubcommand has no callback");
        return;
    }

    uint32_t return_code = subcommand->callback(
        &gamepad->data,
        command_request,
        command_response_interface
    );

    if (return_code)
    {
        debug_print(gamepad, PG_DEBUG_TYPE_ERROR, "\tSubcommand failed: %i", return_code);
        command_response_interface->data_size = 0;
        return;
    }
}

bool pimu_gamepad_command_request(PimuGamepad *gamepad, PGCommandRequest *request, PGCommandResponse *response)
{
    if (request->data_size < sizeof(PGCommandHead))
    {
        debug_print(gamepad, PG_DEBUG_TYPE_ERROR, "Received packet is too small");
        return false;
    }

    PGCommandHead *command_request = (PGCommandHead *)request->data;

    if (command_request->direction != PG_COMMAND_DIRECTION_H2D || command_request->transport != PG_COMMAND_TRANSPORT_USB)
    {
        debug_print(gamepad, PG_DEBUG_TYPE_ERROR, "Request is not an incoming USB request");
        return false;
    }

    PGCommandHead *command_response = (PGCommandHead *)response->data;
    command_response->command_id = command_request->command_id;
    command_response->direction = PG_COMMAND_DIRECTION_D2H;
    command_response->transport = PG_COMMAND_TRANSPORT_USB;
    command_response->subcommand_id = command_request->subcommand_id;
    command_response->acknowledgement = PG_COMMAND_ACKNOWLEDGEMENT_USB;
    command_response->reserved[0] = 0;
    command_response->reserved[1] = 0;

    PGCommandInterface command_response_interface = {
        .data_size = 0,
        .max_data_size = sizeof(response->data) - sizeof(PGCommandHead),
        .data = command_response->data
    };

    process_command(
        gamepad,
        request->data_size,
        command_request,
        &command_response_interface);

    response->data_size = sizeof(PGCommandHead) + command_response_interface.data_size;

    return true;
}

//--------------------------------------------------------------------+

bool pimu_gamepad_get_usb_enabled(PimuGamepad* gamepad)
{
    return pg_status_get_usb_enabled(&gamepad->data.status);
}

uint8_t pimu_gamepad_get_player_leds_mask(PimuGamepad* gamepad)
{
    return pg_status_get_player_leds_mask(&gamepad->data.status);
}

bool pimu_gamepad_get_player_leds_flashing(PimuGamepad* gamepad)
{
    return pg_status_get_player_leds_flashing(&gamepad->data.status);
}

uint8_t pimu_gamepad_get_feature_mask(PimuGamepad* gamepad)
{
    return pg_status_get_feature_mask(&gamepad->data.status);
}

uint8_t pimu_gamepad_get_feature_enabled(PimuGamepad* gamepad)
{
    return pg_status_get_feature_enabled(&gamepad->data.status);
}

uint8_t pimu_gamepad_get_feature_active(PimuGamepad* gamepad)
{
    return pg_status_get_feature_active(&gamepad->data.status);
}

bool pimu_gamepad_get_hid_reports_enabled(PimuGamepad* gamepad)
{
    return pg_status_get_hid_reports_enabled(&gamepad->data.status);
}

uint8_t pimu_gamepad_get_report_id(PimuGamepad* gamepad)
{
    return pg_status_get_report_id(&gamepad->data.status);
}