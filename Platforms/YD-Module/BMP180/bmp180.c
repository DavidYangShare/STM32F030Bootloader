/*****************************************************************

File			:	YD-Module\BMP180\bmp180.c
Fuction		:	BMP180 chip with Digital pressure sensor
Author		:	@hiyangdong
Version		:	V1.0
Time			:	12 Nov. 2015
Thanks		:	https://github.com/LonelyWolf/stm32/tree/master/bmp180

*****************************************************************/
#include "bmp180.h"

#include "i2c.h"

#include <math.h>

BMP180_Calibration_TypeDef bmpCal;
//uint8_t BMP_value[10];



void BMP180_Init(void) 
{
	MX_I2C1_Init();
}


void BMP180_Reset(void)
{
	BMP180_WriteReg(BMP180_SOFT_RESET_REG,0xb6); // Do software reset
}


HAL_StatusTypeDef BMP180_WriteReg(uint8_t reg, uint8_t value)
{
	uint8_t rxData[2] = {reg,value};
	HAL_StatusTypeDef status;
	
	status = HAL_I2C_Master_Transmit(&BMPi2c,BMP180_ADDR,rxData,2,2);
	while(status != HAL_OK)
  {
		if(HAL_I2C_GetError(&BMPi2c) != HAL_I2C_ERROR_AF)
		{}
	}
	return status;
}


uint8_t BMP180_ReadReg(uint8_t reg)
{
	uint8_t value;
	
	while(HAL_I2C_Master_Transmit(&BMPi2c,BMP180_ADDR,&reg,1,100) != HAL_OK)
	{
			if(HAL_I2C_GetError(&BMPi2c) != HAL_I2C_ERROR_AF)
			{}
	}
 
	if(HAL_I2C_Master_Receive(&BMPi2c,BMP180_ADDR+1,&value,1,100) != HAL_OK)
	{
			if(HAL_I2C_GetError(&BMPi2c) != HAL_I2C_ERROR_AF)
			{}
	}
	return value;
}


uint16_t BMP180_ReadTwoByte(uint8_t reg)
{
    uint8_t value[2];
	  int16_t regData = 0;
	
    while(HAL_I2C_Master_Transmit(&BMPi2c,BMP180_ADDR,&reg,1,2) != HAL_OK)
    {
        if(HAL_I2C_GetError(&BMPi2c) != HAL_I2C_ERROR_AF)
        {}
    }
   
    if(HAL_I2C_Master_Receive(&BMPi2c,BMP180_ADDR+1,value,2,2) != HAL_OK)
    {
        if(HAL_I2C_GetError(&BMPi2c) != HAL_I2C_ERROR_AF)
        {}
    }
		
		regData = (value[0]<<8)+value[1];
		
    return regData;
}


/*读取BMP180的校准系数*/
void BMP180_ReadCalibration(void)
{
	
	bmpCal.AC1 = BMP180_ReadTwoByte(0xaa);
	bmpCal.AC2 = BMP180_ReadTwoByte(0xac);
	bmpCal.AC3 = BMP180_ReadTwoByte(0xae);
	bmpCal.AC4 = BMP180_ReadTwoByte(0xb0);
	bmpCal.AC5 = BMP180_ReadTwoByte(0xb2);
	bmpCal.AC6 = BMP180_ReadTwoByte(0xb4);
	bmpCal.B1  = BMP180_ReadTwoByte(0xb6);
	bmpCal.B2  = BMP180_ReadTwoByte(0xb8);
	bmpCal.MB  = BMP180_ReadTwoByte(0xba);
	bmpCal.MC  = BMP180_ReadTwoByte(0xbc);
	bmpCal.MD  = BMP180_ReadTwoByte(0xbe);
	

}

