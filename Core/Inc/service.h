/*
 * service.h
 *
 *  Created on: Mar 3, 2024
 *      Author: Engineer Protobetz
 */

#ifndef INC_SERVICE_H_
#define INC_SERVICE_H_

tBleStatus add_services(void);
void APP_UserEvtRx(void *pData);
void send_data(uint8_t* new_data, uint8_t no_bytes);


#endif /* INC_SERVICE_H_ */
