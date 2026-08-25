#include "command.h"

// Source - 2026-08-14
// https://github.com/ndeadly/switch2_controller_research/blob/master/commands.md#command-0x15---bluetooth-pairing

//--------------------------------------------------------------------+
// Exchange address
//--------------------------------------------------------------------+

typedef PACKED_STRUCT
{
    uint8_t unknown;
    uint8_t count;
    uint8_t addresses[];
} ExchangeAddressRequest;

typedef PACKED_STRUCT
{
    uint8_t unknown[2];
    uint8_t count;
    uint8_t address[6];
} ExchangeAddressResponse;

PG_SUBCOMMAND_CALLBACK_DEF(bluetooth_pairing, exchange_address)
{
    PG_SUBCOMMAND_REQUEST_SETUP(ExchangeAddressRequest);
    PG_SUBCOMMAND_RESPONSE_SETUP(ExchangeAddressResponse);

    uint16_t minRequest_size = sizeof(ExchangeAddressRequest) + request->count * 6;
    if(minRequest_size > request_head->data_length)
    {
        return PG_SUBCOMMAND_RETURN_CODE_REQUEST_TOO_SMALL;
    }

    pg_connection_set_bluetooth_addresses_rev(&data->connection, request->addresses, request->count);

    response->unknown[0] = 1;
    response->unknown[1] = 0;
    response->count = 1;
    pg_connection_get_device_address_rev(&data->connection, response->address);

    return PG_SUBCOMMAND_RETURN_CODE_OK;
}


//--------------------------------------------------------------------+
// Confirm LTK
//--------------------------------------------------------------------+

#define LTK_CHALLENGE_SIZE 16

typedef PACKED_STRUCT
{
    uint8_t unknown;
    uint8_t challenge[LTK_CHALLENGE_SIZE];
} ConfirmLTKRequest;

typedef PACKED_STRUCT
{
    uint8_t unknown;
    uint8_t response[LTK_CHALLENGE_SIZE];
} ConfirmLTKResponse;

static void reverse_challenge(uint8_t* challenge)
{
    for (int i = 0; i < (LTK_CHALLENGE_SIZE / 2); i++)
    {
        uint8_t tmp = challenge[i];
        challenge[i] = challenge[LTK_CHALLENGE_SIZE - 1 - i];
        challenge[LTK_CHALLENGE_SIZE - 1 - i] = tmp;
    }
}

PG_SUBCOMMAND_CALLBACK_DEF(bluetooth_pairing, confirm_ltk)
{
    PG_SUBCOMMAND_REQUEST_SETUP(ConfirmLTKRequest);
    PG_SUBCOMMAND_RESPONSE_SETUP(ConfirmLTKResponse);

    reverse_challenge(request->challenge);

    AES_Encrypt(
        pg_connection_get_aes_context_encrypt(&data->connection), 
        request->challenge, 
        LTK_CHALLENGE_SIZE, 
        response->response
    );

    reverse_challenge(response->response);
    // not actually needed, but doing it anyway to preserve in case of... something
    reverse_challenge(request->challenge);
    

    response->unknown = 1;

    return PG_SUBCOMMAND_RETURN_CODE_OK;
}


//--------------------------------------------------------------------+
// Finalize
//--------------------------------------------------------------------+

typedef PACKED_STRUCT
{
    uint8_t unknown;
} FinalizeRequest;

typedef PACKED_STRUCT
{
    uint8_t unknown;
} FinalizeResponse;

PG_SUBCOMMAND_CALLBACK_DEF(bluetooth_pairing, finalize)
{
    PG_SUBCOMMAND_REQUEST_SETUP(FinalizeRequest);
    PG_SUBCOMMAND_RESPONSE_SETUP(FinalizeResponse);

    response->unknown = 1;

    return PG_SUBCOMMAND_RETURN_CODE_OK;
}


//--------------------------------------------------------------------+
// Exchange LTK
//--------------------------------------------------------------------+

typedef PACKED_STRUCT
{
    uint8_t unknown;
    uint8_t host_key[16];
} ExchangeLTKRequest;

typedef PACKED_STRUCT
{
    uint8_t unknown;
    uint8_t device_key[16];
} ExchangeLTKResponse;

PG_SUBCOMMAND_CALLBACK_DEF(bluetooth_pairing, exchange_ltk)
{
    PG_SUBCOMMAND_REQUEST_SETUP(ExchangeLTKRequest);
    PG_SUBCOMMAND_RESPONSE_SETUP(ExchangeLTKResponse);

    pg_connection_set_ltk_host_rev(&data->connection, request->host_key);

    response->unknown = 1;
    pg_connection_get_ltk_device_rev(&data->connection, response->device_key);

    return PG_SUBCOMMAND_RETURN_CODE_OK;
}


//--------------------------------------------------------------------+
// Command setup
//--------------------------------------------------------------------+

PG_SUBCOMMAND_TABLE(bluetooth_pairing) = {
    PG_SUBCOMMAND_NONE,
    PG_SUBCOMMAND(1, "Exchange address", bluetooth_pairing, exchange_address),
    PG_SUBCOMMAND(2, "Confirm LTK", bluetooth_pairing, confirm_ltk),
    PG_SUBCOMMAND(3, "Finalize", bluetooth_pairing, finalize),
    PG_SUBCOMMAND(4, "Exchange LTK", bluetooth_pairing, exchange_ltk),
};

PG_COMMAND(21, bluetooth_pairing, "Bluetooth pairing");
