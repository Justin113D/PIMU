#include "connection.h"

#include <string.h>
#include <stdlib.h>

//--------------------------------------------------------------------+
// Utilities
//--------------------------------------------------------------------+

static void reverse(uint8_t* src, uint8_t* dst, uint8_t size)
{
    for (int i = 0; i < size; i++)
    {
        dst[i] = src[size - 1 - i];
    }
}

void ltk_xor(uint8_t* a, const uint8_t* b, uint8_t* dst)
{
    for(int i = 0; i < LTK_SIZE; i++)
    {
        dst[i] = a[i] ^ b[i];
    }
}

void update_aes(PGConnection* connection)
{
    AES_EncryptInit(&connection->aes_context_encrypt, connection->ltk_bluetooth);
    AES_DecryptInit(&connection->aes_decrypt_context, connection->ltk_bluetooth);

}

//--------------------------------------------------------------------+
// Getters and setters
//--------------------------------------------------------------------+

void pg_connection_set_device_address(PGConnection* connection, uint8_t *new_device_address)
{
    memcpy(connection->device_address, new_device_address, ADDRESS_SIZE);
}

uint8_t *pg_connection_get_device_address(PGConnection* connection)
{
    return connection->device_address;
}

void pg_connection_get_device_address_rev(PGConnection* connection, uint8_t *dst_device_address)
{
    reverse(connection->device_address, dst_device_address, ADDRESS_SIZE);
}

//--------------------------------------------------------------------+

void pg_connection_set_host_address(PGConnection* connection, uint8_t* new_host_address)
{
    memcpy(connection->host_address, new_host_address, ADDRESS_SIZE);
}

uint8_t *pg_connection_get_host_address(PGConnection* connection)
{
    return connection->host_address;
}

void pg_connection_set_host_address_rev(PGConnection* connection, uint8_t* new_host_address)
{
    reverse(new_host_address, connection->host_address, ADDRESS_SIZE);
}

void pg_connection_get_host_address_rev(PGConnection* connection, uint8_t* dst_host_address)
{
    reverse(connection->host_address, dst_host_address, ADDRESS_SIZE);
}

//--------------------------------------------------------------------+

static void set_bluetooth_address_count(PGConnection* connection, uint8_t count)
{
    if(count > connection->bluetooth_address_count)
    {
        if(connection->bluetooth_addresses != NULL)
        {
            free(connection->bluetooth_addresses);
        }

        connection->bluetooth_addresses = malloc(6 * count);
    }

    connection->bluetooth_address_count = count;
}

void pg_connection_set_bluetooth_addresses(PGConnection* connection, uint8_t *new_bluetooth_addresses, uint8_t count)
{
    set_bluetooth_address_count(connection, count);
    memcpy(connection->bluetooth_addresses, new_bluetooth_addresses, count * 6);
}

void pg_connection_set_bluetooth_addresses_rev(PGConnection* connection, uint8_t *new_bluetooth_addresses, uint8_t count)
{
    set_bluetooth_address_count(connection, count);
    for(int i = 0; i < count; i++)
    {
        uint8_t* src = &new_bluetooth_addresses[i * 6];
        uint8_t* dst = &connection->bluetooth_addresses[i * 6];
        reverse(src, dst, ADDRESS_SIZE);
    }
}

uint8_t pg_connection_get_bluetooth_address_count(PGConnection* connection)
{
    return connection->bluetooth_address_count;
}

uint8_t* pg_connection_get_bluetooth_addresses(PGConnection* connection)
{
    return connection->bluetooth_addresses;
}

void pg_connection_get_bluetooth_addresses_rev(PGConnection* connection, uint8_t* dst_bluetooth_addresses)
{
    for(int i = 0; i < connection->bluetooth_address_count; i++)
    {
        uint8_t* src = &connection->bluetooth_addresses[i * 6];
        uint8_t* dst = &dst_bluetooth_addresses[i * 6];
        reverse(src, dst, ADDRESS_SIZE);
    }
}

//--------------------------------------------------------------------+

void pg_connection_set_ltk_host(PGConnection* connection, uint8_t* ltk)
{
    memcpy(connection->ltk_host, ltk, LTK_SIZE);
    ltk_xor(connection->ltk_host, connection->ltk_device, connection->ltk_bluetooth);
    update_aes(connection);
}

void pg_connection_set_ltk_host_rev(PGConnection* connection, uint8_t* ltk)
{
    reverse(connection->ltk_host, ltk, LTK_SIZE);
    ltk_xor(connection->ltk_host, connection->ltk_device, connection->ltk_bluetooth);
    update_aes(connection);
}

uint8_t* pg_connection_get_ltk_host(PGConnection* connection)
{
    return connection->ltk_host;
}

void pg_connection_get_ltk_host_rev(PGConnection* connection, uint8_t* dst_ltk)
{
    reverse(connection->ltk_host, dst_ltk, LTK_SIZE);
}


const uint8_t* pg_connection_get_ltk_device(PGConnection* connection)
{
    return connection->ltk_device;
}

void pg_connection_get_ltk_device_rev(PGConnection* connection, uint8_t* dst_ltk)
{
    reverse((uint8_t*)connection->ltk_device, dst_ltk, LTK_SIZE);
}


void pg_connection_set_ltk_bluetooth(PGConnection* connection, uint8_t* ltk)
{
    memcpy(connection->ltk_bluetooth, ltk, LTK_SIZE);
    ltk_xor(connection->ltk_bluetooth, connection->ltk_device, connection->ltk_host);
    update_aes(connection);
}

void pg_connection_set_ltk_bluetooth_rev(PGConnection* connection, uint8_t* ltk)
{
    reverse(connection->ltk_bluetooth, ltk, LTK_SIZE);
    ltk_xor(connection->ltk_bluetooth, connection->ltk_device, connection->ltk_host);
    update_aes(connection);
}

uint8_t* pg_connection_get_ltk_bluetooth(PGConnection* connection)
{
    return connection->ltk_bluetooth;
}

void pg_connection_get_ltk_bluetooth_rev(PGConnection* connection, uint8_t* dst_ltk)
{
    reverse(connection->ltk_bluetooth, dst_ltk, LTK_SIZE);
}


AES_CTX* pg_connection_get_aes_context_encrypt(PGConnection* connection)
{
    return &connection->aes_context_encrypt;
}

AES_CTX* pg_connection_get_aes_context_decrypt(PGConnection* connection)
{
    return &connection->aes_decrypt_context;
}