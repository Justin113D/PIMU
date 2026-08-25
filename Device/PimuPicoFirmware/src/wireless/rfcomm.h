#ifndef PPF_RFCOMM_H_
#define PPF_RFCOMM_H_

#include <stdint.h>
#include <stdbool.h>

void ppf_rfcomm_init(void);
bool ppf_rfcomm_is_channel_open(void);
bool ppf_rfcomm_send_data(uint8_t *data, uint16_t data_size);

#endif