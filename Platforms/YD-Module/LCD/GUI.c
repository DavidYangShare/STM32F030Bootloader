/*****************************************************************

File			:	YD-Module\LCD\GUI.c
Fuction		:	Power management chip
Author		:	@hiyangdong
Version		:	V1.0
Time			:	07 Dec. 2015

*****************************************************************/

#include "GUI.h"
#include "font.h"

/*****************************************************************
Name			:	void GUI_DrawPoint(uint16_t x,uint16_t y,uint16_t color)
Fuction		:	draw point on the LCD
Input			:	x:cursor x positoin
						y:cursor y positoin
						color:the color of the point
Output		:	void
Editor		:	@hiyangdong
Copyright :	xiao冯@全动电子
Version		:	V0.1
Time			:	07 Dec. 2015
*****************************************************************/
void GUI_DrawPoint(uint16_t x,uint16_t y,uint16_t color)
{
	LCD_SetCursor(x,y);					//set the cursor x y coordinate
	LCD_WR_DATA_16Bit(color);		//set the color 
}


/*****************************************************************
Name			:	void LCD_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color)
Fuction		:	draw point on the LCD
Input			:	sx:set the start x coordinate
						sy:set the start y coordinate
						ex:set the end x coordinate
						ey:set the end y coordinate
						color:set the color 
Output		:	void
Editor		:	@hiyangdong
Copyright :	xiao冯@全动电子
Version		:	V0.1
Time			:	07 Dec. 2015
*****************************************************************/
void LCD_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color)
{  	

	uint16_t i,j;			
	uint16_t width=ex-sx+1; 				//width of the area
	uint16_t height=ey-sy+1;				//height of the area
	LCD_SetWindows(sx,sy,ex-1,ey-1);//set the display windows
	for(i=0;i<height;i++)
	{
		for(j=0;j<width;j++)
		LCD_WR_DATA_16Bit(color);			//set the color 
	}

	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);//recover the windows to full screen
}

/*****************************************************************
Name			:	void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
Fuction		:	draw point on the LCD
Input			:	x1:set the start x coordinate
						y1:set the start y coordinate
						x2:set the end x coordinate
						y2:set the end y coordinate
						color:set the color 
Output		:	void
Editor		:	@hiyangdong
Copyright :	xiao冯@全动电子
Version		:	V0.1
Time			:	07 Dec. 2015
*****************************************************************/
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	uint16_t t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 

	delta_x=x2-x1; 									//calculate increment
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; 						//set the direction 
	else if(delta_x==0)incx=0;			//perpendicular line 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;			//horizontal line
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //select coordinate axis 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )			//draw line
	{  
		LCD_DrawPoint(uRow,uCol);			//draw point
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
} 

/*****************************************************************
Name			:	void LCD_ShowChar
Fuction		:	draw point on the LCD
Input			:	(x,y):start coordinate
						fc:front color
						bc:background color
						num:char number（0-94）
						size:size of char number
						mode:mode-0,fill or erese mode;1,over or addtional mode
Output		:	void
Editor		:	@hiyangdong
Copyright :	xiao冯@全动电子
Version		:	V0.1
Time			:	07 Dec. 2015
*****************************************************************/
void LCD_ShowChar(uint16_t x,uint16_t y,uint16_t fc, uint16_t bc, uint8_t num,uint8_t size,uint8_t mode)
{  
    uint8_t temp;
    uint8_t pos,t;
	uint16_t colortemp=POINT_COLOR;      
		   
	num=num-' ';						//get the value of minus offset
	LCD_SetWindows(x,y,x+size/2-1,y+size-1);	//set single char display window
	if(!mode) 							//fill or erase mode
	{
		
		for(pos=0;pos<size;pos++)
		{
			if(size==12)temp=asc2_1206[num][pos];	//use the 1206 type Font
			else temp=asc2_1608[num][pos];		 		//use the 1608 type Font
			for(t=0;t<size/2;t++)
		    {                 
		        if(temp&0x01)LCD_WR_DATA(fc); 
				else LCD_WR_DATA(bc); 
				temp>>=1; 
				
		    }
			
		}	
	}else	//over or addtional mode
	{
		for(pos=0;pos<size;pos++)
		{
			if(size==12)temp=asc2_1206[num][pos];	//use the 1206 type Font
			else temp=asc2_1608[num][pos];				//use the 1608 type Font
			for(t=0;t<size/2;t++)
		    {   
				POINT_COLOR=fc;              
		        if(temp&0x01)LCD_DrawPoint(x+t,y+pos);//Draw a point    
		        temp>>=1; 
		    }
		}
	}
	POINT_COLOR=colortemp;	
	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);	//recover the windows to full screen	 	  
} 

