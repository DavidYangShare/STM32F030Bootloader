/*****************************************************************

File			:	YD-Module\BLE_DA14580\BLE.h
Fuction		:	
Author		:	@hiyangdong
Version		:	V1.0
Time			:	30 Nov. 2015

*****************************************************************/

#ifndef _PAH8001_H_
#define _PAH8001_H_

#include "stm32l1xx_hal.h"

#include "i2c_HALDriver.h"
#include "i2c.h"


#define PAHi2c 		hi2c1
#define	PAHAddr		0x66

extern uint8_t heartRate[10];

/*bank0*********************************************************/
#define Product_ID1 	0x00		//Product_ID[11:4];read only
#define Product_ID0		0x01		//Product_ID[3:0]; and Version_ID[3:0]
#define Operation_Mode	0x05		//the register opration modes.[7:6]-10;[5:4]-slp_enh[1:0];bit3-Slp2_enh;bit2-Slp2For;Bit1=Slp1For;Bit0-Weakup
//For Operation_Mode register
#define M3000fps 0b10100000		//Disable sleep mode of 3000fps condition
#define M1000fps 0b10010000		//Disable sleep mode of 1000fps condition
#define MSleep1  0b10110000		//Enable sleep1 mode (normal mode to sleep1 mode with Enter_Time register about 1000ms)
#define MSleep2  0b10111000		//Enable sleep2 mode (normal mode to sleep1 mode with Enter_Time register about 1000ms and about 6s to sleep2 mode)
#define MFSleep1 0b10111100		//Force entering sleep2
#define MFSleep2 0b10110010		//Force entering sleep2
#define MFWeakup 0b10110001		//Force wakeup from sleep mode

#define Configu	0x06		//the configuration of the sensor .Bit7-Reset;[6:4]-000;Bit3-PD_enh;[2:0]-010;
//For Configuration register
#define MNormal 0b00000010		//Normal operation mode (Default)
#define MPowerD 0b00001010		//Power down mode
#define MReset  0b10000010		//reset and the bit will be set to 0

#define Write_Protect 0x09		//Write protect for the register bank0 0x0A~0x7F and bank1 0x00~0x7F
//For Write_Protect register
#define WP_EN		0x00					//read only for those register(Default)
#define WP_DIS	0x5A					//read/written only

#define Sleep1_Setting 0x0A		//Sleep1_Setting register set frequency time for the sleep1 mode. Bit[7:4]-Slp1_freq[3:0]-relative value 0~15 to 6~96ms with 6ms per count;Bit[3:0]-0001;
															//Slp1_freq[3:0]:a scale is 6ms the time is (n+1)*6ms;Default is 18ms. (slp1_freq[3:0] = 0010)
#define Enter_Time		 0x0B		//set enter time for the sleep1 and sleep2 mode. Bit[7:4]-Slp1_etm[3:0];Bit[3:0]-Slp2_etm[3:0];
															//Slp1_etm[3:0]:a scale is 250ms the time is (n+1)*250ms;Default is 1000ms. (slp1_freq[3:0] = 0011)
															//Slp2_etm[3:0]:a scale is 750ms the time is (n+1)*750ms;Default is 600ms.  (slp1_freq[3:0] = 0111)
#define Sleep2_Setting 0x0C		//Sleep2_Setting register set frequency time for the sleep2 mode. Bit[7:4]-Slp2_freq[3:0]-relative value 0~15 to 12~192ms with 6ms per count;Bit[3:0]-0001;
															//Slp1_freq[3:0]:a scale is 12ms the time is (n+1)*12ms;Default is 96ms. (slp1_freq[3:0] = 0111)
#define AE_EnH		0x20				//To Enable/Disable Auto-Exposure(AE)function. Bit[7:1]-0b0000111;Bit1-AE_ENH;
															//1=Enable(Default) 10us~160us is auto adjustment by sensor; reg_value:0x0F
															//0=Disable,LED exposure time is fixed 62.5us(16K).reg_value:0x0E
#define Exposure_Time_L 0x32	//Exposure_Time[7:0];read only
#define Exposure_Time_H 0x33	//Bit[7:2]-reserved; Bit[1:0]-Exposure_Time[9:8];read only
															//Exposure_Time value is 32, the exposure time is 10us;and 512 to 160us;
#define Touch_Resolution 0x4D	//Set Touch_Resolution to zoom in or out touch information. Bit[7:4]-0b0001; Bit[3:0]-Touch_Resolution[3:0];
															//Default is 10.
															//Touch_Resolution = 8, touch information *1/4															
															//Touch_Resolution = 9, touch information *1/2															
															//Touch_Resolution = 11, touch information *2															
#define Touch_Detection 0x59	//Bit7-Touch_flag; Bit[6:0]-reserved;read only
															//Touch Flag is 1 meant touch on and 0 meant touch off


/*bank1*********************************************************/

#define SCI_Touch_EnH 0x07		//To Enable/Disable Touch Flag as SCI pin, Bit6-0 is disable£¬1 is enable Bite3=1£¬the others=0
//For Write_Protect register
#define SCI_EN		0x48				//touch pin ENABLE, When touch, SCI pin is keeping high,When no touch, SCI pin is keeping low.
#define SCI_DIS		0x08				//read/written (Default)

#define Touch_TH  0x23				//To Adjust Touch Threshold to distinguish touch on and off.Touch_TH[7:0];adviced value=0x3C
#define Touch_TH_S_L  0x25		//To Adjust Touch Threshold of Strong Light to distinguish touch on and off under strong light.Touch_TH_S[7:0];
#define Touch_TH_S_H  0x26		//To Adjust Touch Threshold of Strong Light to distinguish touch on and off under strong light.Bit[7:4]-reserved;Bit[3:0]-Touch_TH_S[11:8];

#define HR_Data_Algo_A 0x1A		//To get heart rate data for algorithm reference and the range is from 0 to 255.HR_Data_Algo_A[7:0];
#define Frame_Average	 0x1B		//To get current frame average of brightness and the range is from 0 to 255. Frame_Average[7:0];
#define HR_Data_Algo_C 0x1C		//To get heart rate data for algorithm reference and the range is from 0 to 255.HR_Data_Algo_C[7:0];

#define LED_Step 0x38					//Set LED Step to adjust current.Bit[7:5]-0b111;Bit[4:0]-LED_Step[4:0];
															//LED_Step[4:0]:a scale is 1.5mA,the current is n*1.5mA;Default is 27mA. (LED_Step[4:0] = 18)
#define HR_Raw_Data_A 0x64		//To get heart rate raw data[7:0] of unsigned of 32bit raw data.HR_Raw_Data_A[7:0];
#define HR_Raw_Data_B 0x65		//To get heart rate raw data[7:0] of unsigned of 32bit raw data.HR_Raw_Data_A[15:8];
#define HR_Raw_Data_C 0x66		//To get heart rate raw data[7:0] of unsigned of 32bit raw data.HR_Raw_Data_A[23:16];
#define HR_Raw_Data_D 0x67		//To get heart rate raw data[7:0] of unsigned of 32bit raw data.HR_Raw_Data_A[31:24];
#define HR_Data_Ready 0x68		//To get heart rate data ready status.HR_Raw_Data_A[31:24];Bit[7:4]-reserved;Bit[3:0]-HR_Data_Ready_Status[3:0];
															//if status=0, data is not ready; if status=1, data is ready; if status=2, loss one data....if status=15, loss 14 data 



void PAH8001_init(void);
int8_t PAH8001_HRD(void);
uint8_t PAH8001_Int(void);




#endif
