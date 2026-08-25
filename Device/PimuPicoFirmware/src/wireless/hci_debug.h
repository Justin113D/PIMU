#ifndef PPF_HCI_DEBUG_H_
#define PPF_HCI_DEBUG_H_

#define HCI_DEBUG_ENABLED 1

#include <stdint.h>

void ppf_hci_debug_packet(const char* origin, uint8_t *packet);

#endif