/*****************************************************************
Name			:	void LCD_ShowNum2412
Fuction		:	draw point on the LCD
Input			:	(x,y):start coordinate
						fc:front color
						bc:background color
						*p:string address
						size:size of char number
						mode:mode-0,fill or erese mode;1,over or addtional mode
Output		:	void
Editor		:	@hiyangdong
Copyright :	xiao冯@全动电子
Version		:	V0.1
Time			:	07 Dec. 2015
*****************************************************************/
void LCD_ShowNum2412(uint16_t x,uint16_t y,uint16_t fc, uint16_t bc,uint8_t *p ,uint8_t size,uint8_t mode)
{  
    uint16_t temp;
    uint8_t pos,t;
	uint16_t colortemp=POINT_COLOR;      
	uint16_t x0=x;
	uint16_t y0=y; 
	uint8_t num=0;
	

    while((*p<='~')&&(*p>=' '))//judge whether or not char
    {   

		if(x>(lcddev.width-1)||y>(lcddev.height-1)) 
		return;   
		num=*p;
		if(':'==num) 			//special char":"
		num=10;	
		else if('.'==num)	//special char"."
		num=11;	
		else  						//just number  	
		num=num-'0';   
		x0=x;
		    
		for(pos=0;pos<48;pos++)
		{
			temp=asc2_2412[num][pos];
			for(t=0;t<8;t++)
		    {   
				POINT_COLOR=fc;              
		        if(temp&0x80)LCD_DrawPoint(x,y);	//Draw a point    
				//else LCD_WR_DATA_16Bit(bc);   
		        temp<<=1; 
				x++;
				if((x-x0)==12)
				{
					x=x0;
					y++;
					break;
				}
		    }
		}
	if(num<10)
	x+=16;			//distance between char control by custom, original value is 12
	else
	x+=8; 			//distance between char control by custom, original value is 12

	y=y0;
    p++;
    }  
	POINT_COLOR=colortemp;	 	 	  
} 

/*****************************************************************
Name			:	void LCD_ShowString
Fuction		:	draw point on the LCD
Input			:	(x,y):start coordinate
						size:size of char number
						*p:string address
						mode:mode-0,fill or erese mode;1,over or addtional mode
Output		:	void
Editor		:	@hiyangdong
Copyright :	xiao冯@全动电子
Version		:	V0.1
Time			:	07 Dec. 2015
*****************************************************************/
void LCD_ShowString(uint16_t x,uint16_t y,uint8_t size,uint8_t *p,uint8_t mode)
{         
    while((*p<='~')&&(*p>=' '))//judge whether or not char
    {   
		if(x>(lcddev.width-1)||y>(lcddev.height-1)) 
		return;     
        LCD_ShowChar(x,y,POINT_COLOR,BACK_COLOR,*p,size,mode);
        x+=size/2;
        p++;
    }  
} 

/*****************************************************************
Name			:	uint32_t mypow(uint8_t m,uint8_t n)
Fuction		:	caculate m to the power n
Input			:	m:multiplier
						n:power
Output		:	void
Editor		:	@hiyangdong
Copyright :	xiao冯@全动电子
Version		:	V0.1
Time			:	07 Dec. 2015
*****************************************************************/
uint32_t mypow(uint8_t m,uint8_t n)
{
	uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}

