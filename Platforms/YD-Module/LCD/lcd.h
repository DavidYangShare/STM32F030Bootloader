/*****************************************************************

File			:	YD-Module\LCD\lcd.h
Fuction		:	Power management chip
Author		:	@hiyangdong
Version		:	V1.0
Time			:	07 Dec. 2015

*****************************************************************/

#ifndef _LCD_H_
#define _LCD_H_		
 
#include "stdlib.h"
#include "stm32l1xx_hal.h"
#include "spi.h"

typedef struct  
{										    
	uint16_t width;				//LCD width
	uint16_t height;			//LCD height
	uint16_t id;					//LCD ID
	uint8_t  dir;					//direction
	uint16_t	 wramcmd;		//start write RAM
	uint16_t  setxcmd;		//set x command
	uint16_t  setycmd;		//set x command 
}_lcd_dev; 	

extern _lcd_dev lcddev;	//important parameter
#define USE_HORIZONTAL  	0	//0-vertical mode;1-horizontal mode

//define the SIZE of LCD
#if USE_HORIZONTAL==1	//using horizontal
#define LCD_W 128
#define LCD_H 128
#else
#define LCD_W 128
#define LCD_H 128
#endif

extern uint16_t  POINT_COLOR;
extern uint16_t  BACK_COLOR;

#define hspix hspi2

#define LCD_LED        	GPIO_PIN_8  //PA8 connect to TFT -LED with PWM
#define LCD_CTRL   	  	GPIOB				//port of LCD
#define LCD_RS         	GPIO_PIN_12	//PB10 connect to TFT --RS
#define LCD_CS        	GPIO_PIN_14 //PB11 connect to TFT --CS							    
//set pins
#define	LCD_CS_SET  	HAL_GPIO_WritePin(LCD_CTRL,LCD_CS,GPIO_PIN_SET);    
#define	LCD_RS_SET  	HAL_GPIO_WritePin(LCD_CTRL,LCD_RS,GPIO_PIN_SET);
#define	LCD_LED_SET  	HAL_GPIO_WritePin(GPIOA,LCD_LED,GPIO_PIN_SET);
//#define	LCD_RST_SET  	HAL_GPIO_WritePin(LCD_CTRL,LCD_RST,GPIO_PIN_SET);

//clear pins
#define	LCD_CS_CLR  	HAL_GPIO_WritePin(LCD_CTRL,LCD_CS,GPIO_PIN_RESET);    
#define	LCD_RS_CLR  	HAL_GPIO_WritePin(LCD_CTRL,LCD_RS,GPIO_PIN_RESET);
#define	LCD_LED_CLR  	HAL_GPIO_WritePin(GPIOA,LCD_LED,GPIO_PIN_RESET);
//#define	LCD_RST_CLR  	HAL_GPIO_WritePin(LCD_CTRL,LCD_RST,GPIO_PIN_RESET);


//pen color
#define WHITE       0xFFFF
#define BLACK      	0x0000	  
#define BLUE       	0x001F  
#define BRED        0XF81F
#define GRED 			 	0XFFE0
#define GBLUE			 	0X07FF
#define RED         0xF800
#define MAGENTA     0xF81F
#define GREEN       0x07E0
#define CYAN        0x7FFF
#define YELLOW      0xFFE0
#define BROWN 			0XBC40 //brown
#define BRRED 			0XFC07 //brown red
#define GRAY  			0X8430 //gray

//GUI color of panel
#define DARKBLUE      	 0X01CF	//dark blue
#define LIGHTBLUE      	 0X7D7C	//light blue  
#define GRAYBLUE       	 0X5458 //gray blue

 
#define LIGHTGREEN     	0X841F //light gren
#define LIGHTGRAY     	0XEF5B //light gray(PANNEL)
#define LGRAY 			 		0XC618 //gray(PANNEL),window backgroung

#define LGRAYBLUE      	0XA651 //light gray blue(middle layer)
#define LBBLUE          0X2B12 //light brown vlue(clauses background)
	    															  
extern uint16_t BACK_COLOR, POINT_COLOR ;  

void LCD_Init(void);
void LCD_DisplayOn(void);
void LCD_DisplayOff(void);
void LCD_Clear(uint16_t Color);	 
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos);
void LCD_DrawPoint(uint16_t x,uint16_t y);			//draw point 
uint16_t  LCD_ReadPoint(uint16_t x,uint16_t y); //rean poing
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);		   
void LCD_SetWindows(uint16_t xStar, uint16_t yStar,uint16_t xEnd,uint16_t yEnd);
void LCD_DrawPoint_16Bit(uint16_t color);
uint16_t LCD_RD_DATA(void);										//read LCD data								    
void LCD_WriteReg(uint8_t LCD_Reg, uint8_t LCD_RegValue);
void LCD_WR_DATA(uint8_t data);
void LCD_WR_DATA_16Bit(uint16_t data);
uint16_t LCD_ReadReg(uint8_t LCD_Reg);
void LCD_WriteRAM_Prepare(void);
void LCD_WriteRAM(uint16_t RGB_Code);
uint16_t LCD_ReadRAM(void);		   
uint16_t LCD_BGR2RGB(uint16_t c);
void LCD_SetParam(void);
					  		 
#endif  

