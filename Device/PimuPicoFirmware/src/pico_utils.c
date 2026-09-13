#include "pico.h"

#include <stdint.h>

#if defined(PICO_DEFAULT_LED_PIN)
#include "pico/stdlib.h"
#elif defined(CYW43_WL_GPIO_LED_PIN)
#include "pico/cyw43_arch.h"
#include "config.h"
#endif

void ppf_pico_led_init(void) {
#if defined(PICO_DEFAULT_LED_PIN)
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
#elif defined(CYW43_WL_GPIO_LED_PIN)

#if defined(BLUETOOTH_ENABLED)
    if(!ppf_config_get_bluetooth_disabled())
    {
        return;
    }
#endif

    cyw43_arch_init();
#endif
}

void ppf_pico_set_led(bool led_on) {
#if defined(PICO_DEFAULT_LED_PIN)
    gpio_put(PICO_DEFAULT_LED_PIN, led_on);
#elif defined(CYW43_WL_GPIO_LED_PIN)
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);
#endif
}