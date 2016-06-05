/**
****************************************************************

File	:	sMIC.h
Author	:	@hiyangdong
Version	:	V1.0
date	:	2015-05-13
brief	:	header file

*****************************************************************
*/

#ifndef _SKEY_H_
#define _SKEY_H_

#include "stm32f0xx_hal_def.h"

void sMIC_Init(void);
GPIO_PinState sMIC_Read(void);



#endif
