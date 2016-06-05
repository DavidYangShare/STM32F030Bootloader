/*********************************************************************************************************
*
* File                : 24C02.c
* Hardware Environment: 
* Build Environment   : RealView MDK-ARM  Version: 4.20
* Version             : V1.0
* By                  : 
*
*                                  (c) Copyright 2005-2011, WaveShare
*                                       http://www.waveshare.net
*                                          All Rights Reserved
*
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "i2c_HALDriver.h"

#define	MPU6050Address 0xD0
//#define	BMP180Address  0xee
#define	BPAHAddress    0x66

#define	SlaveAddress	BPAHAddress	//IIC写入时的地址字节数据，+1为读取

#define hi2cx hi2c1
extern I2C_HandleTypeDef hi2cx;


#define hi2cbmp hi2c2
extern I2C_HandleTypeDef hi2cbmp;
/**************************************
向I2C设备写入一个字节数据
**************************************/
HAL_StatusTypeDef I2C_WriteByte(I2C_HandleTypeDef *hi2c,uint8_t addr, uint8_t reg,uint8_t data)
{
    uint8_t Data[2] = {reg,data};
		HAL_StatusTypeDef status;
		status = HAL_I2C_Master_Transmit(hi2c,addr,Data,2,20);
//		printf("%d\t",status);
    while(status != HAL_OK)
    {
				status = HAL_I2C_Master_Transmit(hi2c,addr,Data,2,20);
				printf("%d\t",status);
        if(HAL_I2C_GetError(hi2c) != HAL_I2C_ERROR_AF)
        {}
					HAL_Delay(500);
    }
		return status;
}
/**************************************
从I2C设备读取一个字节数据
**************************************/

uint8_t I2C_ReadByte(I2C_HandleTypeDef *hi2c,uint8_t addr, uint8_t reg)
{
    uint8_t REG_data;
    while(HAL_I2C_Master_Transmit(hi2c,addr,&reg,1,2) != HAL_OK)
    {
        if(HAL_I2C_GetError(hi2c) != HAL_I2C_ERROR_AF)
        {}
    }
   
    if(HAL_I2C_Master_Receive(hi2c,addr+1,&REG_data,1,2) != HAL_OK)
    {
        if(HAL_I2C_GetError(hi2c) != HAL_I2C_ERROR_AF)
        {}
    }
    return REG_data;
		
}

/**************************************
从I2C设备读取两个字节数据
**************************************/
HAL_StatusTypeDef I2C_ReadBytes(I2C_HandleTypeDef *hi2c,uint8_t addr, uint8_t reg, uint8_t *pData, uint16_t size)
{
		HAL_StatusTypeDef status;
		status = HAL_I2C_Master_Transmit(hi2c,addr,&reg,1,2);
    while(status != HAL_OK)
    {
        if(HAL_I2C_GetError(hi2c) != HAL_I2C_ERROR_AF)
        {}
    }
		
    if(HAL_I2C_Master_Receive(hi2c,addr+1,pData,size,20) != HAL_OK)
    {
        if(HAL_I2C_GetError(hi2c) != HAL_I2C_ERROR_AF)
        {}
    }
    return status;
		
}	

//void Single_WriteBMPI2C(uint8_t REG_Address,uint8_t REG_data)
//{
//    uint8_t rxData[2] = {REG_Address,REG_data};
//		uint8_t i=0;
//		HAL_StatusTypeDef temp;
//		temp = HAL_I2C_Master_Transmit(&hi2cbmp,BMP180Address,rxData,2,100);
//    while(temp != HAL_OK)
//    {
//			i++;
//			temp = HAL_I2C_Master_Transmit(&hi2cbmp,BMP180Address,rxData,2,100);
//			printf("%d\t",temp);
//			printf("%d\t",i);
//			printf("Error\r\n");
//			HAL_Delay(100);
//        if(HAL_I2C_GetError(&hi2cbmp) != HAL_I2C_ERROR_AF)
//        {}
//    }
//}
//**************************************
//从I2C设备读取一个字节数据
//**************************************
uint8_t Single_ReadI2C(uint8_t REG_Address)
{
    uint8_t REG_data;
    while(HAL_I2C_Master_Transmit(&hi2cx,SlaveAddress,&REG_Address,1,100) != HAL_OK)
    {
        if(HAL_I2C_GetError(&hi2cx) != HAL_I2C_ERROR_AF)
        {}
    }
   
    if(HAL_I2C_Master_Receive(&hi2cx,SlaveAddress+1,&REG_data,1,100) != HAL_OK)
    {
        if(HAL_I2C_GetError(&hi2cx) != HAL_I2C_ERROR_AF)
        {}
    }
    return REG_data;
		
}


