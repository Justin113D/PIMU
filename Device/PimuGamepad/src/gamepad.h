#ifndef PG_GAMEPAD_H_
#define PG_GAMEPAD_H_

#include "pimu_gamepad.h"
#include "connection.h"
#include "status.h"

typedef struct
{
    PimuGamepadDeviceColors device_colors;
    PGConnection connection;
    PGStatus status;
} PGGamepadData;

#endif