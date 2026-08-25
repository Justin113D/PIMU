#ifndef PPF_CONFIG_DESCRIPTORS_H_
#define PPF_CONFIG_DESCRIPTORS_H_

#include "tusb.h"
#include "configs/tusb_config.h"

enum
{
    ITF_NUM_HID,
    ITF_NUM_VENDOR,
#if CFG_TUD_AUDIO
    ITF_NUM_AUDIO_CONTROL,
    ITF_NUM_AUDIO_STREAMING_OUT,
    ITF_NUM_AUDIO_STREAMING_IN,
#endif
    ITF_NUM_TOTAL
};

#define TUD_IAD_DESCRIPTOR(_firstitf, _nitfs, _class, _subclass, _protocol, _stridx) \
    /* Descriptor Length    */ 8, \
    /* Descriptor type      */ TUSB_DESC_INTERFACE_ASSOCIATION, \
    /* First interface      */ _firstitf, \
    /* Interface count      */ _nitfs, \
    /* Function class       */ _class, \
    /* Function subclass    */ _subclass, \
    /* Function protocol    */ _protocol, \
    /* Function             */ _stridx

#define AUDIO_ENDPOINT_DESCRIPTOR(_itfnum, _terminal, _maxPacketSize, _endpoint) \
    \
    /* ---------------- Interface Descriptor ----------------- */ \
    /* Descriptor size      */ 9, \
    /* Descriptor type      */ TUSB_DESC_INTERFACE, \
    /* Interface number     */ _itfnum, \
    /* Alternate settings   */ 0, \
    /* Number of endpoints  */ 0, \
    /* Class                */ TUSB_CLASS_AUDIO, \
    /* Subclass             */ AUDIO_SUBCLASS_STREAMING, \
    /* Protocol             */ AUDIO_INT_PROTOCOL_CODE_UNDEF, \
    /* String index         */ 0, \
    \
    /* ---------------- Interface Descriptor ----------------- */ \
    /* Descriptor size      */ 9, \
    /* Descriptor type      */ TUSB_DESC_INTERFACE, \
    /* Interface number     */ _itfnum, \
    /* Alternate settings   */ 1, \
    /* Number of endpoints  */ 1, \
    /* Class                */ TUSB_CLASS_AUDIO, \
    /* Subclass             */ AUDIO_SUBCLASS_STREAMING, \
    /* Protocol             */ AUDIO_INT_PROTOCOL_CODE_UNDEF, \
    /* String index         */ 0, \
    \
    /* -------- Audio Streaming Interface Descriptor --------- */ \
    /* Descriptor size      */ 7, \
    /* Descriptor type      */ TUSB_DESC_CS_INTERFACE, \
    /* Descriptor subtype   */ AUDIO_CS_AS_INTERFACE_AS_GENERAL, \
    /* Terminal Link        */ _terminal, \
    /* Delay                */ 0, \
    /* Format tag           */ U16_TO_U8S_LE(AUDIO_DATA_FORMAT_TYPE_I_PCM), \
    \
    /* ------- Audio Streaming Format Type Descriptor -------- */ \
    /* Descriptor size      */ 11, \
    /* Descriptor type      */ TUSB_DESC_CS_INTERFACE, \
    /* Descriptor subtype   */ AUDIO_CS_AS_INTERFACE_FORMAT_TYPE, \
    /* Format type          */ AUDIO_FORMAT_TYPE_I, \
    /* Number of channels   */ 2, \
    /* Subframe size        */ 2, \
    /* Bit resolution       */ 16, \
    /* Sample frequ. type   */ 1, \
    /* Sample frequency     */ U16_TO_U8S_LE(CFG_TUD_AUDIO_FUNC_1_SAMPLE_RATE), 0x00, \
    \
    /* ----------------- Endpoint Descriptor ----------------- */ \
    /* Descriptor size      */ 7, \
    /* Descriptor type      */ TUSB_DESC_ENDPOINT, \
    /* Endpoint address     */ _endpoint, \
    /* Attributes           */ 0x0D, \
    /* Max packet size      */ U16_TO_U8S_LE(_maxPacketSize), \
    /* Interval             */ 1, \
    \
    /* ----------- Audio Data Endpoint Descriptor ------------ */ \
    /* Descriptor size      */ 7, \
    /* Descriptor type      */ TUSB_DESC_CS_ENDPOINT, \
    /* Descriptor subtype   */ AUDIO_CS_EP_SUBTYPE_GENERAL, \
    /* Attributes           */ 0, \
    /* Lock delay units     */ 0, \
    /* Lock delay           */ U16_TO_U8S_LE(0)

    
