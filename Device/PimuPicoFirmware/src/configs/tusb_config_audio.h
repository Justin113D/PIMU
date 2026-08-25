#ifndef PPF_TUSB_CONFIG_AUDIO_H_
#define PPF_TUSB_CONFIG_AUDIO_H_

// Audio stuff needs custom drivers to work natively, which i won't do right now
#define CFG_TUD_AUDIO 0 

#if CFG_TUD_AUDIO

// Total size of all descriptors under (and including) the audio IAD descriptor
#define CFG_TUD_AUDIO_FUNC_1_DESC_LEN                                 188 
#define CFG_TUD_AUDIO_FUNC_1_SAMPLE_RATE                              48000

// Number of Standard AS Interface Descriptors (4.9.1) defined per audio function 
#define CFG_TUD_AUDIO_FUNC_1_N_AS_INT                                 2

// Size of control request buffer
#define CFG_TUD_AUDIO_FUNC_1_CTRL_BUF_SZ                              64

#define CFG_TUD_AUDIO_ENABLE_EP_OUT                                   1
#define CFG_TUD_AUDIO_FUNC_1_EP_OUT_SZ_MAX                            192
#define CFG_TUD_AUDIO_FUNC_1_EP_OUT_SW_BUF_SZ                         (4 * CFG_TUD_AUDIO_FUNC_1_EP_OUT_SZ_MAX)

#define CFG_TUD_AUDIO_ENABLE_EP_IN                                    1
#define CFG_TUD_AUDIO_FUNC_1_EP_IN_SZ_MAX                             192
#define CFG_TUD_AUDIO_FUNC_1_EP_IN_SW_BUF_SZ                          (4 * CFG_TUD_AUDIO_FUNC_1_EP_IN_SZ_MAX)

// Driver gets this info from the descriptors
// we define it here to use it to setup the descriptors and to do calculations with it
#define CFG_TUD_AUDIO_FUNC_1_N_BYTES_PER_SAMPLE_TX                    2
#define CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_TX                            2

#endif

#endif