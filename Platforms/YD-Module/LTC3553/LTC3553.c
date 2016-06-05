/*****************************************************************

File			:	YD-Module\LTC3553\LTC3553.c
Fuction		:	Power management chip
Author		:	@hiyangdong
Version		:	V1.0
Time			:	11 Nov. 2015

*****************************************************************/
#include "ltc3553.h"

#include "gpio.h"
#include "adc.h"


/*****************************************************************
Name			:	void Power_Mode(PowerMode mode)
Fuction		:	Power mode configuration
Input			:	power mode(PA0--LTC_ON Low:OFF ; High:ON)
Output		:	void
Author		:	@hiyangdong
Version		:	V0.0.1
Time			:	11 Nov. 2015
*****************************************************************/
void Power_Mode(PowerMode mode)
{
	switch(mode)
	{
		case OFF:
						HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_RESET);	break;
		case ON:
						HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_SET);		break;
		default:
						HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_SET);		break;
			
	}
}

/*****************************************************************
Name			:	void Power_Standby(PowerMode mode)
Fuction		:	standby mode configuration
Input			:	standby mode(PB3	LTC_STBY  Low:Normal ; High:standby mode 10mA)
Output		:	void
Author		:	@hiyangdong
Version		:	V0.1
Time			:	11 Nov. 2015
*****************************************************************/
void Power_Standby(PowerMode mode)
{
	switch(mode)
	{
		case OFF:
						HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_RESET);	break;
		case ON:
						HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_SET);		break;
		default:
						HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_RESET);		break;
			
	}
}

/*****************************************************************
Name			:	uint8_t Power_Up(void)
Fuction		:	the power up button state
Input			:	void
Output		:	the value of button(PA0	LTC_ON Low:OFF ; High:ON)
Author		:	@hiyangdong
Version		:	V0.1
Time			:	11 Nov. 2015
*****************************************************************/
uint8_t Power_Up(void)
{
	if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==GPIO_PIN_RESET)
		return 0;
	else
		return 1;
}

/*****************************************************************
Name			:	void LDO_Mode(PowerMode mode)
Fuction		:	LDO mode configuration
Input			:	Power mode(PB4	LTC_LDO_ON Low:OFF ; High:ON)
Output		:	void
Author		:	@hiyangdong
Version		:	V0.1
Time			:	11 Nov. 2015
*****************************************************************/
void LDO_Mode(PowerMode mode)
{
	switch(mode)
	{
		case OFF:
						HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET);	break;
		case ON:
						HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET);		break;
		default:
						HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET);		break;
			
	}
}

/*****************************************************************
Name			:	void BUCK_Mode(PowerMode mode)
Fuction		:	BUCK mode configuration
Input			:	Power mode(PB2		LTC_BUCK_ON Low:OFF ; High:ON)
Output		:	void
Author		:	@hiyangdong
Version		:	V0.1
Time			:	11 Nov. 2015
*****************************************************************/
void BUCK_Mode(PowerMode mode)
{
	switch(mode)
	{
		case OFF:
						HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2,GPIO_PIN_RESET);	break;
		case ON:
						HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2,GPIO_PIN_SET);		break;
		default:
						HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2,GPIO_PIN_SET);		break;
			
	}
}

/*****************************************************************
Name			:	void Charger_Current(uint8_t mode)
Fuction		:	Charger current configuration
Input			:	current mode(PC13	LTC_HPWR 0:100mA charger; 1:500mA)
Output		:	void
Author		:	@hiyangdong
Version		:	V0.1
Time			:	11 Nov. 2015
*****************************************************************/
void Charger_Current(uint8_t mode)
{
	if(mode==0)
	{
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_SET);
	}
}

/*****************************************************************
Name			:	uint8_t Charger_State(void)
Fuction		:	Charger state configuration
Input			:	void
Output		:	state mode(PA15	LTC_CHRG 0:charging; 1:completed)
Author		:	@hiyangdong
Version		:	V0.1
Time			:	11 Nov. 2015
*****************************************************************/
uint8_t Charger_State(void)
{

	if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_15)==GPIO_PIN_RESET)
		return 0;
	else
		return 1;
	
}

/*****************************************************************
Name			:	float BatteryVoltage(void)
Fuction		:	Battery Voltage
Input			:	void
Output		:	the real voltage of battery (PA1 ADC_IN1)
Author		:	@hiyangdong
Version		:	V0.1
Time			:	11 Nov. 2015
*****************************************************************/
float BatteryVoltage(void)
{
	float bat_vol = 0;
	HAL_ADC_Start(&hadc);
	while(HAL_ADC_PollForConversion(&hadc,1)!=HAL_OK);
	bat_vol = (float)(HAL_ADC_GetValue(&hadc))/2048*4+0.01;
//	bat_vol = (float)(HAL_ADC_GetValue(&hadc))/2048*3.3;
	return bat_vol;		
}

/************    @YD-STM32L    *    File End Here    ************/