#if CFG_TUD_AUDIO
#define CONFIG_DESC_SIZE 268
#else
#define CONFIG_DESC_SIZE 80
#endif

uint8_t const configuration_descriptor[] =
{
    TUD_CONFIG_DESCRIPTOR(
        /* config number        */ 1,                  
        /* interface count      */ ITF_NUM_TOTAL,      
        /* string index         */ 4,                  
        /* total length         */ CONFIG_DESC_SIZE, // sizeof(configuration_descriptor)
        /* attribute            */ 0,               
        /* power in mA          */ 100                 
    ),

    TUD_IAD_DESCRIPTOR(
        /* First interface      */ ITF_NUM_HID,
        /* Interface count      */ 1,
        /* Function class       */ TUSB_CLASS_HID,
        /* Function subclass    */ HID_SUBCLASS_NONE,
        /* Function protocol    */ HID_ITF_PROTOCOL_NONE,
        /* Function name        */ 0
    ),

    TUD_HID_INOUT_DESCRIPTOR(
        /* Interface number     */ ITF_NUM_HID, 
        /* string index         */ 5, 
        /* protocol             */ HID_ITF_PROTOCOL_NONE, 
        /* report desc. len     */ sizeof(report_descriptor), 
        /* EP In address        */ 1 | TUSB_DIR_IN_MASK, 
        /* EP Out address       */ 1,
        /* EP Buffer size       */ CFG_TUD_HID_EP_BUFSIZE, 
        /* EP Polling interval  */ 4
    ),

    TUD_IAD_DESCRIPTOR(
        /* First interface      */ ITF_NUM_VENDOR,
        /* Interface count      */ 1,
        /* Function class       */ TUSB_CLASS_VENDOR_SPECIFIC,
        /* Function subclass    */ HID_SUBCLASS_NONE,
        /* Function protocol    */ HID_ITF_PROTOCOL_NONE,
        /* Function name        */ 0
    ),

    TUD_VENDOR_DESCRIPTOR(
        /* Interface number     */ ITF_NUM_VENDOR, 
        /* string index         */ 6,
        /* EP Out address       */ 2, 
        /* EP In  address       */ 2 | TUSB_DIR_IN_MASK,
        /* EP Buffer size       */ CFG_TUD_VENDOR_EP_BUFSIZE
    ),

    #if CFG_TUD_AUDIO

    TUD_IAD_DESCRIPTOR(
        /* First interface      */ ITF_NUM_AUDIO_CONTROL,
        /* Interface count      */ 3,
        /* Function class       */ TUSB_CLASS_AUDIO,
        /* Function subclass    */ AUDIO_SUBCLASS_CONTROL,
        /* Function protocol    */ AUDIO_INT_PROTOCOL_CODE_UNDEF,
        /* Function name        */ 0
    ),

    /* ---------------- Interface Descriptor ----------------- */
    /* Descriptor size      */ 9,
    /* Descriptor type      */ TUSB_DESC_INTERFACE,
    /* Interface number     */ ITF_NUM_AUDIO_CONTROL,
    /* Alternate settings   */ 0,
    /* Number of endpoints  */ 0,
    /* Class                */ TUSB_CLASS_AUDIO,
    /* Subclass             */ AUDIO_SUBCLASS_CONTROL,
    /* Protocol             */ AUDIO_INT_PROTOCOL_CODE_UNDEF,
    /* String index         */ 0,


    /* ------ Audio Control Interface Header Descriptor ------ */
    /* Descriptor size      */ 10,
    /* Descriptor type      */ TUSB_DESC_CS_INTERFACE,
    /* Descriptor subtype   */ AUDIO_CS_AC_INTERFACE_HEADER,
    /* bcd ADC              */ U16_TO_U8S_LE(0x0100),
    /* Total length         */ U16_TO_U8S_LE(71),
    /* Collection size      */ 2,
    /* Collection values    */ ITF_NUM_AUDIO_STREAMING_OUT, ITF_NUM_AUDIO_STREAMING_IN,


    /* ------- Audio Control Input Terminal Descriptor ------- */
    /* Descriptor size      */ 12,
    /* Descriptor type      */ TUSB_DESC_CS_INTERFACE,
    /* Descriptor subtype   */ AUDIO_CS_AC_INTERFACE_INPUT_TERMINAL,
    /* Terminal ID          */ 1,
    /* Terminal type        */ U16_TO_U8S_LE(AUDIO_TERM_TYPE_USB_STREAMING),
    /* Associated terminal  */ 0,
    /* Number of channels   */ 2,
    /* Channel config       */ U16_TO_U8S_LE(1 | 2), // L, R
    /* Channel names string */ 0,
    /* Terminal name string */ 0,


    /* -------- Audio Control Feature Unit Descriptor -------- */
    /* Descriptor size      */ 10,
    /* Descriptor type      */ TUSB_DESC_CS_INTERFACE,
    /* Descriptor subtype   */ AUDIO_CS_AC_INTERFACE_FEATURE_UNIT,
    /* Unit ID              */ 2,
    /* Source ID            */ 1,
    /* Control size         */ 1,
    /* Controls             */ 3, 0, 0,
    /* String index         */ 0,


    /* ------- Audio Control Output Terminal Descriptor ------ */
    /* Descriptor size      */ 9,
    /* Descriptor type      */ TUSB_DESC_CS_INTERFACE,
    /* Descriptor subtype   */ AUDIO_CS_AC_INTERFACE_OUTPUT_TERMINAL,
    /* Terminal ID          */ 3,
    /* Terminal type        */ U16_TO_U8S_LE(AUDIO_TERM_TYPE_OUT_HEADPHONES),
    /* Associated terminal  */ 0,
    /* Number of channels   */ 2,
    /* Terminal name string */ 0,


    /* ------- Audio Control Input Terminal Descriptor ------- */
    /* Descriptor size      */ 12,
    /* Descriptor type      */ TUSB_DESC_CS_INTERFACE,
    /* Descriptor subtype   */ AUDIO_CS_AC_INTERFACE_INPUT_TERMINAL,
    /* Terminal ID          */ 4,
    /* Terminal type        */ U16_TO_U8S_LE(AUDIO_TERM_TYPE_IN_GENERIC_MIC),
    /* Associated terminal  */ 0,
    /* Number of channels   */ 1,
    /* Channel config       */ U16_TO_U8S_LE(0), // -
    /* Channel names string */ 0,
    /* Terminal name string */ 0,


    /* -------- Audio Control Feature Unit Descriptor -------- */
    /* Descriptor size      */ 9,
    /* Descriptor type      */ TUSB_DESC_CS_INTERFACE,
    /* Descriptor subtype   */ AUDIO_CS_AC_INTERFACE_FEATURE_UNIT,
    /* Unit ID              */ 5,
    /* Source ID            */ 4,
    /* Control size         */ 1,
    /* Controls             */ 3, 0,
    /* String index         */ 0,


    /* ------- Audio Control Output Terminal Descriptor ------ */
    /* Descriptor size      */ 9,
    /* Descriptor type      */ TUSB_DESC_CS_INTERFACE,
    /* Descriptor subtype   */ AUDIO_CS_AC_INTERFACE_OUTPUT_TERMINAL,
    /* Terminal ID          */ 6,
    /* Terminal type        */ U16_TO_U8S_LE(AUDIO_TERM_TYPE_USB_STREAMING),
    /* Associated terminal  */ 0,
    /* Number of channels   */ 5,
    /* Terminal name string */ 0,


    AUDIO_ENDPOINT_DESCRIPTOR(
        /* Interface number     */ ITF_NUM_AUDIO_STREAMING_OUT,
        /* Terminal Link        */ 1,
        /* Max packet size      */ CFG_TUD_AUDIO_FUNC_1_EP_OUT_SZ_MAX,
        /* Endpoint             */ 3
    ),

    AUDIO_ENDPOINT_DESCRIPTOR(
        /* Interface number     */ ITF_NUM_AUDIO_STREAMING_IN,
        /* Terminal Link        */ 6,
        /* Max packet size      */ CFG_TUD_AUDIO_FUNC_1_EP_IN_SZ_MAX,
        /* Endpoint             */ 3 | TUSB_DIR_IN_MASK
    )

    #endif
};

_Static_assert(sizeof(configuration_descriptor) == CONFIG_DESC_SIZE, "configuration descriptor length mismatch");

#if CFG_TUD_AUDIO
_Static_assert(CFG_TUD_AUDIO_FUNC_1_DESC_LEN == 188, "audio descriptor length mismatch");
#endif

#endif