#ifndef PDC_CONNECTOR_H_
#define PDC_CONNECTOR_H_

#include "pimu_device_connector.h"

void pdc_debug_print(PimuDeviceConnector* connector, uint8_t features, PDC_DEBUG_TYPE type, char* format, ...);

#endif