/*****************************************************************

File			:	YD-Module\BLE_DA14580\BLE.c
Fuction		:	Power management chip
Author		:	@hiyangdong
Version		:	V1.0
Time			:	30 Nov. 2015

*****************************************************************/
#include "BLE.h"

#include "gpio.h"
#include "usart.h"

/*****************************************************************
Name			:	void BLE_Role(RoleMode mode)
Fuction		:	the BLE role select
Input			:	BLE role mode (PA5	BLE_SCK Low:Master ; High:Slave)
Output		:	void
Author		:	@hiyangdong
Version		:	V0.1
Time			:	30 Nov. 2015
*****************************************************************/
void BLE_Role(RoleMode mode)
{
	switch(mode)
	{
		case M:
						HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_RESET);	break;
		case S:
						HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_SET);		break;
		default:
						HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_SET);		break;
			
	}
}

/*****************************************************************
Name			:	void BLE_CFG(CFGMode mode)
Fuction		:	the BLE config and transparent serial mode
Input			:	BLE CFG mode (PA6	BLE_MOD Low:Config ; High:Transparent serial)
Output		:	void
Author		:	@hiyangdong
Version		:	V0.1
Time			:	30 Nov. 2015
*****************************************************************/
void BLE_CFG(CFGMode mode)
{
	switch(mode)
	{
		case CFG:
						HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_RESET);	break;
		case TRA:
						HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_SET);		break;
		default:
						HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_SET);		break;
			
	}
}

/*****************************************************************
Name			:	void BLE_WU(CFGMode mode)
Fuction		:	the BLE wake up & power down
Input			:	BLE power mode (PB0	BLE_WU Low:weak up ; High:power down)
Output		:	void
Author		:	@hiyangdong
Version		:	V0.1
Time			:	30 Nov. 2015
*****************************************************************/
void BLE_WU(WUMode mode)
{
	switch(mode)
	{
		case WU:
						HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET);	break;
		case PD:
						HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);		break;
		default:
						HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);		break;
			
	}
}

/*****************************************************************
Name			:	uint8_t BLE_CON(void)
Fuction		:	the BLE connect state
Input			:	void
Output		:	the value of connect state
						(PA4	BLE_CON Low:connected ; High:disconnected)
Author		:	@hiyangdong
Version		:	V0.1
Time			:	30 Nov. 2015
*****************************************************************/
uint8_t BLE_CON(void)
{
	if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4)==GPIO_PIN_RESET)
		return 0;
	else
		return 1;
}

/*****************************************************************
Name			:	uint8_t BLE_Data(void)
Fuction		:	the BLE data received state
Input			:	void
Output		:	the value of data received state
						(PA7 BLE_Data Low:data received and delay 10ms ; High: no data)
Author		:	@hiyangdong
Version		:	V0.1
Time			:	30 Nov. 2015
*****************************************************************/
uint8_t BLE_Data(void)
{
	if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_7)==GPIO_PIN_RESET)
		return 0;
	else
		return 1;
}

/*****************************************************************
Name			:	HAL_StatusTypeDef BLE_Transmit(uint8_t *pData, uint16_t Size)
Fuction		:	the BLE transmit data
Input			:	the data that need to transmit 
Output		:	HAL Status of uart						
Author		:	@hiyangdong
Version		:	V0.1
Time			:	30 Nov. 2015
*****************************************************************/
HAL_StatusTypeDef BLE_Transmit(uint8_t *pData, uint16_t Size)
{
	HAL_StatusTypeDef status;
	status = HAL_UART_Transmit(&BLE_UART,pData,Size,1000);
//	HAL_UART_Transmit_DMA(&BLE_UART,pData,Size);
	return status;
}

/*****************************************************************
Name			:	void BLE_RESET(void)
Fuction		:	reset the BLE
Input			:	void
Output		:	void						
Author		:	@hiyangdong
Version		:	V0.1
Time			:	30 Nov. 2015
*****************************************************************/
void BLE_RESET(void)
{
	BLE_CFG(CFG);
	printf("<RESET>");
	BLE_CFG(TRA);
}

/*****************************************************************
Name			:	void UART_BAUD(uint32_t value)
Fuction		:	config the uart baud
Input			:	baud value
Output		:	void						
Author		:	@hiyangdong
Version		:	V0.1
Time			:	30 Nov. 2015
*****************************************************************/
void UART_BAUD(uint32_t value)
{
#if BLE_UART == huart1
  BLE_UART.Instance = USART1;
#elif BLE_UART == huart2
	BLE_UART.Instance = USART2;
#endif
  BLE_UART.Init.BaudRate = value;
  BLE_UART.Init.WordLength = UART_WORDLENGTH_8B;
  BLE_UART.Init.StopBits = UART_STOPBITS_1;
  BLE_UART.Init.Parity = UART_PARITY_NONE;
  BLE_UART.Init.Mode = UART_MODE_TX_RX;
  BLE_UART.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  BLE_UART.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&BLE_UART);
}

/*****************************************************************
Name			:	void BLE_BAUD(uint32_t value)
Fuction		:	config the Baud
Input			:	baud value
Output		:	void						
Author		:	@hiyangdong
Version		:	V0.1
Time			:	30 Nov. 2015
*****************************************************************/
void BLE_BAUD(uint32_t value)
{

	BLE_CFG(CFG);
	HAL_Delay(100);
	if(value == 4800) 	HAL_UART_Transmit(&huart2 , "<BAUD4800>",   12, 100);	
	if(value == 9600) 	HAL_UART_Transmit(&huart2 , "<BAUD9600>",   12, 100);
	if(value == 19200)	HAL_UART_Transmit(&huart2 , "<BAUD19200>",  12, 100);
	if(value == 38400)	HAL_UART_Transmit(&huart2 , "<BAUD38400>",  12, 100);
	if(value == 57600)	HAL_UART_Transmit(&huart2 , "<BAUD57600>",  12, 100);
	if(value == 115200) HAL_UART_Transmit(&huart2 , "<BAUD115200>", 12, 100);
	if(value == 256000)	HAL_UART_Transmit(&huart2 , "<BAUD256000>", 12, 100);
	HAL_Delay(100);
	BLE_CFG(TRA);
  huart2.Init.BaudRate = value;
  HAL_UART_Init(&huart2);
}