/*****************************************************************
Name			:	void LCD_ShowNum
Fuction		:	draw point on the LCD
Input			:	(x,y):start coordinate
						num:char number（0~4294967295）
						len:dispaly the length of number
						size:the size of the char(12,16)
Output		:	void
Editor		:	@hiyangdong
Copyright :	xiao冯@全动电子
Version		:	V0.1
Time			:	07 Dec. 2015
*****************************************************************/
void LCD_ShowNum(uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint8_t size)
{         	
	uint8_t t,temp;
	uint8_t enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+(size/2)*t,y,POINT_COLOR,BACK_COLOR,' ',size,1);
				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,POINT_COLOR,BACK_COLOR,temp+'0',size,1); 
	}
} 

/*****************************************************************
Name			:	void LCD_ShowChar
Fuction		:	draw point on the LCD
Input			:	(x,y):start coordinate
						fc:front color
						bc:background color
						str:string of english words
						size:size of char number
						mode:mode-0,fill or erese mode;1,over or addtional mode
Output		:	void
Editor		:	@hiyangdong
Copyright :	xiao冯@全动电子
Version		:	V0.1
Time			:	07 Dec. 2015
*****************************************************************/
void Show_Str(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint8_t *str, uint8_t size, uint8_t mode)
{
	uint16_t x0 = x;
	while (*str != 0)		//judge the string display completed?
	{

		if (x>(lcddev.width - size / 2) || y>(lcddev.height - size))
			return;
		else              		//charset
		{
			if (*str == 0x0D)		//new line code
			{
				y += size;
				x = x0;
				str++;
			}
			else
			{
				if (size == 12 || size == 16)
				{
					LCD_ShowChar(x, y, fc, bc, *str, size, mode);
					x += size / 2; 	//half of full char 
				}
				else							//if there is no 16X32 taht replace by 8X16
				{
					LCD_ShowChar(x, y, fc, bc, *str, 16, mode);
					x += 8; 				//half of full char 
				}
			}
			str++;
		}
	}
}

/*****************************************************************
Name			:	void Gui_StrCenter
Fuction		:	make the string show center on the LCD
Input			:	(x,y):start coordinate
						fc:front color
						bc:background color
						str:string of english words
						size:size of char number
						mode:mode-0,fill or erase mode;1,over or addtional mode
Output		:	void
Editor		:	@hiyangdong
Copyright :	xiao冯@全动电子
Version		:	V0.1
Time			:	07 Dec. 2015
*****************************************************************/
void Gui_StrCenter(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint8_t *str,uint8_t size,uint8_t mode)
{
	uint16_t len=strlen((const char *)str);
	uint16_t x1=(lcddev.width-len*8)/2;
	Show_Str(x+x1,y,fc,bc,str,size,mode);
} 

/*****************************************************************
Name			:	void Gui_Drawbmp16
Fuction		:	draw a 16 bit bmp picture 40*40
Input			:	(x,y):start coordinate
						p:the address of picture
Output		:	void
Editor		:	@hiyangdong
Copyright :	xiao冯@全动电子
Version		:	V0.1
Time			:	07 Dec. 2015
*****************************************************************/
void Gui_Drawbmp16(uint16_t x, uint16_t y, const unsigned char *p)
{
	int i;
	unsigned char picH, picL;
	LCD_SetWindows(x, y, x + 40 - 1, y + 40 - 1);		//set the windows
	for (i = 0; i<40 * 40; i++)
	{
		picL = *(p + i * 2);											//the low is in the first
		picH = *(p + i * 2 + 1);
		LCD_WR_DATA_16Bit(picH << 8 | picL);
	}
	LCD_SetWindows(0, 0, lcddev.width - 1, lcddev.height - 1);	//recover the windows to full screen

}


