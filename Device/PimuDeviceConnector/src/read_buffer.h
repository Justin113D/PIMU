#ifndef PDC_READ_BUFFER_H_
#define PDC_READ_BUFFER_H_

#include <stdint.h>
#include <stdbool.h>

#include "message.h"

typedef struct
{
    uint16_t current_position;
    bool is_escaped;
    uint8_t buffer[sizeof(PDCMessageCrc)];
    PDCMessage deserialized_message;
} PDCReadBuffer;

bool pdc_read_buffer_deserialize_byte(PDCReadBuffer *connector, uint8_t value);
void pdc_read_buffer_reset(PDCReadBuffer *connector);

#endif