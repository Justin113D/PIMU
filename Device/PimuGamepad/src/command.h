#ifndef PG_COMMAND_H_
#define PG_COMMAND_H_

#include <stdint.h>
#include <string.h>

#include "common.h"
#include "gamepad.h"

typedef PACKED_STRUCT
{
    uint8_t command_id;
    uint8_t direction;
    uint8_t transport;
    uint8_t subcommand_id;
    union
    {
        struct {
            uint8_t unknown;
            uint8_t data_length;  
        };
        uint16_t acknowledgement;
    };
    uint8_t reserved[2];

    // variable length data; only for fetching the offset
    uint8_t data[];
} PGCommandHead;

#define PG_COMMAND_DIRECTION_D2H 0x01
#define PG_COMMAND_DIRECTION_H2D 0x91

#define PG_COMMAND_TRANSPORT_USB 0
#define PG_COMMAND_TRANSPORT_BLUETOOTH 1

#define PG_COMMAND_ACKNOWLEDGEMENT_BLUETOOTH 0x7810
#define PG_COMMAND_ACKNOWLEDGEMENT_USB 0xF800

//--------------------------------------------------------------------+

typedef struct
{
    uint16_t data_size;
    uint16_t max_data_size;
    uint8_t* data;
} PGCommandInterface;

typedef enum
{
    PG_SUBCOMMAND_RETURN_CODE_OK,
    PG_SUBCOMMAND_RETURN_CODE_NOT_IMPLEMENTED,
    PG_SUBCOMMAND_RETURN_CODE_REQUEST_TOO_SMALL,
    PG_SUBCOMMAND_RETURN_CODE_RESPONSE_TOO_SMALL
} PG_SUBCOMMAND_RETURN_CODE;

#define PG_SUBCOMMAND_CALLBACK_ARGS PGGamepadData* data, PGCommandHead* request_head, PGCommandInterface* response_interface
#define PG_SUBCOMMAND_CALLBACK(_command, _name) pg_command_##_command##_subcommand_##_name
#define PG_SUBCOMMAND_CALLBACK_DEF(_command, _name) static PG_SUBCOMMAND_RETURN_CODE PG_SUBCOMMAND_CALLBACK(_command, _name)(PG_SUBCOMMAND_CALLBACK_ARGS)
typedef PG_SUBCOMMAND_RETURN_CODE (*PGSubcommandCB)(PG_SUBCOMMAND_CALLBACK_ARGS);

#define PG_SUBCOMMAND_REQUEST_VERIFY(_struct) if(request_head->data_length < sizeof(_struct)) return PG_SUBCOMMAND_RETURN_CODE_REQUEST_TOO_SMALL;
#define PG_SUBCOMMAND_RESPONSE_VERIFY(_struct) if(response_interface->max_data_size < sizeof(_struct)) return PG_SUBCOMMAND_RETURN_CODE_RESPONSE_TOO_SMALL;

#define PG_SUBCOMMAND_REQUEST_SETUP(_struct)\
    PG_SUBCOMMAND_REQUEST_VERIFY(_struct)\
    _struct* request = (_struct*)request_head->data;

#define PG_SUBCOMMAND_RESPONSE_SETUP(_struct)\
    PG_SUBCOMMAND_RESPONSE_VERIFY(_struct)\
    _struct* response = (_struct*)response_interface->data;\
    response_interface->data_size = sizeof(_struct);

#define PG_RESPONSE_INIT(_field) memset(response->_field, 0, sizeof(response->_field))

//--------------------------------------------------------------------+

typedef struct 
{
    uint8_t id;
    const char* name;
    const PGSubcommandCB callback;
} PGSubcommand;

#define PG_SUBCOMMAND_TABLE(_command) \
    static const PGSubcommand pg_command_##_command##_subcommand_table[]

#define PG_SUBCOMMAND_NONE { }

#define PG_SUBCOMMAND_UNKNOWN(_id) \
    {\
        .id = _id,\
        .name = "Unknown " #_id\
    }

#define PG_SUBCOMMAND_UNKNOWN_WCB(_id, _cb_command, _cb_name) \
    {\
        .id = _id,\
        .name = "Unknown " #_id,\
        .callback = PG_SUBCOMMAND_CALLBACK(_cb_command, _cb_name)\
    }

#define PG_SUBCOMMAND_NOCB(_id, _name) \
    {\
        .id = _id,\
        .name = _name\
    }

#define PG_SUBCOMMAND(_id, _name, _cb_command, _cb_name)\
    {\
        .id = _id,\
        .name = #_name,\
        .callback = PG_SUBCOMMAND_CALLBACK(_cb_command, _cb_name)\
    }

//--------------------------------------------------------------------+

typedef struct 
{
    uint8_t id;
    const char* name;
    uint8_t subcommand_count;
    const PGSubcommand* subcommand_table;
} PGCommand;

#define PG_COMMAND_DEF(_command) extern const PGCommand pg_command_##_command

#define PG_COMMAND(_id, _command, _name)\
    const PGCommand pg_command_##_command = {\
        .id = _id,\
        .name = _name,\
        .subcommand_count = sizeof(pg_command_##_command##_subcommand_table) / sizeof(PGSubcommand), \
        .subcommand_table = pg_command_##_command##_subcommand_table\
    }

#endif