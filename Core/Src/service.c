/*
 * service.c
 *
 *  Created on: Mar 3, 2024
 *      Author: Engineer Protobetz
 */

#include <stdio.h>
#include <stdlib.h>
#include "bluenrg1_gap.h"
#include "bluenrg1_gatt_aci.h"
#include "service.h"

uint8_t SERVICE_1_UUID[16] = {0X66, 0X9a, 0X03, 0X54, 0X55, 0XC6, 0X66, 0X9a, 0X03, 0X54, 0X55, 0XC6, 0X66, 0X9a, 0X03, 0X54};
uint8_t CHAR_1_1_UUID[16] =   {0X76, 0XAA, 0XCD, 0XE2, 0X10, 0X0F, 0X15, 0X2a, 0X71, 0X99, 0X42, 0XB6, 0XAA, 0X73, 0X00, 0X8F};
uint8_t CHAR_1_2_UUID[16] =   {0X76, 0XAA, 0XCD, 0XE3, 0X10, 0X0F, 0X15, 0X2a, 0X71, 0X99, 0X42, 0XB6, 0XAA, 0X73, 0X00, 0X8F};

uint8_t SERVICE_2_UUID[16] = {0x1D, 0x3F, 0x72, 0xFD, 0xD6, 0x22, 0x42, 0xF9, 0xB7, 0x5B, 0xE3, 0x9B, 0x4A, 0xF1, 0x13, 0x7A};
uint8_t CHAR_2_1_UUID[16] =   {0X76, 0XAA, 0XCD, 0XE4, 0X10, 0X0F, 0X15, 0X2a, 0X71, 0X99, 0X42, 0XB6, 0XAA, 0X73, 0X00, 0X8F};
uint8_t CHAR_2_2_UUID[16] =   {0X76, 0XAA, 0XCD, 0XE5, 0X10, 0X0F, 0X15, 0X2a, 0X71, 0X99, 0X42, 0XB6, 0XAA, 0X73, 0X00, 0X8F};

uint16_t service_1_handle, service_2_handle;
uint16_t char_1_1_handle, char_1_2_handle, char_2_1_handle, char_2_2_handle;

int prop1 = 100;
int prop2 = 200;
int prop3 = 300;
int prop4 = 500;


uint8_t connected = 0;
uint8_t set_connectable = 1;
uint16_t connection_handle = 0;
uint8_t notification_enabled = 0;

