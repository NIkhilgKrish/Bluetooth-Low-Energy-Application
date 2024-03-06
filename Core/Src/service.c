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

//uint8_t SERVICE_2_UUID[16] = {0x1D, 0x3F, 0x72, 0xFD, 0xD6, 0x22, 0x42, 0xF9, 0xB7, 0x5B, 0xE3, 0x9B, 0x4A, 0xF1, 0x13, 0x7A};
//uint8_t CHAR_2_1_UUID[16] =   {0X76, 0XAA, 0XCD, 0XE4, 0X10, 0X0F, 0X15, 0X2a, 0X71, 0X99, 0X42, 0XB6, 0XAA, 0X73, 0X00, 0X8F};
//uint8_t CHAR_2_2_UUID[16] =   {0X76, 0XAA, 0XCD, 0XE5, 0X10, 0X0F, 0X15, 0X2a, 0X71, 0X99, 0X42, 0XB6, 0XAA, 0X73, 0X00, 0X8F};

uint16_t service_chat_handle;
uint16_t char_tx_handle, char_rx_handle;

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
	Service_UUID_t service_uuid;
	Char_UUID_t char_uuid_tx, char_uuid_rx;
	BLUENRG_memcpy(service_uuid.Service_UUID_128, SERVICE_1_UUID, 16);

	/* Add Services */

	ret = aci_gatt_add_service(UUID_TYPE_128, &service_uuid, PRIMARY_SERVICE, 7, &service_chat_handle);

	if( ret != BLE_STATUS_SUCCESS ) {
		printf("aci_gatt_add_service 1 : Failed !! \n\r");
	}

	BLUENRG_memcpy(char_uuid_tx.Char_UUID_128, CHAR_1_1_UUID, 16);
	BLUENRG_memcpy(char_uuid_rx.Char_UUID_128, CHAR_1_2_UUID, 16);

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


	ret = aci_gatt_add_char(service_chat_handle,
							UUID_TYPE_128,
							&char_uuid_tx,
							20,
							CHAR_PROP_NOTIFY,
							ATTR_PERMISSION_NONE,
							0,
							0,
							1,
							&char_tx_handle);

	if( ret != BLE_STATUS_SUCCESS ) {
		printf("aci_gatt_add_char 2.1 : Failed !! \n\r");
	}

	ret = aci_gatt_add_char(service_chat_handle,
							UUID_TYPE_128,
							&char_uuid_rx,
							20,
							CHAR_PROP_WRITE | CHAR_PROP_WRITE_WITHOUT_RESP,
							ATTR_PERMISSION_NONE,
							GATT_NOTIFY_ATTRIBUTE_WRITE,
							0,
							1,
							&char_rx_handle);

	return ret;
}

uint8_t rcv_data[20];

void receive_data(uint8_t* new_data, uint8_t no_bytes){

	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);

	for(int i=0; i < no_bytes;i++){
		rcv_data[i] = new_data[i];
	}

}

void send_data(uint8_t* new_data, uint8_t no_bytes){
	tBleStatus ret;

	/* Update characteristic value*/
	/*
	 * tBleStatus aci_gatt_update_char_value(uint16_t Service_Handle,
                                      uint16_t Char_Handle,
                                      uint8_t Val_Offset,
                                      uint8_t Char_Value_Length,
                                      uint8_t Char_Value[])
	 */
	ret = aci_gatt_update_char_value(service_chat_handle, char_tx_handle, 0, no_bytes, new_data);

	if( ret != BLE_STATUS_SUCCESS ) {
		printf("Characteristic 2.2 : Failed !! \n\r");
	}
}


void Attribute_Modify_CB(uint16_t handle, uint8_t data_length, uint8_t *att_data){
	if(handle == char_rx_handle + 1){
		receive_data(att_data, data_length);
	}
	else if (handle == char_tx_handle + 2) {
		if(att_data[0] == 0x01) {
			notification_enabled = 1;
		}
	}
}

void aci_gatt_attribute_modified_event(uint16_t Connection_Handle,
                                       uint16_t Attr_Handle,
                                       uint16_t Offset,
                                       uint16_t Attr_Data_Length,
                                       uint8_t Attr_Data[])
{
	Attribute_Modify_CB(Attr_Handle, Attr_Data_Length, Attr_Data);
}


void aci_gatt_notification_event(uint16_t Connection_Handle,
                                 uint16_t Attribute_Handle,
                                 uint8_t Attribute_Value_Length,
                                 uint8_t Attribute_Value[])
{
	if(Attribute_Handle == char_tx_handle + 2)
	{
		receive_data(Attribute_Value, Attribute_Value_Length);
	}
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
