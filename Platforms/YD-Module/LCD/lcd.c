/*****************************************************************

File			:	YD-Module\LCD\lcd.c
Fuction		:	Power management chip
Author		:	@hiyangdong
Version		:	V1.0
Time			:	07 Dec. 2015

*****************************************************************/

#include "lcd.h"

_lcd_dev lcddev;

uint16_t POINT_COLOR = 0x0000; //pen color,default red
uint16_t BACK_COLOR  = 0xFFFF; //default white

/*****************************************************************
Name			:	void LCD_RESET()
Fuction		:	reset the LCD
Input			:	void
Output		:	void
Editor		:	@hiyangdong
Copyright :	xiao冯@全动电子
Version		:	V0.1
Time			:	07 Dec. 2015
*****************************************************************/
void LCD_RESET(void)
{

}

/*****************************************************************
Name			:	void LCD_WR_REG(uint8_t data)
Fuction		:	write the 8bit reg data
Input			:	data: 8 bit data
Output		:	void
Editor		:	@hiyangdong
Copyright :	xiao冯@全动电子
Version		:	V0.1
Time			:	07 Dec. 2015
*****************************************************************/
void LCD_WR_REG(uint8_t data)
{
	
	LCD_CS_CLR;
	LCD_RS_CLR;		//reg transfer

	HAL_SPI_Transmit(&hspix,&data,1,10);

	LCD_CS_SET;
}

/*****************************************************************
Name			:	void LCD_WR_DATA(uint8_t data)
Fuction		:	write the 8bit data
Input			:	data: 8 bit data
Output		:	void
Editor		:	@hiyangdong
Copyright :	xiao冯@全动电子
Version		:	V0.1
Time			:	07 Dec. 2015
*****************************************************************/
void LCD_WR_DATA(uint8_t data)
{
	
	LCD_CS_CLR;
	LCD_RS_SET;		//data transfer

	HAL_SPI_Transmit(&hspix,&data,1,100);
	
	LCD_CS_SET;

} 

/*****************************************************************
Name			:	void LCD_WR_DATA(uint8_t data)
Fuction		:	write the 8bit data
Input			:	LCD_Reg:write register address
						LCD_RegValue: write register data
Output		:	void
Editor		:	@hiyangdong
Copyright :	xiao冯@全动电子
Version		:	V0.1
Time			:	07 Dec. 2015
*****************************************************************/
void LCD_WriteReg(uint8_t LCD_Reg, uint8_t LCD_RegValue)
{	
	LCD_WR_REG(LCD_Reg);  
	LCD_WR_DATA(LCD_RegValue);	    		 
}	

/*****************************************************************
Name			:	void LCD_SetParam(void)
Fuction		:	set the parameter of the LCD
Input			:	void
Output		:	void
Editor		:	@hiyangdong
Copyright :	xiao冯@全动电子
Version		:	V0.1
Time			:	07 Dec. 2015
*****************************************************************/
void LCD_SetParam(void)
{ 	
	lcddev.wramcmd=0x2C;
#if USE_HORIZONTAL==1	//using horizontal mode
	lcddev.dir=1;				//horizontal mode
	lcddev.width=128;
	lcddev.height=128;
	lcddev.setxcmd=0x2A;
	lcddev.setycmd=0x2B;			
	LCD_WriteReg(0x36,0xA8);

#else									//using vertical mode
	lcddev.dir=0;				//vertical mode		 	 		
	lcddev.width=128;
	lcddev.height=128;
	lcddev.setxcmd=0x2A;
	lcddev.setycmd=0x2B;	
	LCD_WriteReg(0x36,0xC8);
#endif
}	

/*****************************************************************
Name			:	void LCD_DrawPoint(uint16_t x,uint16_t y)
Fuction		:	draw one point on the LCD
Input			:	x:cursor x positoin
						y:cursor y positoin
Output		:	void
Editor		:	@hiyangdong
Copyright :	xiao冯@全动电子
Version		:	V0.1
Time			:	07 Dec. 2015
*****************************************************************/
void LCD_DrawPoint(uint16_t x,uint16_t y)
{
	LCD_SetCursor(x,y);
	LCD_WR_DATA_16Bit(POINT_COLOR);
}

