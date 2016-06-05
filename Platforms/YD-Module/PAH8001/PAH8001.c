/*****************************************************************

File			:	YD-Module\PAH8001\PAH8001.c
Fuction		:	Power management chip
Author		:	@hiyangdong
Version		:	V0.1
Time			:	1 Dec. 2015

*****************************************************************/
#include "PAH8001.h"

uint8_t PAH8001_Int(void)
{
	GPIO_PinState bitstatus;
	bitstatus = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_7);			//PB7		PAH_INT
	return (uint8_t) bitstatus;
}

HAL_StatusTypeDef PAH8001_writeRegister(uint8_t reg, uint8_t data)
{
	
	HAL_StatusTypeDef status;
	
	status = I2C_WriteByte(&PAHi2c, PAHAddr,reg,data);
	
	return status;
	
}

uint8_t PAH8001_readRegister(uint8_t reg)
{
	uint8_t REG_data;
	
	REG_data = I2C_ReadByte(&PAHi2c, PAHAddr, reg);
	
	return REG_data;
}

void PAH8001_init(void)
{
	uint8_t temp;
	
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_RESET);			//PB7 PAH hardware reset
	HAL_Delay(1000); //about 100ms
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_SET);	
	HAL_Delay(1000); //about 100ms
//Lower Bandwidth (20Hz) or Higher Performance (200Hz) can switch by Address 0x52
	PAH8001_writeRegister(0x06,0x82);			//Reset
	HAL_Delay(100); //about 100ms
		
	PAH8001_writeRegister(0x09,0x5A);			//enable read and write 
	PAH8001_writeRegister(0x05,0xA8); 	//for 3000 fps
	//PAH8001_writeRegister(0x05,0x99);			//1000fps
	temp = PAH8001_readRegister(0x17);			//unknown
	PAH8001_writeRegister(0x17,temp|0x80);	//unknown
	PAH8001_writeRegister(0x27,0xFF);			//unknown
	PAH8001_writeRegister(0x28,0xFA);			//unknown
	PAH8001_writeRegister(0x29,0x0A);			//unknown
	PAH8001_writeRegister(0x2A,0xC8);			//unknown
	PAH8001_writeRegister(0x2B,0xA0);			//unknown
	PAH8001_writeRegister(0x2C,0x8C);			//unknown
	PAH8001_writeRegister(0x2D,0x64);			//unknown
	PAH8001_writeRegister(0x42,0x20);			//unknown
	PAH8001_writeRegister(0x48,0x00);			//unknown
	
	//PAH8001_writeRegister(0x4D,0x1A);			//for wrist application
	PAH8001_writeRegister(0x4D,0x18);		//for fingertip application
	
	PAH8001_writeRegister(0x7A,0xB5);			//unknown
	
	PAH8001_writeRegister(0x7F,0x01);			//unknown
	PAH8001_writeRegister(0x07,0x48);			//unknown
	PAH8001_writeRegister(0x23,0x3C);			//Touch Threshold
	PAH8001_writeRegister(0x26,0x0F);			//Touch Threshold of Strong Light High
	PAH8001_writeRegister(0x2E,0x48);			//unknown
	PAH8001_writeRegister(0x38,0xEA);			//LED_Step current,0xEA equals 15mA
	PAH8001_writeRegister(0x42,0xA4);			//unknown
	PAH8001_writeRegister(0x43,0x41);			//unknown
	PAH8001_writeRegister(0x44,0x41);			//unknown
	PAH8001_writeRegister(0x45,0x24);			//unknown
	PAH8001_writeRegister(0x46,0xC0);			//unknown
	//PAH8001_writeRegister(0x52,0x32); 			//20Hz
	PAH8001_writeRegister(0x52,0x05); 		//200Hz
	PAH8001_writeRegister(0x53,0x28);			//unknown
	PAH8001_writeRegister(0x56,0x60);			//unknown
	PAH8001_writeRegister(0x57,0x28);			//unknown
	PAH8001_writeRegister(0x6D,0x02);			//unknown
	PAH8001_writeRegister(0x0F,0xC8);			//unknown
	PAH8001_writeRegister(0x7F,0x00);			//unknown
	PAH8001_writeRegister(0x5D,0x81);			//unknown
	
	PAH8001_writeRegister(0x0A,0xF1);			//@hiyangdong Sleep1_Setting set frequency time
	
}

int8_t PAH8001_HRD(void)
{

	uint8_t temp=0;

	//Check Touch Status for power saving
	temp = PAH8001_readRegister(0x59)&0x80;	//Touch_Detection

	if(temp==0x80) //Touch status
	{
		PAH8001_writeRegister(0x05,0xA8); 	//for 3000 fps
		//PAH8001_writeRegister(0x05,0x98); 		//for 1000fps
		PAH8001_writeRegister(0x7f,0x01); 		//bank1
		PAH8001_writeRegister(0x42,0xA4);
	}
	else //No touch status to enter sleep mode
	{
		PAH8001_writeRegister(0x05,0xB8);
		PAH8001_writeRegister(0x7f,0x01); 		//bank1
		PAH8001_writeRegister(0x42,0xA0);
	}

	temp = PAH8001_readRegister(0x68)&0x0f; //check ready status: 0 is not ready, 1 is ready, 2 is loss one data?
	if(temp == 0)
	{
		PAH8001_writeRegister(0x7F,0x00); //bank0
		return 0;
	}
	else
//	while(temp == 0)
//	{
//		temp = PAH8001_readRegister(0x68)&0x0f;
//	}
	{
		//Only support burst read (0x64~0x67), when using I2C interface
		I2C_ReadBytes(&PAHi2c, PAHAddr, 0x65,&heartRate[0],3);

//		heartRate[0] = PAH8001_readRegister(0x64);
//		heartRate[1] = PAH8001_readRegister(0x65);
//		heartRate[2] = PAH8001_readRegister(0x66);
//		heartRate[3] = PAH8001_readRegister(0x67);

		//Only support burst read (0x1A~0x1C), when using I2C interface
//		I2C_ReadBytes(&PAHi2c, PAHAddr, 0x1A,&heartRate[4],3);
//		heartRate[4] = PAH8001_readRegister(0x1A);
//		heartRate[5] = PAH8001_readRegister(0x1B);
//		heartRate[6] = PAH8001_readRegister(0x1C);

		PAH8001_writeRegister(0x7F,0x00); //bank0
		
		//bit7 is Touch Flag (bit7=1 is meant Touch, and bit7=0 is meant No Touch)
//		heartRate[7]=(PAH8001_readRegister(0x59)&0x80); //Check Touch Flag
		return 1;
	}
}

