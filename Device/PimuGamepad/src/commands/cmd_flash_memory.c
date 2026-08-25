#include "command.h"

#include "flash.h"

// Source - 2026-08-14
// https://github.com/ndeadly/switch2_controller_research/blob/master/commands.md#command-0x02---flash-memory

//--------------------------------------------------------------------+
// Read Block
//--------------------------------------------------------------------+

typedef PACKED_STRUCT
{
    uint8_t unknown[4];
    uint32_t read_address;
} read_blockRequest;

typedef PACKED_STRUCT
{
    uint8_t read_length;
    uint8_t unknown[3];
    uint32_t read_address;
    uint8_t data[64];
} read_blockResponse;

PG_SUBCOMMAND_CALLBACK_DEF(flash_memory, read_block)
{
    PG_SUBCOMMAND_REQUEST_SETUP(read_blockRequest);
    PG_SUBCOMMAND_RESPONSE_SETUP(read_blockResponse);

    response->read_length = sizeof(response->data);
    PG_RESPONSE_INIT(unknown);
    response->read_address = request->read_address;

    pg_flash_simulate_read(
        data,
        response->data,
        response->read_length,
        response->read_address
    );

    return PG_SUBCOMMAND_RETURN_CODE_OK;
}


//--------------------------------------------------------------------+
// Write Block
//--------------------------------------------------------------------+

typedef PACKED_STRUCT
{
    uint8_t unknown[4];
    uint32_t write_address;
    uint8_t data[64];
} write_blockRequest;

typedef PACKED_STRUCT
{
    uint8_t unknown[4];
    uint32_t write_address;
} write_blockResponse;

PG_SUBCOMMAND_CALLBACK_DEF(flash_memory, write_block)
{
    PG_SUBCOMMAND_REQUEST_SETUP(write_blockRequest);
    PG_SUBCOMMAND_RESPONSE_SETUP(write_blockResponse);

    PG_RESPONSE_INIT(unknown);
    response->write_address = request->write_address;
    
    return PG_SUBCOMMAND_RETURN_CODE_OK;
}


//--------------------------------------------------------------------+
// Erase sector
//--------------------------------------------------------------------+

typedef PACKED_STRUCT
{
    uint8_t unknown[4];
    uint32_t erase_address;
} erase_sectorRequest;

typedef PACKED_STRUCT
{
    uint8_t unknown[4];
} erase_sectorResponse;

PG_SUBCOMMAND_CALLBACK_DEF(flash_memory, erase_sector)
{
    PG_SUBCOMMAND_REQUEST_SETUP(erase_sectorRequest);
    PG_SUBCOMMAND_RESPONSE_SETUP(erase_sectorResponse);

    PG_RESPONSE_INIT(unknown);

    return PG_SUBCOMMAND_RETURN_CODE_OK;
}


//--------------------------------------------------------------------+
// Read
//--------------------------------------------------------------------+

typedef PACKED_STRUCT
{
    uint8_t read_length;
    uint8_t unknown[3];
    uint32_t read_address;
} readRequest;

typedef PACKED_STRUCT
{
    uint8_t read_length;
    uint8_t unknown[3];
    uint32_t read_address;
    uint8_t data[80];
} readResponse;

PG_SUBCOMMAND_CALLBACK_DEF(flash_memory, read)
{
    PG_SUBCOMMAND_REQUEST_SETUP(readRequest);
    PG_SUBCOMMAND_RESPONSE_SETUP(readResponse);

    response->read_length = MIN(request->read_length, sizeof(response->data));
    PG_RESPONSE_INIT(unknown);
    response->read_address = request->read_address;

    pg_flash_simulate_read(
        data,
        response->data,
        response->read_length,
        response->read_address
    );
    
    return PG_SUBCOMMAND_RETURN_CODE_OK;
}


//--------------------------------------------------------------------+
// Write
//--------------------------------------------------------------------+

typedef PACKED_STRUCT
{
    uint8_t write_length;
    uint8_t unknown[3];
    uint32_t write_address;
    uint8_t data[79];
} writeRequest;

typedef PACKED_STRUCT
{
    uint8_t unknown[4];
    uint32_t write_address;
} writeResponse;

PG_SUBCOMMAND_CALLBACK_DEF(flash_memory, write)
{
    PG_SUBCOMMAND_REQUEST_SETUP(writeRequest);
    PG_SUBCOMMAND_RESPONSE_SETUP(writeResponse);

    PG_RESPONSE_INIT(unknown);
    response->write_address = request->write_address;

    return PG_SUBCOMMAND_RETURN_CODE_OK;
}


//--------------------------------------------------------------------+
// Command setup
//--------------------------------------------------------------------+

PG_SUBCOMMAND_TABLE(flash_memory) = {
    PG_SUBCOMMAND_NONE,
    PG_SUBCOMMAND(1, "Read block", flash_memory, read_block),
    PG_SUBCOMMAND(2, "Write block", flash_memory, write_block),
    PG_SUBCOMMAND(3, "Erase sector", flash_memory, erase_sector),
    PG_SUBCOMMAND(4, "Read", flash_memory, read),
    PG_SUBCOMMAND(5, "Write", flash_memory, write),
    PG_SUBCOMMAND_UNKNOWN(6),
};

PG_COMMAND(2, flash_memory, "Flash memory");