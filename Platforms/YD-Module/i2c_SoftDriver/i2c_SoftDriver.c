#include "i2c_soft.h"
#include "delay.h"	   //��������ʱ����
#include "usart.h"
#include <math.h>


//SCL -> PC1
//SDA -> PC2
#define SCL_H()	 GPIO_SetBits(GPIOC, GPIO_Pin_1)
#define SCL_L()	 GPIO_ResetBits(GPIOC, GPIO_Pin_1)
#define SDA_H()	 GPIO_SetBits(GPIOC, GPIO_Pin_2)
#define SDA_L()	 GPIO_ResetBits(GPIOC, GPIO_Pin_2)

#define SDA  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_2)


#define BMP180_SlaveAddr 0xee   //BMP180��������ַ
//BMP180У׼ϵ��
int16_t AC1;
int16_t AC2;
int16_t AC3;
uint16_t AC4;
uint16_t AC5;
uint16_t AC6;
int16_t B1;
int16_t B2;
int16_t MB;
int16_t MC;
int16_t MD;

u8 BMP180_ID=0;	         //BMP180��ID
float True_Temp=0;       //ʵ���¶�,��λ:��
float True_Press=0;      //ʵ����ѹ,��λ:Pa
float True_Altitude=0;	 //ʵ�ʸ߶�,��λ:m

/*�ⲿоƬIIC���ų�ʼ��
 *SCL:PC1
 *SDA:PC2
*/
void IIC_PortInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;  //����һ��GPIO_InitTypeDef���͵Ľṹ��

	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_1|GPIO_Pin_2);	  //PC1,PC2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;           //©����©
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_SetBits(GPIOC, GPIO_Pin_1|GPIO_Pin_2);    //����

}

void IIC_Init(void)
{
	SCL_H();  //SCL = 1;
	delay_us(5);
	SDA_H();  //SDA = 1;
	delay_us(5);	
}

void IIC_Start(void)
{
	SDA_H();  //SDA = 1;
	delay_us(5);
	SCL_H();  //SCL = 1;
	delay_us(5);
	SDA_L();  //SDA = 0;
	delay_us(5);	
}

void IIC_Stop(void)
{
	SDA_L();   //SDA = 0;
	delay_us(5);
	SCL_H();   //SCL = 1;
	delay_us(5);
	SDA_H();   //SDA = 1;
	delay_us(5);
}

unsigned char IIC_ReceiveACK(void)
{
	unsigned char ACK;

	SDA_H();     //SDA=1;//Ҫ���͵�ƽ���������ٶ�,����������Ǵ�������,����Ҫ��
	SCL_H();     //SCL=1;
	delay_us(5);

	if (SDA==1)  //SDAΪ��
	{
		ACK	= 1;	
	}
	else ACK = 0;  //SDAΪ��


	SCL_L();    //SCL = 0;//SCLΪ�͵�ƽʱSDA�ϵ����ݲ�����仯,Ϊ������һ���ֽ���׼�� 
	delay_us(5);
	
	return ACK;					
}

void IIC_SendACK(unsigned char ack)
{
	if (ack == 1)SDA_H();
	else if (ack == 0)SDA_L();
	//SDA = ack;
	SCL_H();   //SCL = 1;
	delay_us(5);
	SCL_L();   //SCL = 0;
	delay_us(5);
}

unsigned char IIC_SendByte(unsigned char dat)
{
	unsigned char i;
	unsigned char bResult=1;
	
	SCL_L();     //SCL = 0;//����ʱ����
	delay_us(5);		

	for( i=0;i<8;i++ ) //һ��SCK,��datһλһλ�����͵�SDA��
	{
		if( (dat<<i)&0x80 )SDA_H();   //SDA = 1;//�ȷ���λ
		else SDA_L();  //SDA = 0;
		delay_us(5);

		SCL_H();  //SCL = 1;
		delay_us(5);
		SCL_L();  //SCL = 0;
		delay_us(5);
	}

	bResult=IIC_ReceiveACK(); //������һ���ֽڵ�����,�ȴ�����Ӧ���ź�

	return bResult;	 //����Ӧ���ź�
}

