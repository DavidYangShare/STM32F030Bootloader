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
	
  result = Xmodem_Receive();		//�������ݲ�д��Flash
	switch(result)
	{
		//����0x00��������תAPP
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
	//��Ϊ�û�����ʼλ��(0x08008000��)��ǰ4���ֽڴ�ŵ��Ƕ�ջ�ĵ�ַ����ջ��ַ�ض���ָ��RAM�ռ�ģ���STM32��RAM�ռ���ʼ��ַΪ0x20000000������Ҫ�����жϡ�
	if (((*(__IO uint32_t*)APPLICATION_ADDRESS) & 0x2FFE0000 ) == 0x20000000)
	{
		/* Jump to user application */
		//������ת��ַ��ȷ�ϣ�ǰ���Ѿ�˵��0x08008004����4���ֽڴ�ŵ��Ǹ�λ��������ڵ�ַ��
		//�þ����˼Ϊ���(ApplicationAddress + 4)��ַ�������ݣ���Ϊ����µĸ�λ������ڵ�ַ��
		JumpAddress = *(__IO uint32_t*) (APPLICATION_ADDRESS + 4);
		
		//��Jump_To_Application�������ָ��ָ��λ������ڵ�ַ��
		JumpToApplication = (pFunction) JumpAddress;
		
		/* Initialize user application's Stack Pointer */
		//��ջ�ĳ�ʼ���������趨ջ������ַ����ջ����ַ����Ϊ�û�����ָ���ջ����ַ��
		__set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);
		
		//��ת���µĸ�λ������
		JumpToApplication();
	}
}



//    //hand shaked !!!!!�����߼�����
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

