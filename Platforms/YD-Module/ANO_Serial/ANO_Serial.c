/*****************************************************************

File		:	YD-Module\SerialScope\SerialScope.c
Fuction		:	Serial digital Scope
Author		:	@hiyangdong
Version		:	V0.1
Time		:	01 Dec. 2015

*****************************************************************/
#include "ano_serial.h"



//void ANO_DT_Send_Power(uint8_t* array)
//{
//	uint8_t TxBuf[10];
//	uint8_t temp,sum;
//	
//	TxBuf[0]=0x88;
//	TxBuf[1]=0xA1;
//	TxBuf[2]=0x04;
//		
//	TxBuf[3] = array[3];	
//	TxBuf[4] = array[2];	
//	TxBuf[5] = array[1];
//	array[8] = 0.5 * array[0] + 0.5 * array[8];
//	TxBuf[6] = heartRate[0];

//	for(uint8_t i=0;i<7;i++)
//		sum += TxBuf[i];
//	
//	TxBuf[7]=sum;
//	
//	HAL_UART_Transmit(&huart2 , TxBuf, 8, 100);
//}

