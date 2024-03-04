/*
 * app_bluenrg.c
 *
 *  Created on: Mar 2, 2024
 *      Author: Engineer Protobetz
 */

#include <stdio.h>
#include "bluenrg_conf.h"
#include "hci.h"
#include "bluenrg1_types.h"
#include "bluenrg1_gap.h"
#include "bluenrg1_aci.h"
#include "bluenrg1_hci_le.h"

#include "app_bluenrg.h"
#include "service.h"

#define BDADDRS_SIZE 6
uint8_t SERVER_BDARR[] = {0X01, 0X02, 0X03, 0X04, 0X05, 0X06};

void blueapp_init(void) {

	tBleStatus ret;
	uint8_t bdaddr[BDADDRS_SIZE];
	const char *name = "Trix";
	uint16_t service_handle, dev_name_char_handle, appearance_char_handle;


	BLUENRG_memcpy(bdaddr, SERVER_BDARR, sizeof(SERVER_BDARR));

	/*
	 * 1. Initialize HCI
	 * 2. Reset HCI
	 * 3. Configure device address
	 * 4. Initialize GAT server
	 * 5. Initialize GAP service
	 * 6. Update Characteristics
	 * 7. Add custom service
	 *
	 */
	hci_init(NULL, NULL);

	hci_reset();

	HAL_Delay(100);

	ret = aci_hal_write_config_data(CONFIG_DATA_PUBADDR_OFFSET, CONFIG_DATA_PUBADDR_LEN, bdaddr);

	if( ret != BLE_STATUS_SUCCESS ) {
		printf("aci_hal_write_config_data : Failed !! \n\r");
	}

	ret = aci_gatt_init();

	if( ret != BLE_STATUS_SUCCESS ) {
		printf("aci_gatt_init : Failed !! \n\r");
	}

	ret = aci_gap_init(GAP_PERIPHERAL_ROLE, 0, 0X07, &service_handle, &dev_name_char_handle, &appearance_char_handle);

	if( ret != BLE_STATUS_SUCCESS ) {
		printf("aci_gap_init : Failed !! \n\r");
	}

	ret = aci_gatt_update_char_value(service_handle, dev_name_char_handle, 0, strlen(name), (uint8_t*)name);

	if( ret != BLE_STATUS_SUCCESS ) {
		printf("aci_gatt_update_char_value : Failed !! \n\r");
	}

	ret = add_simple_service();

	if( ret != BLE_STATUS_SUCCESS ) {
		printf("add_simple_service : Failed !! \n\r");
	}

}

void bluenrg_process(void) {
	tBleStatus ret;

	uint8_t local_name[] = {AD_TYPE_COMPLETE_LOCAL_NAME, 'T', 'B', 'L', 'E', 'E', 'G'};

	ret = aci_gap_set_discoverable(ADV_IND, 0, 0, PUBLIC_ADDR, NO_WHITE_LIST_USE, sizeof(local_name), local_name, 0, NULL, 0, 0);

	if( ret == BLE_STATUS_SUCCESS ) {
		printf("aci_gap_set_discoverable : Success !! \n\r");
	}
}

