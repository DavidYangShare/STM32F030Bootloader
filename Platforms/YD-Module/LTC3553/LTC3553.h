/*********************************************************************************************************
*
* File                : 24c02.h
* Hardware Environment: 
* Build Environment   : RealView MDK-ARM  Version: 4.20
* Version             : V1.0
* By                  : 
*
*                                  (c) Copyright 2005-2011, WaveShare
*                                       http://www.waveshare.net
*                                          All Rights Reserved
*
*********************************************************************************************************/

#ifndef _LTC3553_H_
#define _LTC3553_H_

#include "stm32l1xx_hal.h"

typedef enum
{
  OFF	= 0x00,		/*Power off */
  ON	=	0x01,		/*Power on  */
//	SB	= 0x02,		/*Power standby */
}PowerMode;	

void Power_Mode(PowerMode mode);

void Power_Standby(PowerMode mode);

uint8_t Power_Up(void);

void LDO_Mode(PowerMode mode);

void BUCK_Mode(PowerMode mode);

void Charger_Current(uint8_t mode);

uint8_t Charger_State(void);

float BatteryVoltage(void);

#endif
