#ifndef PPF_ITF_UART_H_
#define PPF_ITF_UART_H_

#include "pimu_device_connector.h"

extern PimuDeviceConnector* ppf_device_connector_uart;

void ppf_itf_uart_init(void);
void ppf_itf_uart_poll(void);

#endif