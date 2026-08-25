#include "read_buffer.h"

#include <string.h>

#include "message.h"
#include "crc.h"

static bool process_input(PDCReadBuffer *buffer)
{
    if (buffer->current_position < (sizeof(PDCMessageHeader) + sizeof(PDCMessageCrcHeader)))
    {
        return false;
    }

    PDCMessageCrc *crc_message = (PDCMessageCrc *)buffer->buffer;

    uint16_t read_packet_size = buffer->current_position - sizeof(PDCMessageCrcHeader);
    uint32_t crc = crc32((uint8_t *)&crc_message->message, read_packet_size);

    if (crc != crc_message->header.crc)
    {
        return false;
    }

    uint16_t packet_size = pdc_message_get_size(&crc_message->message);
    if (read_packet_size != packet_size)
    {
        return false;
    }

    memcpy(
        (uint8_t *)&buffer->deserialized_message,
        (uint8_t *)&crc_message->message,
        packet_size);

    return true;
}

bool pdc_read_buffer_deserialize_byte(PDCReadBuffer *buffer, uint8_t value)
{
    bool result = false;

    if (buffer->is_escaped)
    {
        buffer->is_escaped = false;
    serial_store:
        buffer->buffer[buffer->current_position] = value;
        buffer->current_position = (buffer->current_position + 1) % sizeof(buffer->buffer);
    }
    else
    {
        switch (value)
        {
        case PDC_MESSAGE_CHAR_TERMINATION:
            result = process_input(buffer);
            buffer->current_position = 0;
            break;
        case PDC_MESSAGE_CHAR_ESCAPE:
            buffer->is_escaped = true;
            break;
        default:
            goto serial_store;
        }
    }

    return result;
}

void pdc_read_buffer_reset(PDCReadBuffer *connector)
{
    connector->current_position = 0;
    connector->is_escaped = false;
}