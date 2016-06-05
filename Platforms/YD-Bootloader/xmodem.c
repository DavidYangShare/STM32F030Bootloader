/**
  ******************************************************************************
  * @file    IAP_Main/Src/ymodem.c 
  * @author  MCD Application Team
  * @version 1.0.0
  * @date    8-April-2015
  * @brief   This file provides all the software functions related to the ymodem 
  *          protocol.
  *****************************************************************************/

//数据帧格式，并且做了对齐
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
	
	//发送开始信号NAK，表示累加和校验
	Serial_PutByte(NAK);
	
	//开始接受数据
	while ((file_done == 0) && (result == HAL_OK))
	{
		//接收数据包，已经包含头检查和校验（累加和或者CRC16）
		switch (ReceivePacket(PacketData, &packet_length))
		{
			//接收成功
			case HAL_OK:
				errors = 0;
				switch (packet_length)
				{
					case 0:
						//正常的状态：结束传输，回复ACK
						Serial_PutByte(ACK);
						file_done = 1;
						result = HAL_OK;
						break;
					//正常接收到128byte数据
					default:
						//校验数据序号和应该受到数据的序号
						if (PacketData[PACKET_NUMBER_INDEX] != packets_received)	//校验失败，重新发送
						{
							//重新发送，但是如果多次重传上上一帧数据的话，可能会在这里死循环，可以建议上位机做一帧数据发送次数限制。
							Serial_PutByte(NAK);
							//此程序errors每次都会初始化，因此以下程序有BUG
//							errors ++;
//							if (errors > MAX_ERRORS)
//							{
//								/* Abort communication */
//								Serial_PutByte(CAN);
//								result = HAL_TIMEOUT;
//							}
							//end
						}
						
						//正常数据
						else
						{
							if(packets_received==1)
							{
								FLASH_If_Erase(APPLICATION_ADDRESS);
							}
							//RAM数据从uint8_t 转换成对齐的 uint32_t							
							ramsource = (uint32_t) & PacketData[PACKET_DATA_INDEX];

							//写数据到Flash，并判断正确与否
							if (FLASH_If_Write(flashdestination, (uint32_t*) ramsource, packet_length/4) == FLASHIF_OK)                   
							{
								flashdestination += packet_length;
								//发送请求下一帧
								Serial_PutByte(ACK);
							}
							//如果Flash读写出错，终止程序
							else
							{
								Serial_PutByte(CAN);
							}
							packets_received ++;
						}
						break;
				}
				break;
			default: 	//对于HAL_BUSY,HAL_TIMEOUT,HAL_ERROR都可以解决：重传，超过次数则退出
								//可能会有：
								//超时（第一位帧头，一帧数据）
								//错误（第一位帧头，帧头数据错误，一帧数据，序号错误，CRC错误）
								//忙碌（第一位帧头，一帧数据）
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

//主要根据status检测状态
static HAL_StatusTypeDef ReceivePacket(uint8_t *p_data, uint32_t *p_length)
{
  uint32_t crc;
  uint16_t packet_size = 0;
  HAL_StatusTypeDef status = HAL_OK;
  uint8_t byte;
	
	//接收头数据，一般是SOH和EOT
  status = HAL_UART_Receive(&UartHandle, &byte, 1, FRAME_TIMEOUT);

  if (status == HAL_OK)
  {
    switch (byte)
    {
      case SOH:
        packet_size = PACKET_SIZE;	//开始帧头，接收128byte数据
        break;		
      case EOT:
				packet_size = 0;						//接收完成，size置零
        break;
      default:
        status = HAL_ERROR;					//接收错误，size本来就为零
        break;
    }
		
		//第一位不适用，为了内存对齐，从第二位开始
    p_data[PACKET_START_INDEX] = byte;
		
		//判断是否需要发送一帧数据过来
    if (packet_size == PACKET_SIZE )
    {
			//从第三位开始接受，总共2+128+1，采用累加和的校验
      status = HAL_UART_Receive(&UartHandle, &p_data[PACKET_NUMBER_INDEX], packet_size + PACKET_OVERHEAD_SIZE, FRAME_TIMEOUT);

      /* Simple packet sanity check */
			//判断接收是否成功
      if (status == HAL_OK )
      {
				//检查数据包序号是否正确
        if (p_data[PACKET_NUMBER_INDEX] != ((p_data[PACKET_CNUMBER_INDEX]) ^ 0xFF))
        {
          status = HAL_ERROR;
        }
        else
        {
					//累加和校验
					crc = p_data[ packet_size + PACKET_DATA_INDEX ];
          if (CalcChecksum(&p_data[PACKET_DATA_INDEX], packet_size) != crc )
          {
            status = HAL_ERROR;
          }
					
//          //CRC16校验
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

