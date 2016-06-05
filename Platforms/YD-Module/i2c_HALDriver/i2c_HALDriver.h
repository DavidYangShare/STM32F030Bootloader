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

#ifndef _I2C_HALDriver_H_
#define _I2C_HALDriver_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx_hal.h"

HAL_StatusTypeDef I2C_WriteByte(I2C_HandleTypeDef *hi2c,uint8_t addr, uint8_t reg,uint8_t data);
uint8_t I2C_ReadByte(I2C_HandleTypeDef *hi2c,uint8_t addr, uint8_t reg);
HAL_StatusTypeDef I2C_ReadBytes(I2C_HandleTypeDef *hi2c,uint8_t addr, uint8_t reg, uint8_t *pData, uint16_t size);

void Single_WriteI2C(uint8_t REG_Address,uint8_t REG_data);
void Single_WriteBMPI2C(uint8_t REG_Address,uint8_t REG_data);
uint8_t Single_ReadI2C(uint8_t REG_Address);
uint8_t Single_ReadBMPI2C(uint8_t REG_Address);
int16_t Double_ReadI2C(uint8_t REG_Address);

void Single_Write(uint8_t DEV_Address,uint8_t REG_Address,uint8_t REG_data);
uint8_t Single_Read(uint8_t DEV_Address,uint8_t REG_Address);
#endif
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

