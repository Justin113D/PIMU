#include "hci.h"

#ifdef BLUETOOTH_ENABLED

#include <stdbool.h>

#include "pico/cyw43_arch.h"
#include "btstack.h"

#include "config.h"
#include "debug.h"
#include "device.h"
#include "interfaces.h"

#include "rfcomm.h"
#include "bluetooth.h"
#include "hci_debug.h"

static bool initialized;
static bool pairing_mode_enabled;

void set_pairing_mode(bool enabled) {
    pairing_mode_enabled = enabled;
    gap_discoverable_control(enabled);
    gap_ssp_set_auto_accept(enabled);
}

bool get_pairing_mode() {
    return pairing_mode_enabled;
}


static btstack_packet_callback_registration_t hci_event_callback_registration;
static void handle_packet(uint8_t packet_type, uint16_t __unused channel, uint8_t *packet, uint16_t __unused size)
{
    if (packet_type != HCI_EVENT_PACKET)
    {
        PPF_DEBUG_ERROR_BLUETOOTH("[HCI] Different packet type: %i", packet_type);
        return;
    }

    ppf_hci_debug_packet("HCI", packet);

    uint8_t event = hci_event_packet_get_type(packet);
    bd_addr_t device_address;

    switch (event)
    {
    case BTSTACK_EVENT_STATE:
        HCI_STATE hci_state = btstack_event_state_get_state(packet);

        if (hci_state == HCI_STATE_WORKING)
        {
            bd_addr_t local_addr;
            gap_local_bd_addr(local_addr);
            pimu_gamepad_set_device_address(ppf_gamepad, local_addr);

            set_pairing_mode(true);
        }
        break;

    case GAP_EVENT_PAIRING_COMPLETE:
        set_pairing_mode(false);
        break;

    case HCI_EVENT_DISCONNECTION_COMPLETE:
        set_pairing_mode(true);
        break;
    }
}

void ppf_hci_init(void)
{
    if(ppf_config_get_bluetooth_disabled())
    {
        return;
    }

    initialized = true;

    cyw43_arch_init();
    
    hci_event_callback_registration.callback = &handle_packet;
    hci_add_event_handler(&hci_event_callback_registration);

    l2cap_init();

    ppf_rfcomm_init();
    ppf_bluetooth_init();

    hci_power_control(HCI_POWER_ON);
}

void ppf_hci_poll(void)
{
    if(!initialized)
    {
        return;
    }
    
    pimu_device_connector_poll(ppf_device_connector_bluetooth);
}

#else

void ppf_hci_init(void) { }
void ppf_hci_poll(void) { }

#endif