tBleStatus add_services(void) {
	tBleStatus ret;
	Service_UUID_t service_1_uuid, service_2_uuid;
	Char_UUID_t char_1_1_uuid, char_1_2_uuid, char_2_1_uuid, char_2_2_uuid;
	BLUENRG_memcpy(service_1_uuid.Service_UUID_128, SERVICE_1_UUID, 16);
	BLUENRG_memcpy(service_2_uuid.Service_UUID_128, SERVICE_2_UUID, 16);

	/* Add Services */

	ret = aci_gatt_add_service(UUID_TYPE_128, &service_1_uuid, PRIMARY_SERVICE, 7, &service_1_handle);

	if( ret != BLE_STATUS_SUCCESS ) {
		printf("aci_gatt_add_service 1 : Failed !! \n\r");
	}

	ret = aci_gatt_add_service(UUID_TYPE_128, &service_2_uuid, PRIMARY_SERVICE, 7, &service_2_handle);

	if( ret != BLE_STATUS_SUCCESS ) {
		printf("aci_gatt_add_service 2 : Failed !! \n\r");
	}

	BLUENRG_memcpy(char_1_1_uuid.Char_UUID_128, CHAR_1_1_UUID, 16);
	BLUENRG_memcpy(char_1_2_uuid.Char_UUID_128, CHAR_1_2_UUID, 16);
	BLUENRG_memcpy(char_2_1_uuid.Char_UUID_128, CHAR_2_1_UUID, 16);
	BLUENRG_memcpy(char_2_2_uuid.Char_UUID_128, CHAR_2_2_UUID, 16);

	/*
	 * tBleStatus aci_gatt_add_char(uint16_t Service_Handle,
     *                       uint8_t Char_UUID_Type,
     *                       Char_UUID_t *Char_UUID,
     *                       uint16_t Char_Value_Length,
     *                       uint8_t Char_Properties,
     *                       uint8_t Security_Permissions,
     *                       uint8_t GATT_Evt_Mask,
     *                       uint8_t Enc_Key_Size,
     *                       uint8_t Is_Variable,
     *                       uint16_t *Char_Handle)
	 */
	ret = aci_gatt_add_char(service_1_handle, UUID_TYPE_128, &char_1_1_uuid, 2, CHAR_PROP_READ, ATTR_PERMISSION_NONE, GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP, 0, 0, &char_1_1_handle);
	if( ret != BLE_STATUS_SUCCESS ) {
		printf("aci_gatt_add_char 1.1 : Failed !! \n\r");
	}

	ret = aci_gatt_add_char(service_1_handle, UUID_TYPE_128, &char_1_2_uuid, 2, CHAR_PROP_READ, ATTR_PERMISSION_NONE, GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP, 0, 0, &char_1_2_handle);
	if( ret != BLE_STATUS_SUCCESS ) {
		printf("aci_gatt_add_char 1.2 : Failed !! \n\r");
	}

	ret = aci_gatt_add_char(service_2_handle, UUID_TYPE_128, &char_2_1_uuid, 2, CHAR_PROP_READ, ATTR_PERMISSION_NONE, GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP, 0, 0, &char_2_1_handle);
	if( ret != BLE_STATUS_SUCCESS ) {
		printf("aci_gatt_add_char 2.1 : Failed !! \n\r");
	}

	ret = aci_gatt_add_char(service_2_handle, UUID_TYPE_128, &char_2_2_uuid, 2, CHAR_PROP_READ, ATTR_PERMISSION_NONE, GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP, 0, 0, &char_2_2_handle);

	return ret;
}

void update_1_1_data(int16_t new_data){
	tBleStatus ret;

	/* Update characteristic value*/
	/*
	 * tBleStatus aci_gatt_update_char_value(uint16_t Service_Handle,
                                      uint16_t Char_Handle,
                                      uint8_t Val_Offset,
                                      uint8_t Char_Value_Length,
                                      uint8_t Char_Value[])
	 */
	ret = aci_gatt_update_char_value(service_1_handle, char_1_1_handle, 0, 2, (uint8_t*)&new_data);

	if( ret != BLE_STATUS_SUCCESS ) {
		printf("Characteristic 1.1 : Failed !! \n\r");
	}
}

void update_1_2_data(int16_t new_data){
	tBleStatus ret;

	/* Update characteristic value*/
	/*
	 * tBleStatus aci_gatt_update_char_value(uint16_t Service_Handle,
                                      uint16_t Char_Handle,
                                      uint8_t Val_Offset,
                                      uint8_t Char_Value_Length,
                                      uint8_t Char_Value[])
	 */
	ret = aci_gatt_update_char_value(service_1_handle, char_1_2_handle, 0, 2, (uint8_t*)&new_data);

	if( ret != BLE_STATUS_SUCCESS ) {
		printf("Characteristic 1.2 : Failed !! \n\r");
	}
}

void update_2_1_data(int16_t new_data){
	tBleStatus ret;

	/* Update characteristic value*/
	/*
	 * tBleStatus aci_gatt_update_char_value(uint16_t Service_Handle,
                                      uint16_t Char_Handle,
                                      uint8_t Val_Offset,
                                      uint8_t Char_Value_Length,
                                      uint8_t Char_Value[])
	 */
	ret = aci_gatt_update_char_value(service_2_handle, char_2_1_handle, 0, 2, (uint8_t*)&new_data);

	if( ret != BLE_STATUS_SUCCESS ) {
		printf("Characteristic 2.1 : Failed !! \n\r");
	}
}