unsigned char IIC_ReadByte(void)
{
	unsigned char dat;
	unsigned char i;
	
	SCL_H();     //SCL = 1;//ʼ��������Ϊ��������׼��
	delay_us(5);

	for( i=0;i<8;i++ )
	{
		dat <<= 1;
		dat = dat | (SDA);
		delay_us(5);
		
		SCL_L();   //SCL = 0;
		delay_us(5);	
		SCL_H();   //SCL = 1;
		delay_us(5);	
	}
	return dat;
}

/*��BMP180�ж�1���ֽڵ�����*/
u8 BMP180_ReadOneByte(u8 ReadAddr)
{
	u8 temp = 0;
	u8 IIC_ComFlag = 1;	  //IICͨ�ű�־,Ϊ0��־����,1��ʾͨ�Ŵ���
	
	IIC_Start();     //IIC start
	IIC_ComFlag = IIC_SendByte(BMP180_SlaveAddr);	//slave address+W:0
	//printf("IIC_ComFlag=%u \r\n",IIC_ComFlag);
	if (IIC_ComFlag == 0)                           //����ֵΪ0��ʾͨ������,���Լ���ͨ�š�����ͨ����
	{
		IIC_SendByte(ReadAddr);	                     //�����������
		IIC_Start();
		IIC_SendByte(BMP180_SlaveAddr|0x01);         //slave address+R:1
		temp = IIC_ReadByte();                       //������
		IIC_SendACK(1);					
		IIC_Stop();
	}
	return (temp);		
}

/*��BMP180�ж�2���ֽڵ�����*/
int16_t BMP180_ReadTwoByte(u8 ReadAddr)
{
	u8 IIC_ComFlag = 1;	  //IICͨ�ű�־,Ϊ0��־����,1��ʾͨ�Ŵ���
	u8 MSB,LSB;
	int16_t temp;
	
	IIC_Start();
	IIC_ComFlag = IIC_SendByte(BMP180_SlaveAddr);
	if (IIC_ComFlag == 0)
	{
		IIC_SendByte(ReadAddr);
		IIC_Start();
		IIC_SendByte(BMP180_SlaveAddr|0x01);
		MSB = IIC_ReadByte();	    //�ȶ���λ
		IIC_SendACK(0);		    //ACK
		LSB = IIC_ReadByte();	   //�ٶ���λ
		IIC_SendACK(1);		   //NACK
		IIC_Stop();
	}
	temp = MSB*256+LSB;

	return temp;													
}

/*��BMP180�ļĴ���дһ���ֽڵ�����*/
void Write_OneByteToBMP180(u8 RegAdd, u8 Data)
{
	IIC_Start();                       //IIC start
	IIC_SendByte(BMP180_SlaveAddr);	  //slave address+W:0
	IIC_SendByte(RegAdd);
	IIC_SendByte(Data);
	IIC_Stop();	
}


/*��ȡBMP180��У׼ϵ��*/
void Read_CalibrationData(void)
{
	AC1 = BMP180_ReadTwoByte(0xaa);
	AC2 = BMP180_ReadTwoByte(0xac);
	AC3 = BMP180_ReadTwoByte(0xae);
	AC4 = BMP180_ReadTwoByte(0xb0);
	AC5 = BMP180_ReadTwoByte(0xb2);
	AC6 = BMP180_ReadTwoByte(0xb4);
	B1 = BMP180_ReadTwoByte(0xb6);
	B2 = BMP180_ReadTwoByte(0xb8);
	MB = BMP180_ReadTwoByte(0xba);
	MC = BMP180_ReadTwoByte(0xbc);
	MD = BMP180_ReadTwoByte(0xbe);
	printf("AC1:%d \r\n",AC1);
	printf("AC2:%d \r\n",AC2);
	printf("AC3:%d \r\n",AC3);
	printf("AC4:%d \r\n",AC4);
	printf("AC5:%d \r\n",AC5);
	printf("AC6:%d \r\n",AC6);
	printf("B1:%d \r\n",B1);
	printf("B2:%d \r\n",B2);
	printf("MB:%d \r\n",MB);
	printf("MC:%d \r\n",MC);
	printf("MD:%d \r\n",MD);	
}

