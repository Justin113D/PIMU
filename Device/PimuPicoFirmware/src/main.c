#pragma GCC optimize("O0")

#include "pico/stdlib.h"

#include "bsp/board.h"

#include "config.h"
#include "debug.h"
#include "interfaces.h"
#include "pico_utils.h"

#include "device.h"
#include "wireless/hci.h"

#ifdef BLUETOOTH_ENABLED
#include "pico/cyw43_arch.h"
#endif


int main(void)
{
    ppf_config_load();

    ppf_debug_init();
    PPF_DEBUG_BLANK(0, "================== Initialized ==================");
    PPF_DEBUG_BLANK(0, "Running: PIMU (Switch 2 pro controller Simulator)");
    PPF_DEBUG_BLANK(0, "");

    ppf_interfaces_init();
    ppf_hci_init();
    ppf_pico_led_init();

    bool prev_led_state = false;
    while (true)
    {
        ppf_hci_poll();

        bool led_on = (time_us_32() % 1000000) > 500000;
        if (prev_led_state != led_on)
        {
            ppf_pico_set_led(led_on);
            prev_led_state = led_on;
        }

        ppf_config_poll_save();
    }

    return PICO_OK;
}
