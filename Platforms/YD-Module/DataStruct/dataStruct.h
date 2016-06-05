/*****************************************************************

File			:	YD-Module\BLE_DA14580\BLE.h
Fuction		:	
Author		:	@hiyangdong
Version		:	V1.0
Time			:	30 Nov. 2015

*****************************************************************/

#ifndef _DATASTRUCT_H_
#define _DATASTRUCT_H_

#include "stm32f0xx_hal.h"

#define I2C_DATA_MAX	16 
#define DATA_MAX			I2C_DATA_MAX
#define COUNT(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))

typedef enum
{
  READ	= 0x00,		/*Master		*/
  WRITE	=	0x01,		/*Slave			*/
}Command;	


typedef struct DataFormat
{
	uint8_t data_read[I2C_DATA_MAX];
	uint8_t data_send[I2C_DATA_MAX];
	HAL_StatusTypeDef status;
	uint8_t flag;
	uint16_t Addr_hard;
	uint8_t Addr_soft[4];
	uint8_t  length;
	uint8_t  command;
	uint8_t*  content[6];
	uint16_t crc[6];	
}I2C_DataType;


uint16_t getAddr_hard(uint8_t* p);
uint8_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint8_t BufferLength);

#endif
