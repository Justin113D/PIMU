#include "rfcomm.h"
#ifdef BLUETOOTH_ENABLED

#include <stdio.h>

#include "pico/stdio.h"
#include "btstack.h"

#include "debug.h"
#include "bluetooth.h"
#include "interface/itf_connector.h"
#include "interface/itf_bluetooth.h"
#include "hci_debug.h"

#define RFCOMM_SERVER_CHANNEL 1
static uint8_t spp_service_buffer[150];
static uint16_t rfcomm_channel_id;

static uint8_t* send_buffer;
static uint16_t send_buffer_size;

static void handle_packet(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size)
{
    
    switch (packet_type)
    {
    case HCI_EVENT_PACKET:
        ppf_hci_debug_packet("RFCOMM", packet);
        
        uint8_t hci_event = hci_event_packet_get_type(packet);
        bd_addr_t device_address;

        switch (hci_event)
        {
        case RFCOMM_EVENT_INCOMING_CONNECTION:
            rfcomm_accept_connection(rfcomm_event_incoming_connection_get_rfcomm_cid(packet));
            break;

        case RFCOMM_EVENT_CHANNEL_OPENED:
            if (!rfcomm_event_channel_opened_get_status(packet))
            {
                rfcomm_channel_id = rfcomm_event_channel_opened_get_rfcomm_cid(packet);
                ppf_bluetooth_set_pairing_mode(false);
            }
            break;
        case RFCOMM_EVENT_CHANNEL_CLOSED:
            rfcomm_channel_id = 0;
            ppf_bluetooth_set_pairing_mode(true);
            pimu_device_connector_disconnect(ppf_device_connector_bluetooth);
            break;
        case RFCOMM_EVENT_CAN_SEND_NOW:
            rfcomm_send(rfcomm_channel_id, send_buffer, send_buffer_size);
            pimu_device_connector_confirm_message_sent(ppf_device_connector_bluetooth);
            break;
        }

    case RFCOMM_DATA_PACKET:
        ppf_itf_connector_read_bytes(PPF_CONNECTOR_TYPE_BLUETOOTH, packet, size);
        break;
    }
}

void ppf_rfcomm_init(void)
{
    rfcomm_init();
    rfcomm_register_service(handle_packet, RFCOMM_SERVER_CHANNEL, 0xffff);

    sdp_init();
    memset(spp_service_buffer, 0, sizeof(spp_service_buffer));
    spp_create_sdp_record(spp_service_buffer, sdp_create_service_record_handle(), RFCOMM_SERVER_CHANNEL, "PIMU Device");
    btstack_assert(de_get_len(spp_service_buffer) <= sizeof(spp_service_buffer));
    sdp_register_service(spp_service_buffer);
}

bool ppf_rfcomm_is_channel_open(void)
{
    return rfcomm_channel_id != 0;
}


bool ppf_rfcomm_send_data(uint8_t *data, uint16_t data_size)
{
    if (!ppf_rfcomm_is_channel_open())
    {
        PPF_DEBUG_ERROR_BLUETOOTH("[RFCOMM] Channel not open!");
        return false;
    }

    send_buffer = data;
    send_buffer_size = data_size;

    rfcomm_request_can_send_now_event(rfcomm_channel_id);
    return true;
}

#else

void ppf_rfcomm_init(void) { };
bool ppf_rfcomm_is_channel_open(void) { };
bool ppf_rfcomm_send_data(uint8_t *data, uint16_t data_size) { return false; };

#endif