/*****************************************************************
Name			:	void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
Fuction		:	set the cursor position
Input			:	x:cursor x positoin
						y:cursor y positoin
Output		:	void
Editor		:	@hiyangdong
Copyright :	xiao冯@全动电子
Version		:	V0.1
Time			:	07 Dec. 2015
*****************************************************************/
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{	  	    			
	LCD_SetWindows(Xpos,Ypos,Xpos,Ypos);
} 

/*****************************************************************
Name			:	void LCD_Init(void)
Fuction		:	initialize the LDC screen
Input			:	void
Output		:	void
Editor		:	@hiyangdong
Copyright :	xiao冯@全动电子
Version		:	V0.1
Time			:	07 Dec. 2015
*****************************************************************/
void LCD_Init(void)
{  

 	LCD_RESET(); 	//reset the LCD

	//************* Start Initial Sequence **********//		
	//start initialize the LCD
	LCD_WR_REG(0x11);	//Sleep exit 
	HAL_Delay(120);		
	
	//ST7735R Frame Rate
	LCD_WR_REG(0xB1); 
	LCD_WR_DATA(0x01); 
	LCD_WR_DATA(0x2C); 
	LCD_WR_DATA(0x2D); 

	LCD_WR_REG(0xB2); 
	LCD_WR_DATA(0x01); 
	LCD_WR_DATA(0x2C); 
	LCD_WR_DATA(0x2D); 

	LCD_WR_REG(0xB3); 
	LCD_WR_DATA(0x01); 
	LCD_WR_DATA(0x2C); 
	LCD_WR_DATA(0x2D); 
	LCD_WR_DATA(0x01); 
	LCD_WR_DATA(0x2C); 
	LCD_WR_DATA(0x2D); 
	
	LCD_WR_REG(0xB4); //Column inversion 
	LCD_WR_DATA(0x07); 
	
	//ST7735R Power Sequence
	LCD_WR_REG(0xC0); 
	LCD_WR_DATA(0xA2); 
	LCD_WR_DATA(0x02); 
	LCD_WR_DATA(0x84); 
	LCD_WR_REG(0xC1); 
	LCD_WR_DATA(0xC5); 

	LCD_WR_REG(0xC2); 
	LCD_WR_DATA(0x0A); 
	LCD_WR_DATA(0x00); 

	LCD_WR_REG(0xC3); 
	LCD_WR_DATA(0x8A); 
	LCD_WR_DATA(0x2A); 
	LCD_WR_REG(0xC4); 
	LCD_WR_DATA(0x8A); 
	LCD_WR_DATA(0xEE); 
	
	LCD_WR_REG(0xC5); //VCOM 
	LCD_WR_DATA(0x0E); 
	
	LCD_WR_REG(0x36); //MX, MY, RGB mode 				 
	LCD_WR_DATA(0xC8); 
	
	//ST7735R Gamma Sequence
	LCD_WR_REG(0xe0); 
	LCD_WR_DATA(0x0f); 
	LCD_WR_DATA(0x1a); 
	LCD_WR_DATA(0x0f); 
	LCD_WR_DATA(0x18); 
	LCD_WR_DATA(0x2f); 
	LCD_WR_DATA(0x28); 
	LCD_WR_DATA(0x20); 
	LCD_WR_DATA(0x22); 
	LCD_WR_DATA(0x1f); 
	LCD_WR_DATA(0x1b); 
	LCD_WR_DATA(0x23); 
	LCD_WR_DATA(0x37); 
	LCD_WR_DATA(0x00); 	
	LCD_WR_DATA(0x07); 
	LCD_WR_DATA(0x02); 
	LCD_WR_DATA(0x10); 

	LCD_WR_REG(0xe1); 
	LCD_WR_DATA(0x0f); 
	LCD_WR_DATA(0x1b); 
	LCD_WR_DATA(0x0f); 
	LCD_WR_DATA(0x17); 
	LCD_WR_DATA(0x33); 
	LCD_WR_DATA(0x2c); 
	LCD_WR_DATA(0x29); 
	LCD_WR_DATA(0x2e); 
	LCD_WR_DATA(0x30); 
	LCD_WR_DATA(0x30); 
	LCD_WR_DATA(0x39); 
	LCD_WR_DATA(0x3f); 
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x07); 
	LCD_WR_DATA(0x03); 
	LCD_WR_DATA(0x10);  
	
	LCD_WR_REG(0x2a);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x7f);

	LCD_WR_REG(0x2b);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x9f);
	
	LCD_WR_REG(0xF0); //Enable test command  
	LCD_WR_DATA(0x01); 
	LCD_WR_REG(0xF6); //Disable ram power save mode 
	LCD_WR_DATA(0x00); 
	
	LCD_WR_REG(0x3A); //65k mode 
	LCD_WR_DATA(0x05); 	
	LCD_WR_REG(0x29);	//Display on	

	LCD_SetParam();		//set parameter of LCD	 
	LCD_LED_SET;			//light the back LED 
	LCD_Clear(WHITE);
	
}

