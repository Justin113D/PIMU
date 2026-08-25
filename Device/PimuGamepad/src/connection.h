#ifndef PD_CONNECTION_H_
#define PD_CONNECTION_H_

#include <stdint.h>

#include "aes.h"

#define ADDRESS_SIZE 6
#define LTK_SIZE 16

typedef struct
{
    uint8_t device_address[ADDRESS_SIZE];
    uint8_t host_address[ADDRESS_SIZE];
    uint8_t* bluetooth_addresses;
    uint8_t bluetooth_address_count;

    uint8_t ltk_bluetooth[LTK_SIZE];
    uint8_t ltk_host[LTK_SIZE];
    uint8_t ltk_device[LTK_SIZE];

    AES_CTX aes_context_encrypt;
    AES_CTX aes_decrypt_context;
} PGConnection;

//--------------------------------------------------------------------+
// Address functions
//--------------------------------------------------------------------+

void pg_connection_set_device_address(PGConnection* connection, uint8_t *new_device_address);
uint8_t *pg_connection_get_device_address(PGConnection* connection);
void pg_connection_get_device_address_rev(PGConnection* connection, uint8_t *dst_device_address);

void pg_connection_set_host_address(PGConnection* connection, uint8_t* new_host_address);
uint8_t* pg_connection_get_host_address(PGConnection* connection);
void pg_connection_set_host_address_rev(PGConnection* connection, uint8_t* new_host_address);
void pg_connection_get_host_address_rev(PGConnection* connection, uint8_t* dst_host_address);

void pg_connection_set_bluetooth_addresses(PGConnection* connection, uint8_t *new_bluetooth_addresses, uint8_t count);
void pg_connection_set_bluetooth_addresses_rev(PGConnection* connection, uint8_t *new_bluetooth_addresses, uint8_t count);
uint8_t pg_connection_get_bluetooth_address_count(PGConnection* connection);
uint8_t* pg_connection_get_bluetooth_addresses(PGConnection* connection);
void pg_connection_get_bluetooth_addresses_rev(PGConnection* connection, uint8_t* dst_bluetooth_addresses);

//--------------------------------------------------------------------+
// LTK functions
//--------------------------------------------------------------------+

void pg_connection_set_ltk_host(PGConnection* connection, uint8_t* ltk);
void pg_connection_set_ltk_host_rev(PGConnection* connection, uint8_t* ltk);
uint8_t* pg_connection_get_ltk_host(PGConnection* connection);
void pg_connection_get_ltk_host_rev(PGConnection* connection, uint8_t* dst_ltk);

const uint8_t* pg_connection_get_ltk_device(PGConnection* connection);
void pg_connection_get_ltk_device_rev(PGConnection* connection, uint8_t* dst_ltk);

void pg_connection_set_ltk_bluetooth(PGConnection* connection, uint8_t* ltk);
void pg_connection_set_ltk_bluetooth_rev(PGConnection* connection, uint8_t* ltk);
uint8_t* pg_connection_get_ltk_bluetooth(PGConnection* connection);
void pg_connection_get_ltk_bluetooth_rev(PGConnection* connection, uint8_t* dst_ltk);

AES_CTX* pg_connection_get_aes_context_encrypt(PGConnection* connection);
AES_CTX* pg_connection_get_aes_context_decrypt(PGConnection* connection);

#endif