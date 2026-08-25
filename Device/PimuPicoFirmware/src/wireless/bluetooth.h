#ifndef PPF_BLUETOOTH_H_
#define PPF_BLUETOOTH_H_

#include <stdbool.h>

void ppf_bluetooth_init(void);
void ppf_bluetooth_set_pairing_mode(bool enabled);
bool ppf_bluetooth_get_pairing_mode(void);
void ppf_bluetooth_forget_all_devices(void);

#endif
