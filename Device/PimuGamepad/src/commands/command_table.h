#ifndef PG_COMMAND_TABLES_H_
#define PG_COMMAND_TABLES_H_

#include "command.h"

PG_COMMAND_DEF(nfc);
PG_COMMAND_DEF(flash_memory);
PG_COMMAND_DEF(initialization);
PG_COMMAND_DEF(unknown_4);
PG_COMMAND_DEF(unknown_5);
PG_COMMAND_DEF(unknown_6);
PG_COMMAND_DEF(unknown_7);
PG_COMMAND_DEF(charging_grip);
PG_COMMAND_DEF(player_leds);
PG_COMMAND_DEF(vibration);
PG_COMMAND_DEF(battery);
PG_COMMAND_DEF(feature_select);
PG_COMMAND_DEF(firmware_update);
PG_COMMAND_DEF(unknown_14);
PG_COMMAND_DEF(unknown_15);
PG_COMMAND_DEF(firmware_info);
PG_COMMAND_DEF(unknown_17);
PG_COMMAND_DEF(unknown_18);
PG_COMMAND_DEF(unknown_19);
PG_COMMAND_DEF(unknown_20);
PG_COMMAND_DEF(bluetooth_pairing);
PG_COMMAND_DEF(unknown_22);
PG_COMMAND_DEF(unknown_23);
PG_COMMAND_DEF(unknown_24);

#define PG_COMMAND_NONE NULL
#define PG_COMMAND_REF(_name) &pg_command_##_name

const PGCommand* pg_command_table[] = {
    PG_COMMAND_NONE,
    PG_COMMAND_REF(nfc),
    PG_COMMAND_REF(flash_memory),
    PG_COMMAND_REF(initialization),
    PG_COMMAND_REF(unknown_4),
    PG_COMMAND_REF(unknown_5),
    PG_COMMAND_REF(unknown_6),
    PG_COMMAND_REF(unknown_7),
    PG_COMMAND_REF(charging_grip),
    PG_COMMAND_REF(player_leds),
    PG_COMMAND_REF(vibration),
    PG_COMMAND_REF(battery),
    PG_COMMAND_REF(feature_select),
    PG_COMMAND_REF(firmware_update),
    PG_COMMAND_REF(unknown_14),
    PG_COMMAND_REF(unknown_15),
    PG_COMMAND_REF(firmware_info),
    PG_COMMAND_REF(unknown_17),
    PG_COMMAND_REF(unknown_18),
    PG_COMMAND_REF(unknown_19),
    PG_COMMAND_REF(unknown_20),
    PG_COMMAND_REF(bluetooth_pairing),
    PG_COMMAND_REF(unknown_22),
    PG_COMMAND_REF(unknown_23),
    PG_COMMAND_REF(unknown_24),
};

#endif