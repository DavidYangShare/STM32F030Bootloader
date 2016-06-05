/**
****************************************************************

File	:	sKey.c
Author	:	@hiyangdong
Version	:	V1.0
date	:	2015-05-13
brief	:	code file

*****************************************************************
*/

#include "sSteer.h"

/**
****************************************************************

Function：	sSteer_Init
Author	:	@hiyangdong
Version	:	V1.0
date	:	2015-05-13
brief	:	initialize the pins

*****************************************************************
*/
void sSteer_Init(void)
{

	TIM_OC_InitTypeDef sConfigOC;

	htim14.Instance = TIM14;
	htim14.Init.Prescaler = 79;
	htim14.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim14.Init.Period = 0;
	htim14.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_Base_Init(&htim14);

	HAL_TIM_PWM_Init(&htim14);

	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 100;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	HAL_TIM_PWM_ConfigChannel(&htim14, &sConfigOC, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim14, TIM_CHANNEL_1);

    /* Peripheral clock enable */
    __TIM14_CLK_ENABLE();
  
    /**TIM14 GPIO Configuration    
    PA4     ------> TIM14_CH1 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF4_TIM14;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/**
****************************************************************

Function：	sSteer_Write
Author	:	@hiyangdong
Version	:	V1.0
date	:	2015-05-13
brief	:	Read pins

*****************************************************************
*/
void sSteer_Write(uint32_t Compare1)
{

  /* Set the Capture Compare1 Register value */
  TIM14->CCR1 = Compare1;
  
}
