/**
  ******************************************************************************
  * @file    IAP_Main/Src/ymodem.c 
  * @author  MCD Application Team
  * @version 1.0.0
  * @date    8-April-2015
  * @brief   This file provides all the software functions related to the ymodem 
  *          protocol.
  *****************************************************************************/

//����֡��ʽ���������˶���
/* /-------- Packet in IAP memory ------------------------------------------\
 * | 0      |  1    |  2     |  3   |  4      | ... | n+4     | n+5  | n+6  | 
 * |------------------------------------------------------------------------|
 * | unused | start | number | !num | data[0] | ... | data[n] | crc0 | crc1 |
 * \------------------------------------------------------------------------/
 * the first byte is left unused for memory alignment reasons                 */

#include "xmodem.h"
#include "main.h"
#include "boot.h"
#include "flash_if.h"

uint8_t PacketData[PACKET_SIZE + PACKET_DATA_INDEX + PACKET_TRAILER_SIZE];


HAL_StatusTypeDef Xmodem_Receive (void)
{
  uint32_t packet_length, file_done = 0, errors = 0;
  uint32_t flashdestination, ramsource;
  uint8_t packets_received = 1;
  HAL_StatusTypeDef result = HAL_OK;

  /* Initialize flashdestination variable */
  flashdestination = APPLICATION_ADDRESS;
	
	//���Ϳ�ʼ�ź�NAK����ʾ�ۼӺ�У��
	Serial_PutByte(NAK);
	
	//��ʼ��������
	while ((file_done == 0) && (result == HAL_OK))
	{
		//�������ݰ����Ѿ�����ͷ����У�飨�ۼӺͻ���CRC16��
		switch (ReceivePacket(PacketData, &packet_length))
		{
			//���ճɹ�
			case HAL_OK:
				errors = 0;
				switch (packet_length)
				{
					case 0:
						//������״̬���������䣬�ظ�ACK
						Serial_PutByte(ACK);
						file_done = 1;
						result = HAL_OK;
						break;
					//�������յ�128byte����
					default:
						//У��������ź�Ӧ���ܵ����ݵ����
						if (PacketData[PACKET_NUMBER_INDEX] != packets_received)	//У��ʧ�ܣ����·���
						{
							//���·��ͣ������������ش�����һ֡���ݵĻ������ܻ���������ѭ�������Խ�����λ����һ֡���ݷ��ʹ������ơ�
							Serial_PutByte(NAK);
							//�˳���errorsÿ�ζ����ʼ����������³�����BUG
//							errors ++;
//							if (errors > MAX_ERRORS)
//							{
//								/* Abort communication */
//								Serial_PutByte(CAN);
//								result = HAL_TIMEOUT;
//							}
							//end
						}
						
						//��������
						else
						{
							if(packets_received==1)
							{
								FLASH_If_Erase(APPLICATION_ADDRESS);
							}
							//RAM���ݴ�uint8_t ת���ɶ���� uint32_t							
							ramsource = (uint32_t) & PacketData[PACKET_DATA_INDEX];

							//д���ݵ�Flash�����ж���ȷ���
							if (FLASH_If_Write(flashdestination, (uint32_t*) ramsource, packet_length/4) == FLASHIF_OK)                   
							{
								flashdestination += packet_length;
								//����������һ֡
								Serial_PutByte(ACK);
							}
							//���Flash��д������ֹ����
							else
							{
								Serial_PutByte(CAN);
							}
							packets_received ++;
						}
						break;
				}
				break;
			default: 	//����HAL_BUSY,HAL_TIMEOUT,HAL_ERROR�����Խ�����ش��������������˳�
								//���ܻ��У�
								//��ʱ����һλ֡ͷ��һ֡���ݣ�
								//���󣨵�һλ֡ͷ��֡ͷ���ݴ���һ֡���ݣ���Ŵ���CRC����
								//æµ����һλ֡ͷ��һ֡���ݣ�
				if (packets_received > 0)
				{
					errors ++;
				}
				if (errors > MAX_ERRORS)
				{
					/* Abort communication */
					Serial_PutByte(CAN);
					result = HAL_TIMEOUT;
				}
				else
				{
					/* Ask for a resend packet */
					Serial_PutByte(NAK); 
					result = HAL_OK;
				}
				break;
		}
	}
	if(result == HAL_TIMEOUT)
	{
		Serial_PutString("\r\n==Receive Package TIMEOUT==\r\n\n");
	}
  return result;
}

