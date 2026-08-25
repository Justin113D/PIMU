#include "command.h"

// Source - 2026-08-14
// https://github.com/ndeadly/switch2_controller_research/blob/master/commands.md#command-0x0a---vibration

//--------------------------------------------------------------------+
// Play sample
//--------------------------------------------------------------------+

typedef PACKED_STRUCT
{
    uint8_t sample_id;
    uint8_t unknown[3];
} PlaySampleRequest;

#define PG_VIBRATION_SAMPLE_ID_SILENCE 0
#define PG_VIBRATION_SAMPLE_ID_LOW_FREQ_BUZZ 1
#define PG_VIBRATION_SAMPLE_ID_HIGH_FREQ_BUZZ 2
#define PG_VIBRATION_SAMPLE_ID_SOFT_CLICK 3
#define PG_VIBRATION_SAMPLE_ID_HIGHER_FREQ_BEEP 4
#define PG_VIBRATION_SAMPLE_ID_HARD_CLICK 5
#define PG_VIBRATION_SAMPLE_ID_SHORT_BEEP 6
#define PG_VIBRATION_SAMPLE_ID_SHORT_HIGH_BEEP 7

PG_SUBCOMMAND_CALLBACK_DEF(vibration, play_sample)
{
    // No logic for vibrations right now
    return PG_SUBCOMMAND_RETURN_CODE_OK;
}


//--------------------------------------------------------------------+
// Send data
//--------------------------------------------------------------------+

typedef PACKED_STRUCT
{
    uint8_t unknown[20];
} SendDataRequest;

PG_SUBCOMMAND_CALLBACK_DEF(vibration, send_data)
{
    // No logic for vibrations right now
    return PG_SUBCOMMAND_RETURN_CODE_OK;
}


//--------------------------------------------------------------------+
// Command setup
//--------------------------------------------------------------------+

PG_SUBCOMMAND_TABLE(vibration) = {
    PG_SUBCOMMAND_NONE,
    PG_SUBCOMMAND_UNKNOWN(1),
    PG_SUBCOMMAND(2, "Play sample", vibration, play_sample),
    PG_SUBCOMMAND_UNKNOWN(3),
    PG_SUBCOMMAND_UNKNOWN(4),
    PG_SUBCOMMAND_UNKNOWN(5),
    PG_SUBCOMMAND_UNKNOWN(6),
    PG_SUBCOMMAND_UNKNOWN(7),
    PG_SUBCOMMAND(8, "Send data", vibration, send_data),
    PG_SUBCOMMAND_UNKNOWN(9),
};

PG_COMMAND(10, vibration, "Vibration");