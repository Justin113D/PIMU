#include "send_buffer.h"

#include "connector.h"
#include <stdlib.h>

void pdc_send_buffer_queue_message(PimuDeviceConnector *connector, PDCSendBuffer *send_buffer, PDCMessage *message)
{
    if (send_buffer->message_queue_count >= PIMO_DEVICE_CONNECTOR_MESSAGE_QUEUE_SIZE)
    {
        pdc_debug_print(connector, PDC_DEBUG_TYPE_ERROR, "Message queue is full!");
        return;
    }

    uint16_t index = (send_buffer->message_queue_offset + send_buffer->message_queue_count) % PIMO_DEVICE_CONNECTOR_MESSAGE_QUEUE_SIZE;
    send_buffer->message_queue[index] = *message;
    send_buffer->message_queue_count++;
}

static void dequeue_message(PDCSendBuffer *send_buffer)
{
    send_buffer->message_queue_count--;
    if (send_buffer->message_queue_count == 0)
    {
        send_buffer->escaped_message_index = -1;
        send_buffer->message_queue_offset = 0;
    }
    else
    {
        send_buffer->message_queue_offset = (send_buffer->message_queue_offset + 1) % PIMO_DEVICE_CONNECTOR_MESSAGE_QUEUE_SIZE;
    }
}

void pdc_send_buffer_poll(PimuDeviceConnector *connector, PDCSendBuffer *send_buffer)
{
    switch (send_buffer->wait_mode)
    {
    case PDC_WAIT_MODE_SENT:
        dequeue_message(send_buffer);
        send_buffer->wait_mode = PDC_WAIT_MODE_NONE;
        break;
    case PDC_WAIT_MODE_WAITING:
        return;
    }

    if (send_buffer->message_queue_count == 0 || send_buffer->send_message_data_cb == NULL)
    {
        return;
    }

    if (send_buffer->escaped_message_index != send_buffer->message_queue_offset)
    {
        pdc_escape_message(&send_buffer->message_queue[send_buffer->message_queue_offset], &send_buffer->escaped_message);
        send_buffer->escaped_message_index = send_buffer->message_queue_offset;
    }

    send_buffer->wait_mode = PDC_WAIT_MODE_WAITING;
    PDC_SEND_MESSAGE_DATA_RETURN_CODE return_code = send_buffer->send_message_data_cb(connector, send_buffer->escaped_message.buffer, send_buffer->escaped_message.size);

    if(return_code != PDC_SEND_MESSAGE_DATA_RETURN_CODE_SUCCESS_WAIT)
    {
        send_buffer->wait_mode = PDC_WAIT_MODE_NONE;
    } 

    if(return_code == PDC_SEND_MESSAGE_DATA_RETURN_CODE_SUCCESS)
    {
        dequeue_message(send_buffer);
    }
}

void pdc_send_buffer_confirm_message_sent(PDCSendBuffer *send_buffer)
{
    if (send_buffer->wait_mode == PDC_WAIT_MODE_WAITING)
    {
        send_buffer->wait_mode = PDC_WAIT_MODE_SENT;
    }
}

void pdc_send_buffer_reset(PDCSendBuffer *send_buffer)
{
    send_buffer->escaped_message_index = -1;
    send_buffer->message_queue_offset = 0;
    send_buffer->message_queue_count = 0;
    send_buffer->wait_mode = PDC_WAIT_MODE_NONE;
}