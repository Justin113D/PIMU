#ifndef PPF_ITF_CONNECTOR_H_
#define PPF_ITF_CONNECTOR_H_

#include "pimu_device_connector.h"

typedef enum
{
    PPF_CONNECTOR_TYPE_NONE,  
    PPF_CONNECTOR_TYPE_UART,
    PPF_CONNECTOR_TYPE_BLUETOOTH,
} PPF_CONNECTOR_TYPE;

void ppf_itf_connector_set_common_callbacks(PimuDeviceConnector *connector);

void ppf_itf_connector_send_player_leds(uint8_t player_leds_mask, bool flashing);
void ppf_itf_connector_send_debug_string(uint8_t features, char* string);
void ppf_itf_connector_send_debug_data(uint8_t features, uint8_t* data, uint16_t data_size);

void ppf_itf_connector_read_bytes(PPF_CONNECTOR_TYPE type, uint8_t *data, uint16_t data_size);
bool ppf_itf_connector_check_is_connected(void);
#endif