void update_2_2_data(int16_t new_data){
	tBleStatus ret;

	/* Update characteristic value*/
	/*
	 * tBleStatus aci_gatt_update_char_value(uint16_t Service_Handle,
                                      uint16_t Char_Handle,
                                      uint8_t Val_Offset,
                                      uint8_t Char_Value_Length,
                                      uint8_t Char_Value[])
	 */
	ret = aci_gatt_update_char_value(service_2_handle, char_2_2_handle, 0, 2, (uint8_t*)&new_data);

	if( ret != BLE_STATUS_SUCCESS ) {
		printf("Characteristic 2.2 : Failed !! \n\r");
	}
}

/*
void GAP_ConnectionComplete_CB(uint8_t addr[6], uint16_t handle) {
	connected = 1;
	connection_handle = handle;

	printf("Connection Complete ...\n\r");
}

void GAP_DisconnectionComplete_CB(uint8_t addr[6], uint16_t handle) {
	printf("Disconnection Complete ...\n\r");
}
*/

void Read_Request_CB(uint16_t handle)
{
	if(handle == char_1_1_handle + 1) {
		prop1 += 2;
		update_1_1_data(prop1);
	}
	else if(handle == char_1_2_handle + 1) {
		prop2 += 2;
		update_1_2_data(prop2);
	}
	else if(handle == char_2_1_handle + 1) {
		prop3 += 2;
		update_2_1_data(prop3);
	}
	else if(handle == char_2_2_handle + 1) {
		prop4 += 2;
		update_2_2_data(prop4);
	}

	if (connection_handle != 0)
	{
		aci_gatt_allow_read(connection_handle);
	}
}

void aci_gatt_read_permit_req_event(uint16_t Connection_Handle,
									uint16_t Attribute_Handle,
									uint16_t Offset)
{
	Read_Request_CB(Attribute_Handle);
}


void hci_le_connection_complete_event(uint8_t Status,
                                      uint16_t Connection_Handle,
                                      uint8_t Role,
                                      uint8_t Peer_Address_Type,
                                      uint8_t Peer_Address[6],
                                      uint16_t Conn_Interval,
                                      uint16_t Conn_Latency,
                                      uint16_t Supervision_Timeout,
                                      uint8_t Master_Clock_Accuracy)
{
	connected = 1;
	connection_handle = Connection_Handle;
}


void hci_disconnection_complete_event(uint8_t Status,
                                      uint16_t Connection_Handle,
                                      uint8_t Reason)
{
	connected  = 1;
	set_connectable = 1;
	connection_handle = 0;

	printf("Disconnected\n\r");
}


void APP_UserEvtRx(void *pData)
{
	uint32_t i;
	hci_spi_pckt *hci_pckt = (hci_spi_pckt *)pData;

	if(hci_pckt->type == HCI_EVENT_PKT){
		hci_event_pckt *event_pckt = (hci_event_pckt *)hci_pckt->data;

		/*Process event metadata*/
		if(event_pckt->evt == EVT_LE_META_EVENT) {
			evt_le_meta_event *evt = (void *)event_pckt->data;

			/*Process all events in table*/
			for (i = 0; i < sizeof(hci_le_meta_events_table)/ sizeof(hci_le_meta_events_table_type);i++) {
				if(evt->subevent == hci_le_meta_events_table[i].evt_code){
					hci_le_meta_events_table[i].process((void *) evt->data);
				}
			}
		}

		/*Process vendor event*/
		else if (event_pckt->evt == EVT_VENDOR) {
			evt_blue_aci *blue_evt = (void *)event_pckt->data;

			for (i = 0; i < sizeof(hci_vendor_specific_events_table)/ sizeof(hci_vendor_specific_events_table_type); i++){
				if(blue_evt->ecode == hci_vendor_specific_events_table[i].evt_code) {
					hci_vendor_specific_events_table[i].process((void *)blue_evt->data);
				}
			}
		}

		/* Process other events*/
		else {
			for(i = 0; i < sizeof(hci_events_table)/sizeof(hci_events_table_type); i++){
				if(event_pckt->evt == hci_events_table[i].evt_code){
					hci_events_table[i].process((void *)event_pckt->data);
				}
			}
		}
	}
}
