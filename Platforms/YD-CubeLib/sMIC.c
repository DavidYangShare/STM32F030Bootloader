/**
****************************************************************

File	:	sKey.c
Author	:	@hiyangdong
Version	:	V1.0
date	:	2015-05-13
brief	:	code file

*****************************************************************
*/

#include "sKey.h"

/**
****************************************************************

Function：	sMIC_Init
Author	:	@hiyangdong
Version	:	V1.0
date	:	2015-05-13
brief	:	initialize the pins

*****************************************************************
*/
void sMIC_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __GPIOA_CLK_ENABLE();

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/**
****************************************************************

Function：	sMIC_Read
Author	:	@hiyangdong
Version	:	V1.0
date	:	2015-05-13
brief	:	Read pins

*****************************************************************
*/
GPIO_PinState sMIC_Read(void)
{
	
	return HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);
	
}

