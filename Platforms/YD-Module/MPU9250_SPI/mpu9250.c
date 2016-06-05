#include "mpu9250.h"
#include "spi.h"

S_INT16_XYZ MPU9250_ACC_LAST;
S_INT16_XYZ MPU9250_GYRO_LAST;
S_INT16_XYZ MPU9250_MAG_LAST;
int16_t			MPU9250_TEMP_LAST;

//MPU9250��ʼ��
u8 MPU9250_Init(void)
{
	MPU9250_IOAndSPI_Init();//MPU9250 IO�ں�SPI��ʼ��
	if(MPU9250_Read_Reg(WHO_AM_I)==0x71)
	{
		MPU9250_Write_Reg(USER_CTRL,0X10); //ʹ��MPU9250SPI
		MPU9250_Write_Reg(PWR_MGMT_1,0X80);  //��Դ����,��λMPU9250
		MPU9250_Write_Reg(SMPLRT_DIV,0x07);//������1000/(1+7)=125HZ
		MPU9250_Write_Reg(CONFIG,0X06);				//��ͨ�˲��� 0x06 5hz
		MPU9250_Write_Reg(GYRO_CONFIG,0X18);  //�����ǲ�����Χ 0X18 ����2000��
		MPU9250_Write_Reg(ACCEL_CONFIG,0x18); //���ٶȼƲ�����Χ 0X18 ����16g
		return 0;
	}
	return 1;
}

//MPU9250��ʼ��
void MPU9250_IOAndSPI_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOG, ENABLE);	 //ʹ��PB,D,G�˿�ʱ��
    	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				 //PB12���� ��ֹW25X�ĸ���
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);	//��ʼ��ָ��IO
 	GPIO_SetBits(GPIOB,GPIO_Pin_12);//����				
 	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3; //PD2���� ��ֹSD���ĸ��� PD3ΪMPU9250Ƭѡ�ź�
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_Init(GPIOD, &GPIO_InitStructure);	//��ʼ��ָ��IO
 	GPIO_SetBits(GPIOD,GPIO_Pin_2|GPIO_Pin_3);//����
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;	//PG7���� 	 ��ֹNRF24L01 ��ֹNRF24L01�ĸ���
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 	GPIO_Init(GPIOG, &GPIO_InitStructure);//��ʼ��ָ��IO
	GPIO_SetBits(GPIOG,GPIO_Pin_7);//����
	
	SPI2_Init();   //��ʼ��SPI2
	
	SPI_Cmd(SPI2,DISABLE);  //SPI2��ʹ��
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//SPI����
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		//ʱ�����յ�
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	//���ݲ����ڵ�1��ʱ����
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS�ź����������
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;		//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ16
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRCֵ����Ķ���ʽ
	SPI_Init(SPI2, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
	
	SPI_Cmd(SPI2,ENABLE);//ʹ��SPI2
	MPU9250_CS=1;  //MPU9250Ƭѡȡ��
}


//SPIд�Ĵ���
//reg:ָ���ļĴ�����ַ
//value:д���ֵ
u8 MPU9250_Write_Reg(u8 reg,u8 value)
{
	u8 status;
	MPU9250_CS=0;  //ʹ��SPI����
	status=SPI2_ReadWriteByte(reg); //����д����+�Ĵ�����
	SPI2_ReadWriteByte(value);//д��Ĵ���ֵ
	MPU9250_CS=1;  //��ֹMPU925  50
	return(status);//����״ֵ̬
}

//SPI��ȡ�Ĵ���
//reg:ָ���ļĴ�����ַ
u8 MPU9250_Read_Reg(u8 reg)
{
	u8 reg_val;
	MPU9250_CS=0;  //ʹ��SPI����
	SPI2_ReadWriteByte(reg|0x80); //���Ͷ�����+�Ĵ�����
	reg_val=SPI2_ReadWriteByte(0xff);//��ȡ�Ĵ���ֵ
	MPU9250_CS=1;  //��ֹMPU9250
	return(reg_val);
}

//��ȡMPU9250����
void MPU9250_ReadValue(void)
{
	u8 MPU9250_buf[14],i;
	MPU9250_CS=0;  //ʹ��SPI����
	SPI2_ReadWriteByte(ACCEL_XOUT_H|0x80); //���Ͷ�����+�Ĵ�����
	for(i=0;i<14;i++)//һ����ȡ14�ֽڵ�����
	{
		MPU9250_buf[i]=SPI2_ReadWriteByte(0xff); //ѭ����ȡ
		
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
	
	MPU9250_CS=1;  //��ֹMPU9250
}















	
	