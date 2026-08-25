#include "bluetooth.h"

#ifdef BLUETOOTH_ENABLED

#include "btstack.h"

static bool pairing_mode_enabled;

void ppf_bluetooth_init(void)
{
    gap_ssp_set_io_capability(SSP_IO_CAPABILITY_DISPLAY_YES_NO);
    gap_set_local_name("PIMU Device");

    // Limited discoverable mode; Peripheral; Gamepad
    gap_set_class_of_device(0x000508);
}

void ppf_bluetooth_set_pairing_mode(bool enabled)
{
    pairing_mode_enabled = enabled;
    gap_discoverable_control(enabled);
    gap_ssp_set_auto_accept(enabled);
}

bool ppf_bluetooth_get_pairing_mode(void)
{
    return pairing_mode_enabled;
}

void ppf_bluetooth_forget_all_devices(void)
{
    gap_delete_all_link_keys();
}

#else

void ppf_bluetooth_init(void) { }
void ppf_bluetooth_set_pairing_mode(bool enabled) { }
bool ppf_bluetooth_get_pairing_mode(void) { return false; }
void ppf_bluetooth_forget_all_devices(void) { }

#endif