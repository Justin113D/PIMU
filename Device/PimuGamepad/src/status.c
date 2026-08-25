#include "status.h"

#include <stdlib.h>

void pg_status_set_owner(PGStatus* status, PimuGamepad* gamepad)
{
    status->owner = gamepad;
}

//--------------------------------------------------------------------+

void pg_status_set_usb_enabled(PGStatus* status, bool enabled)
{
    status->is_usb_enabled = enabled;
}

bool pg_status_get_usb_enabled(PGStatus* status)
{
    return status->is_usb_enabled;
}

//--------------------------------------------------------------------+

void pg_status_set_player_leds_changed_cb(PGStatus* status, PGPlayerLEDsChangedCB callback)
{
    status->set_player_leds_changed_cb = callback;
}

static void unvoke_player_leds_changed(PGStatus* status)
{
    if(status->set_player_leds_changed_cb != NULL)
    {
        status->set_player_leds_changed_cb(
            status->owner, 
            status->player_leds_mask,
            status->player_leds_flashing
        );
    }
}

void pg_status_set_player_leds_mask(PGStatus* status, uint8_t new_mask)
{
    if(status->player_leds_mask != new_mask)
    {
        status->player_leds_mask = new_mask;
        unvoke_player_leds_changed(status);
    }
}

uint8_t pg_status_get_player_leds_mask(PGStatus* status)
{
    return status->player_leds_mask;
}

void pg_status_set_player_leds_flashing(PGStatus* status, bool enabled)
{
    if(status->player_leds_flashing != enabled)
    {
        status->player_leds_flashing = enabled;
        unvoke_player_leds_changed(status);
    }
}

bool pg_status_get_player_leds_flashing(PGStatus* status)
{
    return status->player_leds_flashing;
}

//--------------------------------------------------------------------+

void pg_status_set_feature_mask(PGStatus* status, uint8_t mask)
{
    status->feature_mask = mask;
}

uint8_t pg_status_get_feature_mask(PGStatus* status)
{
    return status->feature_mask;
}

void pg_status_set_feature_enabled(PGStatus* status, uint8_t enabled)
{
    status->feature_enabled = enabled;
}

uint8_t pg_status_get_feature_enabled(PGStatus* status)
{
    return status->feature_enabled;
}

uint8_t pg_status_get_feature_active(PGStatus* status)
{
    return status->feature_enabled & status->feature_mask;
}

//--------------------------------------------------------------------+

void pg_status_set_hid_reports_enabled(PGStatus* status, bool enabled)
{
    status->hid_reports_enabled = enabled;
}

bool pg_status_get_hid_reports_enabled(PGStatus* status)
{
    return status->hid_reports_enabled;
}

void pg_status_set_report_id(PGStatus* status, uint8_t new_report_id)
{
    if(new_report_id != 5 && new_report_id != 9)
    {
        return;
    }

    status->report_id = new_report_id;
}

uint8_t pg_status_get_report_id(PGStatus* status)
{
    return status->report_id;
}