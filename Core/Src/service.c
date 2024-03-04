/*
 * service.c
 *
 *  Created on: Mar 3, 2024
 *      Author: Engineer Protobetz
 */

#include <stdio.h>
#include "bluenrg1_gap.h"
#include "bluenrg1_gatt_aci.h"
#include "service.h"

uint8_t SERVER_UUID[16] = {0X66, 0X9a, 0X03, 0X54, 0X55, 0XC6, 0X66, 0X9a, 0X03, 0X54, 0X55, 0XC6, 0X66, 0X9a, 0X03, 0X54};
uint8_t CHAR_UUID[16] =   {0X76, 0XAA, 0XCD, 0XE1, 0X10, 0X0F, 0X15, 0X2a, 0X71, 0X99, 0X42, 0XB6, 0XAA, 0X73, 0X00, 0X8F};

uint16_t my_service_handle;
uint16_t my_char_handle;

tBleStatus add_simple_service(void) {
	tBleStatus ret;
	Service_UUID_t service_uuid;
	Char_UUID_t char_uuid;
	BLUENRG_memcpy(service_uuid.Service_UUID_128, SERVER_UUID, 16);

	/* Add Service */

	ret = aci_gatt_add_service(UUID_TYPE_128, &service_uuid, PRIMARY_SERVICE, 7, &my_service_handle);

	if( ret != BLE_STATUS_SUCCESS ) {
		printf("aci_gatt_add_service : Failed !! \n\r");
	}

	BLUENRG_memcpy(char_uuid.Char_UUID_128, CHAR_UUID, 16);

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
	ret = aci_gatt_add_char(my_service_handle, UUID_TYPE_128, &char_uuid, 1, CHAR_PROP_NOTIFY, ATTR_PERMISSION_NONE, 0, 0, 0, &my_char_handle);

	return ret;
}
