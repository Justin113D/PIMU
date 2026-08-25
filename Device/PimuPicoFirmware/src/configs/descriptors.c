#include "tusb.h"

#include "tusb_config.h"
#include "debug.h"

#define USB2_BCD 0x0200

//--------------------------------------------------------------------+
// Device
//--------------------------------------------------------------------+

static const tusb_desc_device_t desc_device = {
    .bLength = sizeof(tusb_desc_device_t),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = USB2_BCD,
    .bDeviceClass = TUSB_CLASS_MISC,
    .bDeviceSubClass = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol = MISC_PROTOCOL_IAD,
    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,

    .idVendor = 0x057E,
    .idProduct = 0x2069,
    .bcdDevice = 0x0201,

    .iManufacturer = 0x01,
    .iProduct = 0x02,
    .iSerialNumber = 0x03,

    .bNumConfigurations = 0x01,
};

uint8_t const *tud_descriptor_device_cb(void)
{
    PPF_DEBUG_INFO_USB("[DESC] Device");
    return (uint8_t const *)&desc_device;
}

//--------------------------------------------------------------------+
// Device Qualifier
//--------------------------------------------------------------------+

static const tusb_desc_device_qualifier_t const desc_device_qualifier =
{
  .bLength            = sizeof(tusb_desc_device_qualifier_t),
  .bDescriptorType    = TUSB_DESC_DEVICE_QUALIFIER,
  .bcdUSB             = USB2_BCD,

  .bDeviceClass       = 0x00,
  .bDeviceSubClass    = 0x00,
  .bDeviceProtocol    = 0x00,

  .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,
  .bNumConfigurations = 0x01,
  .bReserved          = 0x00
};

uint8_t const* tud_descriptor_device_qualifier_cb(void)
{
    PPF_DEBUG_INFO_USB("[DESC] Device qualifier");
    return (uint8_t const*) &desc_device_qualifier;
}

//--------------------------------------------------------------------+
// Report
//--------------------------------------------------------------------+

const uint8_t report_descriptor[] = {
    HID_USAGE_PAGE(HID_USAGE_PAGE_DESKTOP),
    HID_USAGE(HID_USAGE_DESKTOP_GAMEPAD),
    HID_COLLECTION ( HID_COLLECTION_APPLICATION ),
        HID_REPORT_ID(5)
        HID_USAGE_PAGE(0xFF),
        HID_USAGE(1),
        HID_LOGICAL_MIN(0),
        HID_LOGICAL_MAX_N(0xFF, 2),
        HID_REPORT_COUNT(63),
        HID_REPORT_SIZE(8),
        HID_INPUT(HID_VARIABLE),
        HID_REPORT_ID(9)
        HID_USAGE(1),
        HID_REPORT_COUNT(2),
        HID_INPUT(HID_VARIABLE),
        HID_USAGE_PAGE(9),
        HID_USAGE_MIN(1),
        HID_USAGE_MAX(21),
        HID_LOGICAL_MAX(1),
        HID_REPORT_COUNT(21),
        HID_REPORT_SIZE(1),
        HID_INPUT(HID_VARIABLE),
        HID_REPORT_COUNT(1),
        HID_REPORT_SIZE(3),
        HID_INPUT(HID_CONSTANT | HID_VARIABLE),
        HID_USAGE_PAGE(1),
        HID_USAGE(5),
        HID_COLLECTION ( HID_COLLECTION_PHYSICAL ),
            HID_USAGE(0x30), 
            HID_USAGE(0x31),
            HID_USAGE(0x33),
            HID_USAGE(0x35),
            HID_LOGICAL_MAX_N(0xFFF, 2),
            HID_REPORT_COUNT(4),
            HID_REPORT_SIZE(12),
            HID_INPUT(HID_VARIABLE),
        HID_COLLECTION_END,
        HID_USAGE_PAGE(0xFF),
        HID_USAGE(2),
        HID_LOGICAL_MAX_N(0xFF, 2),
        HID_REPORT_COUNT(52),
        HID_REPORT_SIZE(8),
        HID_INPUT(HID_VARIABLE),
        HID_REPORT_ID(2)
        HID_USAGE(1),
        HID_REPORT_COUNT(63),
        HID_OUTPUT(HID_VARIABLE),
    HID_COLLECTION_END
};

