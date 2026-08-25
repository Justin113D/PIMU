#ifndef PG_STATUS_H_
#define PG_STATUS_H_

#include <stdint.h>
#include <stdbool.h>
#include "pimu_gamepad.h"

typedef struct
{
    PimuGamepad* owner;

    bool is_usb_enabled;

    uint8_t player_leds_mask;
    bool player_leds_flashing;
    PGPlayerLEDsChangedCB set_player_leds_changed_cb;

    uint8_t feature_mask;
    uint8_t feature_enabled;
    bool hid_reports_enabled;
    uint8_t report_id;
} PGStatus;

void pg_status_set_owner(PGStatus* status, PimuGamepad* gamepad);

void pg_status_set_usb_enabled(PGStatus* status, bool enabled);
bool pg_status_get_usb_enabled(PGStatus* status);

void pg_status_set_player_leds_mask(PGStatus* status, uint8_t new_mask);
uint8_t pg_status_get_player_leds_mask(PGStatus* status);
void pg_status_set_player_leds_flashing(PGStatus* status, bool enabled);
bool pg_status_get_player_leds_flashing(PGStatus* status);
void pg_status_set_player_leds_changed_cb(PGStatus* status, PGPlayerLEDsChangedCB callback);

void pg_status_set_feature_mask(PGStatus* status, uint8_t mask);
uint8_t pg_status_get_feature_mask(PGStatus* status);
void pg_status_set_feature_enabled(PGStatus* status, uint8_t enabled);
uint8_t pg_status_get_feature_enabled(PGStatus* status);
uint8_t pg_status_get_feature_active(PGStatus* status);

void pg_status_set_hid_reports_enabled(PGStatus* status, bool enabled);
bool pg_status_get_hid_reports_enabled(PGStatus* status);
void pg_status_set_report_id(PGStatus* status, uint8_t new_report_id);
uint8_t pg_status_get_report_id(PGStatus* status);

#endif