/*��BMP180û�о����������¶�ֵ*/
int32_t Get_BMP180UT(void)
{
	int32_t UT;

	Write_OneByteToBMP180(0xf4,0x2e);       //write 0x2E into reg 0xf4
	delay_ms(10);								    //wait 4.5ms
	UT = BMP180_ReadTwoByte(0xf6);          //read reg 0xF6(MSB),0xF7(LSB)
	printf("UT:%ld \r\n",UT);

	return UT;
}

/*��BMP180û�о���������ѹ��ֵ*/
int32_t Get_BMP180UP(void)
{
	int32_t UP=0;

	Write_OneByteToBMP180(0xf4,0x34);       //write 0x34 into reg 0xf4 
	delay_ms(10);									 //wait 4.5ms
	UP = BMP180_ReadTwoByte(0xf6); 
	UP &= 0x0000FFFF;
	printf("UP:%ld \r\n",UP);
	
	return UP;		
}

/*��δ�����������¶Ⱥ�ѹ��ֵת��Ϊʱ����¶Ⱥ�ѹ��ֵ
 *True_Temp:ʵ���¶�ֵ,��λ:��
 *True_Press:ʱ��ѹ��ֵ,��λ:Pa
 *True_Altitude:ʵ�ʺ��θ߶�,��λ:m
*/
void Convert_UncompensatedToTrue(long UT,long UP)
{
	long X1,X2,X3,B3,B5,B6,B7,T,P;
	unsigned long B4;
	
	X1 = ((UT-AC6)*AC5)>>15;		//printf("X1:%ld \r\n",X1);
	X2 = ((long)MC<<11)/(X1+MD);	//printf("X2:%ld \r\n",X2);
	B5 = X1+X2;						   //printf("B5:%ld \r\n",B5);
	T = (B5+8)>>4;				      //printf("T:%ld \r\n",T);
	True_Temp = T/10.0;			   printf("Temperature:%.1f \r\n",True_Temp);

	B6 = B5-4000;						//printf("B6:%ld \r\n",B6);
	X1 = (B2*B6*B6)>>23;				//printf("X1:%ld \r\n",X1);
	X2 = (AC2*B6)>>11;				//printf("X2:%ld \r\n",X2);
	X3 = X1+X2;							//printf("X3:%ld \r\n",X3);
	B3 = (((long)AC1*4+X3)+2)/4;	//printf("B3:%ld \r\n",B3);
	X1 = (AC3*B6)>>13;				//printf("X1:%ld \r\n",X1);
	X2 = (B1*(B6*B6>>12))>>16;		//printf("X2:%ld \r\n",X2);
	X3 = ((X1+X2)+2)>>2;				//printf("X3:%ld \r\n",X3);
	B4 = AC4*(unsigned long)(X3+32768)>>15;   //printf("B4:%lu \r\n",B4);
	B7 = ((unsigned long)UP-B3)*50000;        //printf("B7:%lu \r\n",B7);
	if (B7 < 0x80000000)
	{
		P = (B7*2)/B4;	
	}
	else P=(B7/B4)*2;					//printf("P:%ld \r\n",P);			
	X1 = (P/256.0)*(P/256.0);		//printf("X1:%ld \r\n",X1);
	X1 = (X1*3038)>>16;				//printf("X1:%ld \r\n",X1);
	X2 = (-7357*P)>>16;				//printf("X2:%ld \r\n",X2);
	P = P+((X1+X2+3791)>>4);		//printf("P:%ld \r\n",P);
	True_Press = P;					printf("Press:%.1fPa \r\n",True_Press);

	True_Altitude = 44330*(1-pow((P/101325.0),(1.0/5.255)));		    
	printf("Altitude:%.3fm \r\n",True_Altitude);	
}