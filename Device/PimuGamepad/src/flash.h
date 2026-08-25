#ifndef PG_FLASH_H_
#define PG_FLASH_H_

#include <stdint.h>
#include "gamepad.h"

void pg_flash_simulate_read(PGGamepadData* data, uint8_t* output, uint8_t output_length, uint32_t read_address);

#endif