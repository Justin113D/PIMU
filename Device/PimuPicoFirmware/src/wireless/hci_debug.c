#include "hci_debug.h"
#if defined(BLUETOOTH_ENABLED) && HCI_DEBUG_ENABLED

#include <stdio.h>

#include "pico/stdlib.h"
#include "btstack.h"

#include "debug.h"

const char *event_type_print_table[256] = {
    "HCI_EVENT_NOP",
    "HCI_EVENT_INQUIRY_COMPLETE",
    "HCI_EVENT_INQUIRY_RESULT",
    "HCI_EVENT_CONNECTION_COMPLETE",
    "HCI_EVENT_CONNECTION_REQUEST",
    "HCI_EVENT_DISCONNECTION_COMPLETE",
    "HCI_EVENT_AUTHENTICATION_COMPLETE_EVENT",
    "HCI_EVENT_REMOTE_NAME_REQUEST_COMPLETE",
    "HCI_EVENT_ENCRYPTION_CHANGE",
    "HCI_EVENT_CHANGE_CONNECTION_LINK_KEY_COMPLETE",
    "HCI_EVENT_MASTER_LINK_KEY_COMPLETE",
    "HCI_EVENT_READ_REMOTE_SUPPORTED_FEATURES_COMPLETE",
    "HCI_EVENT_READ_REMOTE_VERSION_INFORMATION_COMPLETE",
    "HCI_EVENT_QOS_SETUP_COMPLETE",
    "HCI_EVENT_COMMAND_COMPLETE",
    "HCI_EVENT_COMMAND_STATUS",
    "HCI_EVENT_HARDWARE_ERROR",
    "HCI_EVENT_FLUSH_OCCURRED",
    "HCI_EVENT_ROLE_CHANGE",
    "HCI_EVENT_NUMBER_OF_COMPLETED_PACKETS",
    "HCI_EVENT_MODE_CHANGE",
    "HCI_EVENT_RETURN_LINK_KEYS",
    "HCI_EVENT_PIN_CODE_REQUEST",
    "HCI_EVENT_LINK_KEY_REQUEST",
    "HCI_EVENT_LINK_KEY_NOTIFICATION",
    "HCI_EVENT_LOOPBACK_COMMAND",
    "HCI_EVENT_DATA_BUFFER_OVERFLOW",
    "HCI_EVENT_MAX_SLOTS_CHANGED",
    "HCI_EVENT_READ_CLOCK_OFFSET_COMPLETE",
    "HCI_EVENT_CONNECTION_PACKET_TYPE_CHANGED",
    "HCI_EVENT_QOS_VIOLATION",
    NULL,
    "HCI_EVENT_PAGE_SCAN_REPETITION_MODE_CHANGE",
    "HCI_EVENT_FLOW_SPECIFICATION_COMPLETE",
    "HCI_EVENT_INQUIRY_RESULT_WITH_RSSI",
    "HCI_EVENT_READ_REMOTE_EXTENDED_FEATURES_COMPLETE",
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    "HCI_EVENT_SYNCHRONOUS_CONNECTION_COMPLETE",
    "HCI_EVENT_SYNCHRONOUS_CONNECTION_CHANGED",
    "HCI_EVENT_SNIFF_SUBRATING",
    "HCI_EVENT_EXTENDED_INQUIRY_RESPONSE",
    "HCI_EVENT_ENCRYPTION_KEY_REFRESH_COMPLETE",
    "HCI_EVENT_IO_CAPABILITY_REQUEST",
    "HCI_EVENT_IO_CAPABILITY_RESPONSE",
    "HCI_EVENT_USER_CONFIRMATION_REQUEST",
    "HCI_EVENT_USER_PASSKEY_REQUEST",
    "HCI_EVENT_REMOTE_OOB_DATA_REQUEST",
    "HCI_EVENT_SIMPLE_PAIRING_COMPLETE",
    NULL,
    "HCI_EVENT_LINK_SUPERVISION_TIMEOUT_CHANGED",
    "HCI_EVENT_ENHANCED_FLUSH_COMPLETE",
    NULL,
    "HCI_EVENT_USER_PASSKEY_NOTIFICATION",
    "HCI_EVENT_KEYPRESS_NOTIFICATION",
    "HCI_EVENT_REMOTE_HOST_SUPPORTED_FEATURES",
    "HCI_EVENT_LE_META",
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    "HCI_EVENT_NUMBER_OF_COMPLETED_DATA_BLOCKS",
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    "HCI_EVENT_ENCRYPTION_CHANGE_V2",
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    "BTSTACK_EVENT_STATE",
    "BTSTACK_EVENT_NR_CONNECTIONS_CHANGED",
    "BTSTACK_EVENT_POWERON_FAILED",
    "DAEMON_EVENT_VERSION",
    "DAEMON_EVENT_SYSTEM_BLUETOOTH_ENABLED",
    "DAEMON_EVENT_REMOTE_NAME_CACHED",
    "BTSTACK_EVENT_SCAN_MODE_CHANGED",
    "DAEMON_EVENT_CONNECTION_OPENED",
    "DAEMON_EVENT_CONNECTION_CLOSED",
    "HCI_EVENT_TRANSPORT_SLEEP_MODE",
    "HCI_EVENT_TRANSPORT_USB_INFO",
    "HCI_EVENT_BIS_CAN_SEND_NOW",
    "HCI_EVENT_CIS_CAN_SEND_NOW",
    "HCI_EVENT_TRANSPORT_READY",
    "HCI_EVENT_TRANSPORT_PACKET_SENT",
    "HCI_EVENT_SCO_CAN_SEND_NOW",
    "L2CAP_EVENT_CHANNEL_OPENED",
    "L2CAP_EVENT_CHANNEL_CLOSED",
    "L2CAP_EVENT_INCOMING_CONNECTION",
    "L2CAP_EVENT_TIMEOUT_CHECK",
    "DAEMON_EVENT_L2CAP_CREDITS",
    "DAEMON_EVENT_L2CAP_SERVICE_REGISTERED",
    "L2CAP_EVENT_CONNECTION_PARAMETER_UPDATE_REQUEST",
    "L2CAP_EVENT_CONNECTION_PARAMETER_UPDATE_RESPONSE",
    "L2CAP_EVENT_INFORMATION_RESPONSE",
    "L2CAP_EVENT_CAN_SEND_NOW",
    "L2CAP_EVENT_PACKET_SENT",
    "L2CAP_EVENT_ERTM_BUFFER_RELEASED",
    "L2CAP_EVENT_CBM_INCOMING_CONNECTION",
    "L2CAP_EVENT_CBM_CHANNEL_OPENED",
    "L2CAP_EVENT_TRIGGER_RUN",
    "L2CAP_EVENT_ECBM_INCOMING_CONNECTION",
    "RFCOMM_EVENT_CHANNEL_OPENED",
    "RFCOMM_EVENT_CHANNEL_CLOSED",
    "RFCOMM_EVENT_INCOMING_CONNECTION",
    "RFCOMM_EVENT_REMOTE_LINE_STATUS",
    NULL,
    "DAEMON_EVENT_RFCOMM_SERVICE_REGISTERED",
    "DAEMON_EVENT_RFCOMM_PERSISTENT_CHANNEL",
    "RFCOMM_EVENT_REMOTE_MODEM_STATUS",
    "RFCOMM_EVENT_PORT_CONFIGURATION",
    "RFCOMM_EVENT_CAN_SEND_NOW",
    "L2CAP_EVENT_ECBM_CHANNEL_OPENED",
    "L2CAP_EVENT_ECBM_RECONFIGURED",
    "L2CAP_EVENT_ECBM_RECONFIGURATION_COMPLETE",
    NULL,
    NULL,
    NULL,
    "DAEMON_EVENT_SDP_SERVICE_REGISTERED",
    "SDP_EVENT_QUERY_COMPLETE",
    "SDP_EVENT_QUERY_RFCOMM_SERVICE",
    "SDP_EVENT_QUERY_ATTRIBUTE_BYTE",
    "SDP_EVENT_QUERY_ATTRIBUTE_VALUE",
    "SDP_EVENT_QUERY_SERVICE_RECORD_HANDLE",
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    "GATT_EVENT_QUERY_COMPLETE",
    "GATT_EVENT_SERVICE_QUERY_RESULT",
    "GATT_EVENT_CHARACTERISTIC_QUERY_RESULT",
    "GATT_EVENT_INCLUDED_SERVICE_QUERY_RESULT",
    "GATT_EVENT_ALL_CHARACTERISTIC_DESCRIPTORS_QUERY_RESULT",
    "GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT",
    "GATT_EVENT_LONG_CHARACTERISTIC_VALUE_QUERY_RESULT",
    "GATT_EVENT_NOTIFICATION",
    "GATT_EVENT_INDICATION",
    "GATT_EVENT_CHARACTERISTIC_DESCRIPTOR_QUERY_RESULT",
    "GATT_EVENT_LONG_CHARACTERISTIC_DESCRIPTOR_QUERY_RESULT",
    "GATT_EVENT_MTU",
    "GATT_EVENT_CAN_WRITE_WITHOUT_RESPONSE",
    "GATT_EVENT_CONNECTED",
    "GATT_EVENT_DISCONNECTED",
    "GATT_EVENT_SERVICE_CHANGED",
    NULL,
    NULL,
    NULL,
    "ATT_EVENT_CONNECTED",
    "ATT_EVENT_DISCONNECTED",
    "ATT_EVENT_MTU_EXCHANGE_COMPLETE",
    "ATT_EVENT_HANDLE_VALUE_INDICATION_COMPLETE",
    "ATT_EVENT_CAN_SEND_NOW",
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    "BNEP_EVENT_SERVICE_REGISTERED",
    "BNEP_EVENT_CHANNEL_OPENED",
    "BNEP_EVENT_CHANNEL_CLOSED",
    "BNEP_EVENT_CHANNEL_TIMEOUT",
    "BNEP_EVENT_CAN_SEND_NOW",
    NULL,
    NULL,
    "SM_EVENT_SECURITY_REQUEST",
    "SM_EVENT_JUST_WORKS_REQUEST",
    "SM_EVENT_PASSKEY_DISPLAY_NUMBER",
    "SM_EVENT_PASSKEY_DISPLAY_CANCEL",
    "SM_EVENT_PASSKEY_INPUT_NUMBER",
    "SM_EVENT_NUMERIC_COMPARISON_REQUEST",
    "SM_EVENT_IDENTITY_RESOLVING_STARTED",
    "SM_EVENT_IDENTITY_RESOLVING_FAILED",
    "SM_EVENT_IDENTITY_RESOLVING_SUCCEEDED",
    "SM_EVENT_AUTHORIZATION_REQUEST",
    "SM_EVENT_AUTHORIZATION_RESULT",
    "SM_EVENT_KEYPRESS_NOTIFICATION",
    "SM_EVENT_IDENTITY_CREATED",
    "SM_EVENT_PAIRING_STARTED",
    "SM_EVENT_PAIRING_COMPLETE",
    "SM_EVENT_REENCRYPTION_STARTED",
    "SM_EVENT_REENCRYPTION_COMPLETE",
    "GAP_EVENT_SECURITY_LEVEL",
    "GAP_EVENT_DEDICATED_BONDING_COMPLETED",
    "GAP_EVENT_ADVERTISING_REPORT",
    "GAP_EVENT_EXTENDED_ADVERTISING_REPORT",
    "GAP_EVENT_INQUIRY_RESULT",
    "GAP_EVENT_INQUIRY_COMPLETE",
    "GAP_EVENT_RSSI_MEASUREMENT",
    "GAP_EVENT_LOCAL_OOB_DATA",
    "GAP_EVENT_PAIRING_STARTED",
    "GAP_EVENT_PAIRING_COMPLETE",
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    "HCI_EVENT_META_GAP",
    "HCI_EVENT_HSP_META",
    "HCI_EVENT_HFP_META",
    "HCI_EVENT_ANCS_META",
    "HCI_EVENT_AVDTP_META",
    "HCI_EVENT_AVRCP_META",
    "HCI_EVENT_GOEP_META",
    "HCI_EVENT_PBAP_META",
    "HCI_EVENT_HID_META",
    "HCI_EVENT_A2DP_META",
    "HCI_EVENT_HIDS_META",
    "HCI_EVENT_GATTSERVICE_META",
    "HCI_EVENT_BIP_META",
    "HCI_EVENT_MAP_META",
    "HCI_EVENT_MESH_META",
    "HCI_EVENT_LEAUDIO_META",
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    "HCI_EVENT_VENDOR_SPECIFIC",
};

