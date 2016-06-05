/**
****************************************************************

File	:	sSteer.h
Author	:	@hiyangdong
Version	:	V1.0
date	:	2015-05-13
brief	:	header file

*****************************************************************
*/

#ifndef _SSTEER_H_
#define _SSTEER_H_

#include "stm32f0xx_hal_def.h"

void sSteer_Init(void);
void sSteer_Write(uint32_t Compare1);

#endif
