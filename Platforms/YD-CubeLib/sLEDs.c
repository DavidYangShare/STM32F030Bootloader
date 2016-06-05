/**
****************************************************************

File	:	sLEDs.c
Author	:	@hiyangdong
Version	:	V1.0
date	:	2015-05-13
brief	:	information of the file

*****************************************************************
*/

#include "sLEDs.h"

/**
****************************************************************

Function：sLEDs_Init
Author	:	@hiyangdong
Version	:	V1.0
date		:	2015-05-13
brief		:	initialize the RGB pins

*****************************************************************
*/
void sLEDs_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __GPIOA_CLK_ENABLE();

  /*Configure GPIO pin : PA1 PA2 PA3 */
  GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/**
****************************************************************

Function：sLEDs_Write
Author	:	@hiyangdong
Version	:	V1.0
date	:	2015-05-13
brief	:	write the RGB pins

*****************************************************************
*/
void sLEDs_Write(RGB PinColor, LED_State LEDState)
{
	GPIO_PinState PinState;
	if(LEDState==0)
	{
		PinState = GPIO_PIN_RESET;
	}
	else
		PinState = GPIO_PIN_SET;
	if(PinColor==R)
	{
		  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,PinState);
	}
	else if(PinColor==G)
	{
		  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,PinState);
	}	
	else if(PinColor==B)
	{
		  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,PinState);
	}
}

/**
****************************************************************

Function：sLEDs_Toggle
Author	:	@hiyangdong
Version	:	V1.0
date	:	2015-05-13
brief	:	Toggle the RGB pins

*****************************************************************
*/
void sLEDs_Toggle(RGB PinColor)
{
	if(PinColor==R)
	{
		  HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_2);
	}
	else if(PinColor==G)
	{
		  HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_1);
	}	
	else if(PinColor==B)
	{
		  HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_3);
	}
}