/*读BMP180没有经过补偿的温度值*/
uint16_t BMP180_Read_UT(void)
{
	uint16_t UT;

	BMP180_WriteReg(BMP180_CTRL_MEAS_REG,BMP180_T_MEASURE);
	HAL_Delay(5);				// Wait for 4.5ms by datasheet
	UT = BMP180_ReadTwoByte(0xf6);
	return UT;
}


/*读BMP180没有经过补偿的压力值*/
uint32_t BMP180_Read_UP(uint8_t oss) 
{
	uint32_t UP;
	uint8_t cmd,delay;

	switch(oss) {
	case 0:
		cmd = BMP180_P0_MEASURE;
		delay   = 6;
		break;
	case 1:
		cmd = BMP180_P1_MEASURE;
		delay   = 9;
		break;
	case 2:
		cmd = BMP180_P2_MEASURE;
		delay   = 15;
		break;
	case 3:
		cmd = BMP180_P3_MEASURE;
		delay   = 27;
		break;
	}
	
	BMP180_WriteReg(BMP180_CTRL_MEAS_REG,cmd); 		 
	HAL_Delay(delay);	   									//wait some time
	
	UP = BMP180_ReadTwoByte(0xf6); 
	UP &= 0x0000FFFF;
	
	return UP;		
}

/*把未经过补偿的温度和压力值转换为实际的温度和压力值
 *True_Temp:实际温度值,单位:℃
 *True_Press:时间压力值,单位:Pa
 *True_Altitude:实际海拔高度,单位:m
*/

float BMP180_Calc_RT(uint16_t UT) 
{
	float	Temp = 0;
	float True_Temp = 0;

	bmpCal.B5  = (((int32_t)UT - (int32_t)bmpCal.AC6) * (int32_t)bmpCal.AC5) >> 15;
	bmpCal.B5 += ((int32_t)bmpCal.MC << 11) / (bmpCal.B5 + bmpCal.MD);
	
	Temp = (bmpCal.B5 + 8) >> 4;
	True_Temp = Temp/10.0;
	printf("Temperature:%.1f \r\n",True_Temp);
	
	return True_Temp;
}

float BMP180_Calc_RP(uint32_t UP, uint8_t oss) 
{
	int32_t B3,B6,X3,p;
	uint32_t B4,B7;
	float True_Press = 0;
	
	B6 = bmpCal.B5 - 4000;
	X3 = ((bmpCal.B2 * ((B6 * B6) >> 12)) >> 11) + ((bmpCal.AC2 * B6) >> 11);
	B3 = (((((int32_t)bmpCal.AC1) * 4 + X3) << oss) + 2) >> 2;
	X3 = (((bmpCal.AC3 * B6) >> 13) + ((bmpCal.B1 * ((B6 * B6) >> 12)) >> 16) + 2) >> 2;
	B4 = (bmpCal.AC4 * (uint32_t)(X3 + 32768)) >> 15;
	B7 = ((uint32_t)UP - B3) * (50000 >> oss);
	if (B7 < 0x80000000) 
		p = (B7 << 1) / B4; 
	else 
		p = (B7 / B4) << 1;
	
	True_Press = p + (((((p >> 8) * (p >> 8) * BMP180_PARAM_MG) >> 16) + ((BMP180_PARAM_MH * p) >> 16) + BMP180_PARAM_MI) >> 4);
	
	printf("Press:%.1fPa \r\n",True_Press);
	
	return True_Press;
}


float BMP180_kpa_to_altitude(float Pa)
{
	float True_Altitude = 0;
	
	True_Altitude = 44330*(1-pow((Pa/101325.0),(1.0/5.255)))/1000;
	
	printf("Altitude:%.3fm \r\n",True_Altitude);	
	
	return True_Altitude;
}
	

// Fast integer Pa -> mmHg conversion (Pascals to millimeters of mercury)
float BMP180_kpa_to_mmhg(int32_t Pa)
{
	float mmHg = 0;
	
	mmHg = (float)Pa * 76.0 / 10130.0;
	printf("mmHg:%.3fm \t",mmHg);
	
	return mmHg;
}


