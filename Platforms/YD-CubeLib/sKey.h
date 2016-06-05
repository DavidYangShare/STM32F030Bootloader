/**
****************************************************************

File	:	sKey.h
Author	:	@hiyangdong
Version	:	V1.0
date	:	2015-05-13
brief	:	information of the file

*****************************************************************
*/

#ifndef _SKEY_H_
#define _SKEY_H_

#include "stm32f0xx_hal.h"
#include "main.h"

void sKey_Init(void);
GPIO_PinState sKey_Read(void);
void sKey_Write(RGB PinColor, LED_State LEDState);
void sKey_Toggle(RGB PinColor);


#endif
