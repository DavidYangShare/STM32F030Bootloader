/**
****************************************************************

File	:	sSteer.h
Author	:	@hiyangdong
Version	:	V1.0
date	:	2015-05-13
brief	:	header file

*****************************************************************
*/

#ifndef _SLIGHT_SENSITIVE_H_
#define _SLIGHT_SENSITIVE_H_

#include "stm32f0xx_hal_def.h"

void sLS_Init(void);
uint16_t sLS_Read(uint32_t Compare1);

#endif
