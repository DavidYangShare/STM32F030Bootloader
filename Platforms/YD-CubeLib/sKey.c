/**
****************************************************************

File	:	sKey.c
Author	:	@hiyangdong
Version	:	V1.0
date	:	2015-05-13
brief	:	information of the file

*****************************************************************
*/

#include "sKey.h"

/**
****************************************************************

Function：	cKey_Init
Author	:	@hiyangdong
Version	:	V1.0
date	:	2015-05-13
brief	:	initialize the RGB pins

*****************************************************************
*/
void sKey_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __GPIOA_CLK_ENABLE();

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  /*Configure GPIO pin : PA5 PA6 PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/**
****************************************************************

Function：	sKey_Read
Author	:	@hiyangdong
Version	:	V1.0
date	:	2015-05-13
brief	:	write the RGB pins

*****************************************************************
*/
GPIO_PinState sKey_Read(void)
{
	
	return HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);
	
}

/**
****************************************************************

Function：	sKey_Write
Author	:	@hiyangdong
Version	:	V1.0
date	:	2015-05-13
brief	:	write the RGB pins

*****************************************************************
*/
void sKey_Write(RGB PinColor, LED_State LEDState)
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
		  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,PinState);
	}	
	else if(PinColor==B)
	{
		  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,PinState);
	}
}

/**
****************************************************************

Function：	sKey_Toggle
Author	:	@hiyangdong
Version	:	V1.0
date	:	2015-05-13
brief	:	Toggle the RGB pins

*****************************************************************
*/
void sKey_Toggle(RGB PinColor)
{
	if(PinColor==R)
	{
		  HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_2);
	}
	else if(PinColor==G)
	{
		  HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_3);
	}	
	else if(PinColor==B)
	{
		  HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_1);
	}
}