const char *hci_state_print_table[6] = {
    "HCI_STATE_OFF",
    "HCI_STATE_INITIALIZING",
    "HCI_STATE_WORKING",
    "HCI_STATE_HALTING",
    "HCI_STATE_SLEEPING",
    "HCI_STATE_FALLING_ASLEEP",
};

void ppf_hci_debug_packet(const char *origin, uint8_t *packet)
{
    uint8_t event = hci_event_packet_get_type(packet);

    if (event == HCI_EVENT_COMMAND_STATUS 
        || event == HCI_EVENT_COMMAND_COMPLETE 
        || event == HCI_EVENT_TRANSPORT_PACKET_SENT 
        || event == HCI_EVENT_NUMBER_OF_COMPLETED_PACKETS 
        || event == HCI_EVENT_READ_REMOTE_SUPPORTED_FEATURES_COMPLETE
        || event == RFCOMM_EVENT_CAN_SEND_NOW)
    {
        return;
    }

    bd_addr_t device_address;

    PPF_DEBUG_INFO_BLUETOOTH("[%s] %s", origin, event_type_print_table[event]);

    switch (event)
    {
    case BTSTACK_EVENT_STATE:
        PPF_DEBUG_INFO_BLUETOOTH("\tState: %s", hci_state_print_table[btstack_event_state_get_state(packet)]);
        break;

    case HCI_EVENT_COMMAND_STATUS:
        PPF_DEBUG_INFO_BLUETOOTH("\tOPCode: 0x%04X", hci_event_command_status_get_command_opcode(packet));
        PPF_DEBUG_INFO_BLUETOOTH("\tNo. of command packets: %i", hci_event_command_status_get_num_hci_command_packets(packet));
        PPF_DEBUG_INFO_BLUETOOTH("\tStatus: %i", hci_event_command_status_get_status(packet));
        break;

    case HCI_EVENT_COMMAND_COMPLETE:
        PPF_DEBUG_INFO_BLUETOOTH("\tOPCode: 0x%04X", hci_event_command_complete_get_command_opcode(packet));
        PPF_DEBUG_INFO_BLUETOOTH("\tNo. of command packets: %i", hci_event_command_complete_get_num_hci_command_packets(packet));
        break;

    case BTSTACK_EVENT_SCAN_MODE_CHANGED:
        PPF_DEBUG_INFO_BLUETOOTH("\tConnectable: %i", btstack_event_scan_mode_changed_get_connectable(packet));
        PPF_DEBUG_INFO_BLUETOOTH("\tDiscoverable: %i", btstack_event_scan_mode_changed_get_discoverable(packet));
        break;

    case HCI_EVENT_CONNECTION_REQUEST:
        hci_event_connection_request_get_bd_addr(packet, device_address);

        PPF_DEBUG_INFO_BLUETOOTH("\tDevice: %s", bd_addr_to_str(device_address));
        PPF_DEBUG_INFO_BLUETOOTH("\tClass of Device: 0x%06X", hci_event_connection_request_get_class_of_device(packet));
        PPF_DEBUG_INFO_BLUETOOTH("\tLink Type: %i", hci_event_connection_request_get_link_type(packet));
        break;

    case BTSTACK_EVENT_NR_CONNECTIONS_CHANGED:
        PPF_DEBUG_INFO_BLUETOOTH("\tConnections: %i", btstack_event_nr_connections_changed_get_number_connections(packet));
        break;

    case HCI_EVENT_CONNECTION_COMPLETE:
        hci_event_connection_complete_get_bd_addr(packet, device_address);

        PPF_DEBUG_INFO_BLUETOOTH("\tDevice: %s", bd_addr_to_str(device_address));
        PPF_DEBUG_INFO_BLUETOOTH("\tConnection handle: 0x%04X", hci_event_connection_complete_get_connection_handle(packet));
        PPF_DEBUG_INFO_BLUETOOTH("\tEncryption enabled: %i", hci_event_connection_complete_get_encryption_enabled(packet));
        PPF_DEBUG_INFO_BLUETOOTH("\tLink type: %i", hci_event_connection_complete_get_link_type(packet));
        PPF_DEBUG_INFO_BLUETOOTH("\tConnection status: %i", hci_event_connection_complete_get_status(packet));
        break;

    case HCI_EVENT_MAX_SLOTS_CHANGED:
        PPF_DEBUG_INFO_BLUETOOTH("\tHandle: 0x%04X", hci_event_max_slots_changed_get_handle(packet));
        PPF_DEBUG_INFO_BLUETOOTH("\tMax slots: %i", hci_event_max_slots_changed_get_lmp_max_slots(packet));
        break;

    case HCI_EVENT_LINK_SUPERVISION_TIMEOUT_CHANGED:
        PPF_DEBUG_INFO_BLUETOOTH("\tHandle: 0x%04X", hci_event_link_supervision_timeout_changed_get_handle(packet));
        PPF_DEBUG_INFO_BLUETOOTH("\tTimeout: %i", hci_event_link_supervision_timeout_changed_get_link_supervision_timeout(packet));
        break;

    case GAP_EVENT_PAIRING_STARTED:
        gap_event_pairing_started_get_bd_addr(packet, device_address);

        PPF_DEBUG_INFO_BLUETOOTH("\tDevice: %s", bd_addr_to_str(device_address));
        PPF_DEBUG_INFO_BLUETOOTH("\tHandle: 0x%04X", gap_event_pairing_started_get_con_handle(packet));
        PPF_DEBUG_INFO_BLUETOOTH("\tInitiator: 0x%02X", gap_event_pairing_started_get_initiator(packet));
        PPF_DEBUG_INFO_BLUETOOTH("\tSSP: 0x%02X", gap_event_pairing_started_get_ssp(packet));
        break;

    case HCI_EVENT_IO_CAPABILITY_RESPONSE:
        hci_event_io_capability_response_get_bd_addr(packet, device_address);

        PPF_DEBUG_INFO_BLUETOOTH("\tDevice: %s", bd_addr_to_str(device_address));
        PPF_DEBUG_INFO_BLUETOOTH("\tAuth. requirements: 0x%02X", hci_event_io_capability_response_get_authentication_requirements(packet));
        PPF_DEBUG_INFO_BLUETOOTH("\tIO capability: 0x%02X", hci_event_io_capability_response_get_io_capability(packet));
        PPF_DEBUG_INFO_BLUETOOTH("\tOOB data present: %i", hci_event_io_capability_response_get_oob_data_present(packet));
        break;

    case HCI_EVENT_IO_CAPABILITY_REQUEST:
        hci_event_io_capability_request_get_bd_addr(packet, device_address);
        PPF_DEBUG_INFO_BLUETOOTH("\tDevice: %s", bd_addr_to_str(device_address));
        break;

    case HCI_EVENT_USER_CONFIRMATION_REQUEST:
        hci_event_user_confirmation_request_get_bd_addr(packet, device_address);

        PPF_DEBUG_INFO_BLUETOOTH("\tDevice: %s", bd_addr_to_str(device_address));
        PPF_DEBUG_INFO_BLUETOOTH("\tValue: %i", hci_event_user_confirmation_request_get_numeric_value(packet));
        break;

    case GAP_EVENT_PAIRING_COMPLETE:
        gap_event_pairing_complete_get_bd_addr(packet, device_address);

        PPF_DEBUG_INFO_BLUETOOTH("\tDevice: %s", bd_addr_to_str(device_address));
        PPF_DEBUG_INFO_BLUETOOTH("\tHandle: 0x%04X", gap_event_pairing_complete_get_con_handle(packet));
        PPF_DEBUG_INFO_BLUETOOTH("\tStatus: %i", gap_event_pairing_complete_get_status(packet));
        break;

    case HCI_EVENT_SIMPLE_PAIRING_COMPLETE:
        hci_event_simple_pairing_complete_get_bd_addr(packet, device_address);

        PPF_DEBUG_INFO_BLUETOOTH("\tDevice: %s", bd_addr_to_str(device_address));
        PPF_DEBUG_INFO_BLUETOOTH("\tStatus: %i", hci_event_simple_pairing_complete_get_status(packet));
        break;

    case HCI_EVENT_LINK_KEY_NOTIFICATION:
        break;

    case HCI_EVENT_ENCRYPTION_CHANGE:
        PPF_DEBUG_INFO_BLUETOOTH("\tConnection handle: 0x%04X", hci_event_encryption_change_get_connection_handle(packet));
        PPF_DEBUG_INFO_BLUETOOTH("\tEncryption enabled: %i", hci_event_encryption_change_get_encryption_enabled(packet));
        PPF_DEBUG_INFO_BLUETOOTH("\tConnection status: %i", hci_event_encryption_change_get_status(packet));
        break;

    case GAP_EVENT_SECURITY_LEVEL:
        PPF_DEBUG_INFO_BLUETOOTH("\tConnection handle: 0x%04X", gap_event_security_level_get_handle(packet));
        PPF_DEBUG_INFO_BLUETOOTH("\tSecurity level: %i", gap_event_security_level_get_security_level(packet));
        PPF_DEBUG_INFO_BLUETOOTH("\tConnection status: %i", gap_event_security_level_get_status(packet));
        break;

    case HCI_EVENT_DISCONNECTION_COMPLETE:
        PPF_DEBUG_INFO_BLUETOOTH("\tConnection handle: 0x%04X", hci_event_disconnection_complete_get_connection_handle(packet));
        PPF_DEBUG_INFO_BLUETOOTH("\tReason: %i", hci_event_disconnection_complete_get_reason(packet));
        PPF_DEBUG_INFO_BLUETOOTH("\tConnection status: %i", hci_event_disconnection_complete_get_status(packet));
        break;
    }
}


#else

void ppf_hci_debug_packet(const char* origin, uint8_t *packet) { }

#endif