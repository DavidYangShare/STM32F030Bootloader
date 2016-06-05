#include "mpu9250.h"
#include "spi.h"

S_INT16_XYZ MPU9250_ACC_LAST;
S_INT16_XYZ MPU9250_GYRO_LAST;
S_INT16_XYZ MPU9250_MAG_LAST;
int16_t			MPU9250_TEMP_LAST;

//MPU9250初始化
u8 MPU9250_Init(void)
{
	MPU9250_IOAndSPI_Init();//MPU9250 IO口和SPI初始化
	if(MPU9250_Read_Reg(WHO_AM_I)==0x71)
	{
		MPU9250_Write_Reg(USER_CTRL,0X10); //使能MPU9250SPI
		MPU9250_Write_Reg(PWR_MGMT_1,0X80);  //电源管理,复位MPU9250
		MPU9250_Write_Reg(SMPLRT_DIV,0x07);//采样率1000/(1+7)=125HZ
		MPU9250_Write_Reg(CONFIG,0X06);				//低通滤波器 0x06 5hz
		MPU9250_Write_Reg(GYRO_CONFIG,0X18);  //陀螺仪测量范围 0X18 正负2000度
		MPU9250_Write_Reg(ACCEL_CONFIG,0x18); //加速度计测量范围 0X18 正负16g
		return 0;
	}
	return 1;
}

//MPU9250初始化
void MPU9250_IOAndSPI_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOG, ENABLE);	 //使能PB,D,G端口时钟
    	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				 //PB12上拉 防止W25X的干扰
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);	//初始化指定IO
 	GPIO_SetBits(GPIOB,GPIO_Pin_12);//上拉				
 	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3; //PD2上拉 禁止SD卡的干扰 PD3为MPU9250片选信号
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_Init(GPIOD, &GPIO_InitStructure);	//初始化指定IO
 	GPIO_SetBits(GPIOD,GPIO_Pin_2|GPIO_Pin_3);//上拉
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;	//PG7上拉 	 禁止NRF24L01 防止NRF24L01的干扰
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 	GPIO_Init(GPIOG, &GPIO_InitStructure);//初始化指定IO
	GPIO_SetBits(GPIOG,GPIO_Pin_7);//上拉
	
	SPI2_Init();   //初始化SPI2
	
	SPI_Cmd(SPI2,DISABLE);  //SPI2不使能
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//SPI主机
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		//时钟悬空低
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	//数据捕获于第1个时钟沿
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS信号由软件控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;		//定义波特率预分频的值:波特率预分频值为16
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式
	SPI_Init(SPI2, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
	
	SPI_Cmd(SPI2,ENABLE);//使能SPI2
	MPU9250_CS=1;  //MPU9250片选取消
}


//SPI写寄存器
//reg:指定的寄存器地址
//value:写入的值
u8 MPU9250_Write_Reg(u8 reg,u8 value)
{
	u8 status;
	MPU9250_CS=0;  //使能SPI传输
	status=SPI2_ReadWriteByte(reg); //发送写命令+寄存器号
	SPI2_ReadWriteByte(value);//写入寄存器值
	MPU9250_CS=1;  //禁止MPU925  50
	return(status);//返回状态值
}

//SPI读取寄存器
//reg:指定的寄存器地址
u8 MPU9250_Read_Reg(u8 reg)
{
	u8 reg_val;
	MPU9250_CS=0;  //使能SPI传输
	SPI2_ReadWriteByte(reg|0x80); //发送读命令+寄存器号
	reg_val=SPI2_ReadWriteByte(0xff);//读取寄存器值
	MPU9250_CS=1;  //禁止MPU9250
	return(reg_val);
}

//读取MPU9250数据
void MPU9250_ReadValue(void)
{
	u8 MPU9250_buf[14],i;
	MPU9250_CS=0;  //使能SPI传输
	SPI2_ReadWriteByte(ACCEL_XOUT_H|0x80); //发送读命令+寄存器号
	for(i=0;i<14;i++)//一共读取14字节的数据
	{
		MPU9250_buf[i]=SPI2_ReadWriteByte(0xff); //循环读取
		
	}
	
	/*MPU9250_buf[0]=SPI2_ReadWriteByte(0xff);
	
	SPI2_ReadWriteByte(ACCEL_XOUT_L|0x80); 
	MPU9250_buf[1]=SPI2_ReadWriteByte(0xff);
	MPU9250_ACC_LAST.X = ((int16_t)MPU9250_buf[0]<<8) + MPU9250_buf[1];*/
	
	MPU9250_ACC_LAST.X = ((int16_t)MPU9250_buf[0]<<8) | MPU9250_buf[1];
	MPU9250_ACC_LAST.Y = ((int16_t)MPU9250_buf[2]<<8) | MPU9250_buf[3];
	MPU9250_ACC_LAST.Z = ((int16_t)MPU9250_buf[4]<<8) | MPU9250_buf[5];
	
	MPU9250_TEMP_LAST =  ((int16_t)MPU9250_buf[6]<<8) | MPU9250_buf[7];
	
	MPU9250_GYRO_LAST.X = ((int16_t)MPU9250_buf[8]<<8) | MPU9250_buf[9];
	MPU9250_GYRO_LAST.Y = ((int16_t)MPU9250_buf[10]<<8) | MPU9250_buf[11];
	MPU9250_GYRO_LAST.Z = ((int16_t)MPU9250_buf[12]<<8) | MPU9250_buf[13];
	
	MPU9250_CS=1;  //禁止MPU9250
}















	
	