/*****************************************************************
Name			:	void LCD_WriteRAM_Prepare(void)
Fuction		:	write RAM prepared
Input			:	void
Output		:	void
Editor		:	@hiyangdong
Copyright :	xiao冯@全动电子
Version		:	V0.1
Time			:	07 Dec. 2015
*****************************************************************/
void LCD_WriteRAM_Prepare(void)
{
	LCD_WR_REG(lcddev.wramcmd);
}	 

/*****************************************************************
Name			:	void LCD_WR_DATA_16Bit(uint16_t data)
Fuction		:	write a 16bit data not reg
Input			:	data:16bit data
Output		:	void
Editor		:	@hiyangdong
Copyright :	xiao冯@全动电子
Version		:	V0.1
Time			:	07 Dec. 2015
*****************************************************************/
void LCD_WR_DATA_16Bit(uint16_t data)
{	
	uint8_t dat[2]={(uint8_t)data>>8,(uint8_t)data};
	LCD_CS_CLR;
	LCD_RS_SET;
	HAL_SPI_Transmit(&hspix,dat,2,10);
	LCD_CS_SET;
}

/*****************************************************************
Name			:	void LCD_SetWindows(uint16_t xStar, uint16_t yStar,uint16_t xEnd,uint16_t yEnd)
Fuction		:	set display window with auto insert a new line
Input			:	xStar:set the start x coordinate
						yStar:set the start y coordinate
						xEnd:set the end x coordinate
						yEnd:set the end y coordinate
Output		:	void
Editor		:	@hiyangdong
Copyright :	xiao冯@全动电子
Version		:	V0.1
Time			:	07 Dec. 2015
*****************************************************************/
void LCD_SetWindows(uint16_t xStar, uint16_t yStar,uint16_t xEnd,uint16_t yEnd)
{
#if USE_HORIZONTAL==1	//使用横屏
	LCD_WR_REG(lcddev.setxcmd);	
	LCD_WR_DATA(xStar>>8);
	LCD_WR_DATA(0x00FF&xStar+3);		
	LCD_WR_DATA(xEnd>>8);
	LCD_WR_DATA(0x00FF&xEnd+3);

	LCD_WR_REG(lcddev.setycmd);	
	LCD_WR_DATA(yStar>>8);
	LCD_WR_DATA(0x00FF&yStar+2);		
	LCD_WR_DATA(yEnd>>8);
	LCD_WR_DATA(0x00FF&yEnd+2);	

#else
	
	LCD_WR_REG(lcddev.setxcmd);	
	LCD_WR_DATA(xStar>>8);
	LCD_WR_DATA(0x00FF&xStar+2);		
	LCD_WR_DATA(xEnd>>8);
	LCD_WR_DATA(0x00FF&xEnd+2);

	LCD_WR_REG(lcddev.setycmd);	
	LCD_WR_DATA(yStar>>8);
	LCD_WR_DATA(0x00FF&yStar+3);		
	LCD_WR_DATA(yEnd>>8);
	LCD_WR_DATA(0x00FF&yEnd+3);	
#endif

	LCD_WriteRAM_Prepare();	//start write into RAM		
}   

/*****************************************************************
Name			:	void LCD_SetWindows(uint16_t xStar, uint16_t yStar,uint16_t xEnd,uint16_t yEnd)
Fuction		:	set display window with new color
Input			:	color: clear to the same color
Output		:	void
Editor		:	@hiyangdong
Copyright :	xiao冯@全动电子
Version		:	V0.1
Time			:	07 Dec. 2015
*****************************************************************/
void LCD_Clear(uint16_t Color)
{
	uint16_t i,j;      
	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);	  
	for(i=0;i<lcddev.width;i++)
	{
		for(j=0;j<lcddev.height;j++)
		LCD_WR_DATA_16Bit(Color);	
	}
}   
