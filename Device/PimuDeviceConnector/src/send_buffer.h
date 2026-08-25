#ifndef PDC_SEND_BUFFER_H_
#define PDC_SEND_BUFFER_H_

#include "pimu_device_connector.h"
#include "message.h"

#define PIMO_DEVICE_CONNECTOR_MESSAGE_QUEUE_SIZE 16

typedef enum
{
    PDC_WAIT_MODE_NONE,
    PDC_WAIT_MODE_WAITING,
    PDC_WAIT_MODE_SENT
} PDC_WAIT_MODE;

typedef struct
{
    PDCSendMessageDataCB send_message_data_cb;
    PDC_WAIT_MODE wait_mode;

    uint16_t message_queue_offset;
    uint16_t message_queue_count;
    PDCMessage message_queue[PIMO_DEVICE_CONNECTOR_MESSAGE_QUEUE_SIZE];

    uint16_t escaped_message_index;
    PDCEscapedMessageCrc escaped_message; 
} PDCSendBuffer;

void pdc_send_buffer_queue_message(PimuDeviceConnector* connector, PDCSendBuffer *send_buffer, PDCMessage* message);
void pdc_send_buffer_poll(PimuDeviceConnector* connector, PDCSendBuffer* send_buffer);
void pdc_send_buffer_confirm_message_sent(PDCSendBuffer *send_buffer);
void pdc_send_buffer_reset(PDCSendBuffer *send_buffer);

#endif