//uint8_t Single_ReadBMPI2C(uint8_t REG_Address)
//{
//    uint8_t REG_data;
//    while(HAL_I2C_Master_Transmit(&hi2cbmp,BMP180Address,&REG_Address,1,100) != HAL_OK)
//    {
//        if(HAL_I2C_GetError(&hi2cbmp) != HAL_I2C_ERROR_AF)
//        {}
//    }
//   
//    if(HAL_I2C_Master_Receive(&hi2cbmp,BMP180Address+1,&REG_data,1,100) != HAL_OK)
//    {
//        if(HAL_I2C_GetError(&hi2cbmp) != HAL_I2C_ERROR_AF)
//        {}
//    }
//    return REG_data;
//		
//}
//**************************************
//从I2C设备读取两个字节数据
//**************************************
//int16_t Double_ReadI2C(uint8_t REG_Address)
//{
//    uint8_t REG_data[2];
//	  int16_t temp = 0;
//    while(HAL_I2C_Master_Transmit(&hi2cbmp,BMP180Address,&REG_Address,1,100) != HAL_OK)
//    {
//        if(HAL_I2C_GetError(&hi2cbmp) != HAL_I2C_ERROR_AF)
//        {}
//    }
//   
//    if(HAL_I2C_Master_Receive(&hi2cbmp,BMP180Address+1,REG_data,2,100) != HAL_OK)
//    {
//        if(HAL_I2C_GetError(&hi2cbmp) != HAL_I2C_ERROR_AF)
//        {}
//    }
//		temp = (REG_data[0]<<8)+REG_data[1];
//		
//    return temp;
//}		
		
void Single_Write(uint8_t DEV_Address,uint8_t REG_Address,uint8_t REG_data)
{
    uint8_t rxData[2] = {REG_Address,REG_data};
		HAL_StatusTypeDef temp;
//		printf("OK\t");
		temp = HAL_I2C_Master_Transmit(&hi2cbmp,DEV_Address,rxData,2,100);
    while(temp != HAL_OK)
    {
			temp = HAL_I2C_Master_Transmit(&hi2cbmp,DEV_Address,rxData,2,100);
//			printf("%d",temp);
			printf("ERROR\t");
        if(HAL_I2C_GetError(&hi2cbmp) != HAL_I2C_ERROR_AF)
        {}
    }
}

//**************************************
//从I2C设备读取一个字节数据
//**************************************
uint8_t Single_Read(uint8_t DEV_Address,uint8_t REG_Address)
{
    uint8_t REG_data;
    while(HAL_I2C_Master_Transmit(&hi2cbmp,DEV_Address,&REG_Address,1,100) != HAL_OK)
    {
        if(HAL_I2C_GetError(&hi2cbmp) != HAL_I2C_ERROR_AF)
        {}
    }
   
    if(HAL_I2C_Master_Receive(&hi2cbmp,DEV_Address+1,&REG_data,1,100) != HAL_OK)
    {
        if(HAL_I2C_GetError(&hi2cbmp) != HAL_I2C_ERROR_AF)
        {}
    }
    return REG_data;
		
}

//**************************************
//向I2C设备写入一个字节数据
//**************************************
//void Single_WriteI2C(uint8_t REG_Address,uint8_t REG_data)
//{
//    uint8_t rxData[2] = {REG_Address,REG_data};
//		uint8_t i=0;
//		HAL_StatusTypeDef temp;
//		temp = HAL_I2C_Master_Transmit(&hi2cx,SlaveAddress,rxData,2,100);
//    while(temp != HAL_OK)
//    {
//			i++;
//			temp = HAL_I2C_Master_Transmit(&hi2cx,SlaveAddress,rxData,2,100);
//			printf("%d\t",temp);
//			printf("%d\t",i);
//			printf("Error\r\n");
//			HAL_Delay(100);
//        if(HAL_I2C_GetError(&hi2cx) != HAL_I2C_ERROR_AF)
//        {}
//    }
//}