//��Ҫ����status���״̬
static HAL_StatusTypeDef ReceivePacket(uint8_t *p_data, uint32_t *p_length)
{
  uint32_t crc;
  uint16_t packet_size = 0;
  HAL_StatusTypeDef status = HAL_OK;
  uint8_t byte;
	
	//����ͷ���ݣ�һ����SOH��EOT
  status = HAL_UART_Receive(&UartHandle, &byte, 1, FRAME_TIMEOUT);

  if (status == HAL_OK)
  {
    switch (byte)
    {
      case SOH:
        packet_size = PACKET_SIZE;	//��ʼ֡ͷ������128byte����
        break;		
      case EOT:
				packet_size = 0;						//������ɣ�size����
        break;
      default:
        status = HAL_ERROR;					//���մ���size������Ϊ��
        break;
    }
		
		//��һλ�����ã�Ϊ���ڴ���룬�ӵڶ�λ��ʼ
    p_data[PACKET_START_INDEX] = byte;
		
		//�ж��Ƿ���Ҫ����һ֡���ݹ���
    if (packet_size == PACKET_SIZE )
    {
			//�ӵ���λ��ʼ���ܣ��ܹ�2+128+1�������ۼӺ͵�У��
      status = HAL_UART_Receive(&UartHandle, &p_data[PACKET_NUMBER_INDEX], packet_size + PACKET_OVERHEAD_SIZE, FRAME_TIMEOUT);

      /* Simple packet sanity check */
			//�жϽ����Ƿ�ɹ�
      if (status == HAL_OK )
      {
				//������ݰ�����Ƿ���ȷ
        if (p_data[PACKET_NUMBER_INDEX] != ((p_data[PACKET_CNUMBER_INDEX]) ^ 0xFF))
        {
          status = HAL_ERROR;
        }
        else
        {
					//�ۼӺ�У��
					crc = p_data[ packet_size + PACKET_DATA_INDEX ];
          if (CalcChecksum(&p_data[PACKET_DATA_INDEX], packet_size) != crc )
          {
            status = HAL_ERROR;
          }
					
//          //CRC16У��
//          crc = p_data[ packet_size + PACKET_DATA_INDEX ] << 8;
//          crc += p_data[ packet_size + PACKET_DATA_INDEX + 1 ];
//          if (Cal_CRC16(&p_data[PACKET_DATA_INDEX], packet_size) != crc )
//          {
//            status = HAL_ERROR;
//          }
        }
      }
    }
  }
	else if(status == HAL_TIMEOUT)
	{
		Serial_PutString("\r\n=================== Receive Package TIMEOUT ===========\r\n\n");
	}
	else
	{
		Serial_PutString("\r\n=================== Receive Package ERROR ===========\r\n\n");
	}
  *p_length = packet_size;
  return status;
}

/**
  * @brief  Update CRC16 for input byte
  * @param  crc_in input value 
  * @param  input byte
  * @retval None
  */
uint16_t UpdateCRC16(uint16_t crc_in, uint8_t byte)
{
  uint32_t crc = crc_in;
  uint32_t in = byte | 0x100;

  do
  {
    crc <<= 1;
    in <<= 1;
    if(in & 0x100)
      ++crc;
    if(crc & 0x10000)
      crc ^= 0x1021;
  }
  
  while(!(in & 0x10000));

  return crc & 0xffffu;
}

/**
  * @brief  Cal CRC16 for YModem Packet
  * @param  data
  * @param  length
  * @retval None
  */
uint16_t Cal_CRC16(const uint8_t* p_data, uint32_t size)
{
  uint32_t crc = 0;
  const uint8_t* dataEnd = p_data+size;

  while(p_data < dataEnd)
    crc = UpdateCRC16(crc, *p_data++);
 
  crc = UpdateCRC16(crc, 0);
  crc = UpdateCRC16(crc, 0);

  return crc&0xffffu;
}

/**
  * @brief  Calculate Check sum for YModem Packet
  * @param  p_data Pointer to input data
  * @param  size length of input data
  * @retval uint8_t checksum value
  */
uint8_t CalcChecksum(const uint8_t *p_data, uint32_t size)
{
  uint32_t sum = 0;
  const uint8_t *p_data_end = p_data + size;

  while (p_data < p_data_end )
  {
    sum += *p_data++;
  }

  return (sum & 0xffu);
}

/**
  * @brief  Transmit a byte to the HyperTerminal
  * @param  param The byte to be sent
  * @retval HAL_StatusTypeDef HAL_OK if OK
  */
HAL_StatusTypeDef Serial_PutByte(uint8_t param)
{
  /* May be timeouted... */
  if ( UartHandle.State == HAL_UART_STATE_TIMEOUT )
  {
    UartHandle.State = HAL_UART_STATE_READY;
  }
	
  return HAL_UART_Transmit(&UartHandle, &param, 1, TX_TIMEOUT);
}

/**
  * @brief  Print a string on the HyperTerminal
  * @param  p_string: The string to be printed
  * @retval None
  */
void Serial_PutString(uint8_t *p_string)
{
  uint16_t length = 0;

  while (p_string[length] != '\0')
  {
    length++;
  }
  HAL_UART_Transmit(&UartHandle, p_string, length, TX_TIMEOUT);
}

