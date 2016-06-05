/**
  ******************************************************************************
  * @file    IAP_Main/Src/ymodem.c 
  * @author  MCD Application Team
  * @version 1.0.0
  * @date    8-April-2015
  * @brief   This file provides all the software functions related to the ymodem 
  *          protocol.
  *****************************************************************************/

#include "boot.h"
#include "xmodem.h"

pFunction JumpToApplication;
uint32_t JumpAddress;

void Boot(void)
{
	HAL_StatusTypeDef result;
	
  result = Xmodem_Receive();		//接收数据并写入Flash
	switch(result)
	{
		//发送0x00，并且跳转APP
		case HAL_OK:			Serial_PutByte(HAL_OK);Boot2APP();break;
		case HAL_ERROR:   Serial_PutByte(HAL_ERROR);				break;
		case HAL_BUSY:		Serial_PutByte(HAL_BUSY);					break;
		case HAL_TIMEOUT:	Serial_PutByte(HAL_TIMEOUT);			break;
		default:					Boot2APP();break;
	}

}

void Boot2APP(void)
{
	HAL_UART_MspDeInit(&UartHandle);

	/* Test if user code is programmed starting from address "APPLICATION_ADDRESS" */
	//因为用户程序开始位置(0x08008000处)的前4个字节存放的是堆栈的地址，堆栈地址必定是指向RAM空间的，而STM32的RAM空间起始地址为0x20000000，所以要进行判断。
	if (((*(__IO uint32_t*)APPLICATION_ADDRESS) & 0x2FFE0000 ) == 0x20000000)
	{
		/* Jump to user application */
		//程序跳转地址的确认，前面已经说过0x08008004处的4个字节存放的是复位函数的入口地址，
		//该句的意思为获得(ApplicationAddress + 4)地址处的数据，即为获得新的复位函数入口地址。
		JumpAddress = *(__IO uint32_t*) (APPLICATION_ADDRESS + 4);
		
		//令Jump_To_Application这个函数指针指向复位函数入口地址。
		JumpToApplication = (pFunction) JumpAddress;
		
		/* Initialize user application's Stack Pointer */
		//堆栈的初始化，重新设定栈顶代地址，把栈顶地址设置为用户代码指向的栈顶地址。
		__set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);
		
		//跳转到新的复位函数。
		JumpToApplication();
	}
}



//    //hand shaked !!!!!还有逻辑问题
//		num = 10;
//    while(num)
//		{
//			state = HAL_UART_Transmit(&UartHandle, "IamReady\r\n", 10, TX_TIMEOUT);
//			state = HAL_UART_Receive(&UartHandle, &data, 1, RX_TIMEOUT);
//			if(state == HAL_OK)
//			{
//				break;
//			}
//			else
//			{
//				num--
//			}
//		}

