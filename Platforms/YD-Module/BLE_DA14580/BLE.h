/*****************************************************************

File			:	YD-Module\BLE_DA14580\BLE.h
Fuction		:	
Author		:	@hiyangdong
Version		:	V1.0
Time			:	30 Nov. 2015

*****************************************************************/

#ifndef _BLE_H_
#define _BLE_H_

#include "stm32l1xx_hal.h"

#define BLE_UART huart2


#define RESET <RESET>
#define BAUD 	<BAUD19200>


typedef enum
{
  M	= 0x00,		/*Master		*/
  S	=	0x01,		/*Slave			*/
}RoleMode;	

typedef enum
{
  CFG	= 0x00,		/*Config	*/
  TRA	=	0x01,		/*Transparent*/
}CFGMode;

typedef enum
{
  PD	= 0x00,		/*Power down */
  WU	=	0x01,		/*weak up		 */
}WUMode;



void BLE_Role(RoleMode mode);
void BLE_CFG(CFGMode mode);
void BLE_WU(WUMode mode);
uint8_t BLE_CON(void);
uint8_t BLE_Data(void);
HAL_StatusTypeDef BLE_Transmit(uint8_t *pData, uint16_t Size);

void BLE_RESET(void);
void BLE_BAUD(uint32_t value);

#endif
