/*****************************************************************

File			:	YD-Module\SerialScope\SerialScope.h
Fuction		:	Serial digital Scope
Author		:	@hiyangdong
Version		:	V0.1
Time			:	01 Dec. 2015

*****************************************************************/
#ifndef _SERIALSCOPE_H_
#define _SERIALSCOPE_H_

#include "stm32l1xx_hal.h"
#include "usart.h"
#include "BLE.h"


void SerialOut(uint16_t* data);	
unsigned short CRC_CHECK(unsigned char *Buf, unsigned char CRC_CNT);	

#endif  


