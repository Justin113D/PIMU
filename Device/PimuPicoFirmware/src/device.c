#include "device.h"

#include "pico/flash.h"
#include "tusb.h"
#include "bsp/board.h"

#include "pimu_gamepad_input.h"

#include "debug.h"
#include "interface/itf_gamepad.h"
#include "interface/itf_input.h"

//--------------------------------------------------------------------+
// General USB callbacks
//--------------------------------------------------------------------+

void tud_mount_cb(void)
{
    PPF_DEBUG_INFO_USB("[USB] Mounted");
}

void tud_umount_cb(void)
{
    PPF_DEBUG_INFO_USB("[USB] Unmounted");
}

void tud_suspend_cb(bool remote_wakeup_en)
{
    PPF_DEBUG_INFO_USB("[USB] Bus suspended (remote wakeup: %i)", remote_wakeup_en);
}

void tud_resume_cb(void)
{
    PPF_DEBUG_INFO_USB("[USB] Bus resumed");
}

//--------------------------------------------------------------------+
// HID callbacks
//--------------------------------------------------------------------+

uint16_t tud_hid_get_report_cb(
    uint8_t itf, 
    uint8_t report_id, 
    hid_report_type_t report_type, 
    uint8_t __unused *buffer,
     uint16_t __unused reqlen)
{
    //PPF_DEBUG_INFO_USB("[HID] Get Report (%i, %i, %i)", itf, report_id, report_type);
    return 0;
}

void tud_hid_set_report_cb(
    uint8_t itf,
    uint8_t report_id, 
    hid_report_type_t report_type,
    uint8_t const __unused *buffer, 
    uint16_t __unused bufsize)
{
    //PPF_DEBUG_INFO_USB("[HID] Set Report (%i, %i, %i)", itf, report_id, report_type);
}

//--------------------------------------------------------------------+
// Vendor callbacks
//--------------------------------------------------------------------+

bool tud_vendor_control_xfer_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const *transfer_request)
{
    PPF_DEBUG_INFO_USB("[VENDOR] Control transfer (%i, %i; %i, %i, %i)", 
        stage, 
        transfer_request->bRequest,
        transfer_request->bmRequestType_bit.direction, 
        transfer_request->bmRequestType_bit.type,
        transfer_request->bmRequestType_bit.recipient
    );
    
    PGControlTransferRequest request = {
        .direction = transfer_request->bmRequestType_bit.direction,
        .type = transfer_request->bmRequestType_bit.type,
        .recipient = transfer_request->bmRequestType_bit.recipient,
        .stage = stage,
        .request_id = transfer_request->bRequest
    };

    PGControlTransferResponse response;
    PG_CONTROL_TRANSFER_RETURN_CODE return_code = pimu_gamepad_control_transfer_request(ppf_gamepad, &request, &response);

    if(return_code == PG_CONTROL_TRANSFER_RETURN_CODE_TRUE_WITH_DATA)
    {
        void* response_data = NULL;
        
        if(response.data_size > 0)
        {
            response_data = (void*)response.data;
        }

        tud_control_xfer(rhport, transfer_request, response_data, response.data_size);
    }

    return return_code != PG_CONTROL_TRANSFER_RETURN_CODE_FALSE;
}

void tud_vendor_rx_cb(uint8_t idx, const uint8_t *buf, uint16_t bufs) 
{
    PPF_DEBUG_INFO_USB("[VENDOR] Received data (%i, %i)", idx, bufs);
    
    PGCommandRequest request = {
        .data = (uint8_t*)buf,
        .data_size = bufs
    };

    PGCommandResponse response;

    if(pimu_gamepad_command_request(ppf_gamepad, &request, &response))
    {
        tud_vendor_n_write(idx, &response.data, response.data_size);
        tud_vendor_n_write_flush(idx);
    }
}

//--------------------------------------------------------------------+
// HID Input handling
//--------------------------------------------------------------------+

static uint64_t polling_rate = 1000000 / 250;
static absolute_time_t next_report_timestamp;

void hid_poll(void)
{
    if (!tud_mounted())
    {
        return;
    }

    if (absolute_time_diff_us(get_absolute_time(), next_report_timestamp) > 0)
    {
        return;
    }

    ppf_itf_input_update();
    
    if (tud_hid_ready() && pimu_gamepad_get_usb_enabled(ppf_gamepad) && pimu_gamepad_get_feature_mask(ppf_gamepad))
    {
        switch (pimu_gamepad_get_report_id(ppf_gamepad))
        {
        case 9:
            tud_hid_report(9, (uint8_t *)&ppf_gamepad_input_report_9, sizeof(ppf_gamepad_input_report_9));
            break;
        case 5:
            tud_hid_report(5, (uint8_t *)&ppf_gamepad_input_report_5, sizeof(ppf_gamepad_input_report_5));
            break;
        }
    }

    next_report_timestamp = delayed_by_us(get_absolute_time(), polling_rate);
    ppf_gamepad_input_report_5.counter++;
    ppf_gamepad_input_report_9.counter++;
}

//--------------------------------------------------------------------+

void device_init(void)
{
    tusb_init();
    board_init_after_tusb();
    flash_safe_execute_core_init();
}

void device_poll(void)
{
    tud_task();

    if (tud_suspended()) {
        tud_remote_wakeup();
    }

    if (tud_hid_ready()) {
        hid_poll();
    }
}

void device_loop(void)
{
    device_init();
    while(true)
    {
        device_poll();
    }
}   

#include "pico/multicore.h"

static bool started;

void ppf_device_start(void)
{
    if(!started)
    {
        multicore_launch_core1(device_loop);
        started = true;
    }
}

bool ppf_device_check_is_writing(void)
{
    return tud_vendor_n_write_available(0) < CFG_TUD_VENDOR_TX_BUFSIZE;
}
