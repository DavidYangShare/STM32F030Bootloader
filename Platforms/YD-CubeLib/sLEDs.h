/**
****************************************************************

File		:	sLEDs.h
Author	:	@hiyangdong
Version	:	V1.0
date		:	2015-05-13
brief		:	information of the file

*****************************************************************
*/

#ifndef _SLEDS_H_
#define _SLEDS_H_

#include "stm32f0xx_hal.h"
#include "main.h"

//typedef enum
//{
//  R,
//  G,
//  B
//}RGB;

//typedef enum
//{
//  ON = 0,
//  OFF
//}LED_State;

void sLEDs_Init(void);
void sLEDs_Write(RGB PinColor, LED_State LEDState);
void sLEDs_Toggle(RGB PinColor);


#endif

/**
****************************************************************

File		:	sLEDs.h
Author	:	@hiyangdong
Version	:	V1.0
date		:	2015-05-13
brief		:	application of the file

*****************************************************************
*/
/*
				switch(i2c_content[0]){
					case 0x01:cLEDs_Write(R,ON);
										break;
					case 0x02:cLEDs_Write(R,OFF);
										break;
					case 0x03:cLEDs_Toggle(R);
										break;
					case 0x04:cLEDs_Write(G,ON);
										break;
					case 0x05:cLEDs_Write(G,OFF);
										break;
					case 0x06:cLEDs_Toggle(G);
										break;
					case 0x07:cLEDs_Write(B,ON);
										break;
					case 0x08:cLEDs_Write(B,OFF);
										break;
					case 0x09:cLEDs_Toggle(B);
										break;
					default:
										break;
				}
*/
