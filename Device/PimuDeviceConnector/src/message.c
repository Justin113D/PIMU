#include "message.h"

#include "crc.h"

uint16_t pdc_message_get_size(PDCMessage *message)
{
    return sizeof(PDCMessage) - (PDC_MESSAGE_DATA_MAX_SIZE - message->header.data_size);
}

static uint16_t message_get_size_crc(PDCMessageCrc *message)
{
    return sizeof(PDCMessageCrc) - (PDC_MESSAGE_DATA_MAX_SIZE - message->message.header.data_size);
}

static void calculate_crc(PDCMessageCrc *message)
{
    uint16_t message_size = pdc_message_get_size(&message->message);
    message->header.crc = crc32((uint8_t*)&message->message, message_size);
}

uint16_t pdc_escape_message(PDCMessage *message, PDCEscapedMessageCrc* destination)
{
    PDCMessageCrc crc_packet;
    crc_packet.message = *message;
    calculate_crc(&crc_packet);

    // preceding double-terminator 
    // (to ensure that the receiving end identifies this as a new message)
    destination->buffer[0] = PDC_MESSAGE_CHAR_TERMINATION;
    destination->buffer[1] = PDC_MESSAGE_CHAR_TERMINATION;

    destination->size = 2;

    uint8_t *crc_packet_data = (uint8_t *)&crc_packet;
    uint16_t packet_size = message_get_size_crc(&crc_packet);
    for (int i = 0; i < packet_size; i++)
    {
        uint8_t packet_byte = *crc_packet_data;
        crc_packet_data++;

        if (packet_byte == PDC_MESSAGE_CHAR_TERMINATION || packet_byte == PDC_MESSAGE_CHAR_ESCAPE)
        {
            destination->buffer[destination->size] = PDC_MESSAGE_CHAR_ESCAPE;
            destination->size++;
        }

        destination->buffer[destination->size] = packet_byte;
        destination->size++;
    }

    destination->buffer[destination->size] = PDC_MESSAGE_CHAR_TERMINATION;
    destination->size++;
}