uint8_t const * tud_hid_descriptor_report_cb(uint8_t itf)
{
    PPF_DEBUG_INFO_USB("[DESC] Report %i", itf);
    return report_descriptor;
}

//--------------------------------------------------------------------+
// Configuration
//--------------------------------------------------------------------+

#include "config_descriptor.h"

uint8_t const *tud_descriptor_configuration_cb(uint8_t index)
{
    PPF_DEBUG_INFO_USB("[DESC] Config %i", index);
    return configuration_descriptor;
}

static uint8_t desc_other_speed_configuration[sizeof(configuration_descriptor)];

uint8_t const *tud_descriptor_other_speed_configuration_cb(uint8_t index) {
    (void)index;
    // The genuine controller reports the same full-speed endpoint sizes in its
    // other-speed configuration, but uses high-speed interval encoding.

    PPF_DEBUG_INFO_USB("[DESC] Config other speed %i", index);
    memcpy(desc_other_speed_configuration, configuration_descriptor, sizeof(desc_other_speed_configuration));
    
    desc_other_speed_configuration[1] = TUSB_DESC_OTHER_SPEED_CONFIG;

    for (size_t offset = 0; offset < sizeof(desc_other_speed_configuration);) {
        uint8_t const length = desc_other_speed_configuration[offset];
        if (length == 0 || offset + length > sizeof(desc_other_speed_configuration)) {
            break;
        }

        if (desc_other_speed_configuration[offset + 1] == TUSB_DESC_ENDPOINT && length >= 7) {
            uint8_t const transfer_type = desc_other_speed_configuration[offset + 3] & 0x03;
            if (transfer_type == TUSB_XFER_INTERRUPT) {
                desc_other_speed_configuration[offset + 6] = 6;
            } else if (transfer_type == TUSB_XFER_ISOCHRONOUS) {
                desc_other_speed_configuration[offset + 6] = 4;
            }
        }

        offset += length;
    }

    return desc_other_speed_configuration;
}

//--------------------------------------------------------------------+
// String
//--------------------------------------------------------------------+

// array of pointer to string descriptors
static char const *string_desc_arr[] =
{
    // 0: English - United States, Bulgarian and Arabic - Irac 
    // (don't ask me, i copied this off the switch 2 pro controller)
    (const char[]){0x09, 0x04, 0x02, 0x04, 0x01, 0x08}, 

    "Nintendo",                 // 1: Manufacturer
    "Switch 2 Pro Controller",  // 2: Product
    "00",                       // 3: Serial
    "Config_0",                 // 4: Config name
    "If_Hid",                   // 5: HID name
    "Switch 2 Pro Controller",  // 6: Vendor Interface name
};

static uint16_t _desc_str_buffer[32 + 1];

uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
    (void)langid;
    size_t chr_count;

    PPF_DEBUG_INFO_USB("[DESC] String %i for %i", index, langid);

    if(index == 0) // language
    {
        memcpy(&_desc_str_buffer[1], string_desc_arr[0], 6);
        chr_count = 3;
    }
    else
    {
        // Note: the 0xEE index string is a Microsoft OS 1.0 Descriptors.
        // https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/microsoft-defined-usb-descriptors

        if (index >= (sizeof(string_desc_arr) / sizeof(string_desc_arr[0])))
        {
            return NULL;
        }

        const char *str = string_desc_arr[index];

        // Cap at max char
        chr_count = strlen(str);
        size_t const max_count = sizeof(_desc_str_buffer) / sizeof(_desc_str_buffer[0]) - 1; // -1 for string type
        if (chr_count > max_count)
        {
            chr_count = max_count;
        }

        // Convert ASCII string into UTF-16
        for (size_t i = 0; i < chr_count; i++)
        {
            _desc_str_buffer[1 + i] = str[i];
        }
    }

    // first byte is length (including header), second byte is string type
    _desc_str_buffer[0] = (uint16_t)((TUSB_DESC_STRING << 8) | (2 * chr_count + 2));

    return _desc_str_buffer;
}