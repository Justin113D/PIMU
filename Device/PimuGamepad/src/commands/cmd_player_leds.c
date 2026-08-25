#include "command.h"

// Source - 2026-08-14
// https://github.com/ndeadly/switch2_controller_research/blob/master/commands.md#command-0x09---player-leds

//--------------------------------------------------------------------+
// Set player 1
//--------------------------------------------------------------------+

PG_SUBCOMMAND_CALLBACK_DEF(player_leds, set_player_1)
{
    pg_status_set_player_leds_mask(&data->status, 0x01);
    return PG_SUBCOMMAND_RETURN_CODE_OK;
}


//--------------------------------------------------------------------+
// Set player 2
//--------------------------------------------------------------------+

PG_SUBCOMMAND_CALLBACK_DEF(player_leds, set_player_2)
{
    pg_status_set_player_leds_mask(&data->status, 0x02);
    return PG_SUBCOMMAND_RETURN_CODE_OK;
}


//--------------------------------------------------------------------+
// Set player 3
//--------------------------------------------------------------------+

PG_SUBCOMMAND_CALLBACK_DEF(player_leds, set_player_3)
{
    pg_status_set_player_leds_mask(&data->status, 0x04);
    return PG_SUBCOMMAND_RETURN_CODE_OK;
}


//--------------------------------------------------------------------+
// Set player 4
//--------------------------------------------------------------------+

PG_SUBCOMMAND_CALLBACK_DEF(player_leds, set_player_4)
{
    pg_status_set_player_leds_mask(&data->status, 0x08);
    return PG_SUBCOMMAND_RETURN_CODE_OK;
}


//--------------------------------------------------------------------+
// Set all
//--------------------------------------------------------------------+

PG_SUBCOMMAND_CALLBACK_DEF(player_leds, set_all)
{
    pg_status_set_player_leds_mask(&data->status, 0x0F);
    return PG_SUBCOMMAND_RETURN_CODE_OK;
}


//--------------------------------------------------------------------+
// Set none
//--------------------------------------------------------------------+

PG_SUBCOMMAND_CALLBACK_DEF(player_leds, set_none)
{
    pg_status_set_player_leds_mask(&data->status, 0x00);
    return PG_SUBCOMMAND_RETURN_CODE_OK;
}


//--------------------------------------------------------------------+
// Set pattern
//--------------------------------------------------------------------+

typedef PACKED_STRUCT
{
    uint8_t led_bitmask;
    uint8_t unknown[3];
} SetPatternRequest;

PG_SUBCOMMAND_CALLBACK_DEF(player_leds, set_pattern)
{
    PG_SUBCOMMAND_REQUEST_SETUP(SetPatternRequest);

    pg_status_set_player_leds_mask(&data->status, request->led_bitmask & 0x0F);
    
    return PG_SUBCOMMAND_RETURN_CODE_OK;
}


//--------------------------------------------------------------------+
// Flash
//--------------------------------------------------------------------+

typedef PACKED_STRUCT
{
    uint8_t enable;
    uint8_t unknown[7];
} FlashRequest;

PG_SUBCOMMAND_CALLBACK_DEF(player_leds, flash)
{
    PG_SUBCOMMAND_REQUEST_SETUP(FlashRequest);

    pg_status_set_player_leds_flashing(&data->status, request->enable);

    return PG_SUBCOMMAND_RETURN_CODE_OK;
}


//--------------------------------------------------------------------+
// Command setup
//--------------------------------------------------------------------+

PG_SUBCOMMAND_TABLE(player_leds) = {
    PG_SUBCOMMAND_NONE,
    PG_SUBCOMMAND(1, "Set player 1", player_leds, set_player_1),
    PG_SUBCOMMAND(2, "Set player 2", player_leds, set_player_2),
    PG_SUBCOMMAND(3, "Set player 3", player_leds, set_player_3),
    PG_SUBCOMMAND(4, "Set player 4", player_leds, set_player_4),
    PG_SUBCOMMAND(5, "Set all", player_leds, set_all),
    PG_SUBCOMMAND(6, "Set none", player_leds, set_none),
    PG_SUBCOMMAND(7, "Set pattern", player_leds, set_pattern),
    PG_SUBCOMMAND(8, "Flash", player_leds, flash),
};

PG_COMMAND(9, player_leds, "Player LEDs");