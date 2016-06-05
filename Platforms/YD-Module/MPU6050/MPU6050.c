#include "stm32l1xx_hal.h"
#include "MPU6050.h"
#include "i2c_HALDriver.h"
//各坐标轴上静止偏差（重力，角速度）
int16_t offsetAccelX = -195;
int16_t offsetAccelY = 560;
int16_t offsetAccelZ = -169;

int16_t offsetGyroX = 12;
int16_t offsetGyroY = 33;
int16_t offsetGyroZ = 4;

#define hi2cx hi2c1
extern I2C_HandleTypeDef hi2cx;
#define	SlaveAddress	0xD0	//IIC写入时的地址字节数据，+1为读取

//**************************************
//向I2C设备写入一个字节数据
//**************************************
//void Single_WriteI2C(uint8_t REG_Address,uint8_t REG_data)
//{
//    uint8_t rxData[2] = {REG_Address,REG_data};
//    while(HAL_I2C_Master_Transmit(&hi2cx,SlaveAddress,rxData,2,5000) != HAL_OK)
//    {
//        if(HAL_I2C_GetError(&hi2cx) != HAL_I2C_ERROR_AF)
//        {}
//    }
//}
////**************************************
////从I2C设备读取一个字节数据
////**************************************
//uint8_t Single_ReadI2C(uint8_t REG_Address)
//{
//    uint8_t REG_data;
//    while(HAL_I2C_Master_Transmit(&hi2cx,SlaveAddress,&REG_Address,1,5000) != HAL_OK)
//    {
//        if(HAL_I2C_GetError(&hi2cx) != HAL_I2C_ERROR_AF)
//        {}
//    }
//   
//    if(HAL_I2C_Master_Receive(&hi2cx,SlaveAddress+1,&REG_data,1,5000) != HAL_OK)
//    {
//        if(HAL_I2C_GetError(&hi2cx) != HAL_I2C_ERROR_AF)
//        {}
//    }
//    return REG_data;
//}
//**************************************
//初始化MPU6050
//**************************************
void InitMPU6050()
{
    Single_WriteI2C(PWR_MGMT_1, 0x00);    //解除休眠状态

    Single_WriteI2C(SMPLRT_DIV, 0x07);

    Single_WriteI2C(CONFIG, 0x06);

    Single_WriteI2C(GYRO_CONFIG, 0x18);

    Single_WriteI2C(ACCEL_CONFIG, 0x01);
}
//**************************************
//合成数据
//**************************************
int16_t GetMPUOutValue(uint8_t REG_Address)
{
    int16_t result;
    uint8_t H,L;
    H=Single_ReadI2C(REG_Address);
    L=Single_ReadI2C(REG_Address+1);
    result = (H<<8)+L;
    return result;   //合成数据
}

//**************************************
//取某一轴上的加速度数据
//**************************************
int16_t GetAccelValue(char axis)
{
    int16_t result = 0;
    switch(axis)
    {
        case 'x':
        case 'X':
        {
            result = GetMPUOutValue(ACCEL_XOUT_H) - offsetAccelX;
        }
        break;
        case 'y':
        case 'Y':
        {
            result = GetMPUOutValue(ACCEL_YOUT_H) - offsetAccelY;
        }
        break;
        case 'z':
        case 'Z':
        {
            result = GetMPUOutValue(ACCEL_ZOUT_H) - offsetAccelZ;
        }
        break;
    }
    return result;
}

//**************************************
//取某一轴上的角速度数据
//**************************************
int16_t GetGyroValue(char axis)
{
    int16_t result = 0;
    switch(axis)
    {
        case 'x':
        case 'X':
        {
            result = GetMPUOutValue(GYRO_XOUT_H) - offsetGyroX;
        }
        break;
        case 'y':
        case 'Y':
        {
            result = GetMPUOutValue(GYRO_YOUT_H) - offsetGyroY;
        }
        break;
        case 'z':
        case 'Z':
        {
            result = GetMPUOutValue(GYRO_ZOUT_H) - offsetGyroZ;
        }
        break;
    }
    return result;
}

