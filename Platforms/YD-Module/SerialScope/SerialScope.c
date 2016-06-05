/*****************************************************************

File			:	YD-Module\SerialScope\SerialScope.c
Fuction		:	Serial digital Scope
Author		:	@hiyangdong
Version		:	V0.1
Time			:	01 Dec. 2015

*****************************************************************/
#include "serialScope.h"


void SerialOut(uint16_t* data)
{
	uint16_t CRC_Tmp;
	uint8_t TxBuf[10];

	TxBuf[0] = (uint8_t)data[0]&0xff; 				// low 8 bit
	TxBuf[1] = (uint8_t)data[0]>>8;  		// high 8 bit
	TxBuf[2] = (uint8_t)data[1]&0xff;				
	TxBuf[3] = (uint8_t)data[1]>>8;	
	TxBuf[4] = (uint8_t)data[2]&0xff;		
	TxBuf[5] = (uint8_t)data[2]>>8;	
	TxBuf[6] = (uint8_t)data[3]&0xff;		
	TxBuf[7] = (uint8_t)data[3]>>8;	

	CRC_Tmp =	 CRC_CHECK(TxBuf,8);
	TxBuf[8] = CRC_Tmp&0xff;
	TxBuf[9] = CRC_Tmp>>8;

	HAL_UART_Transmit(&BLE_UART , TxBuf, 10, 100);
}


unsigned short CRC_CHECK(unsigned char *Buf, unsigned char CRC_CNT)
{
    unsigned short CRC_Temp;
    unsigned char i,j;
    CRC_Temp = 0xffff;

    for (i=0;i<CRC_CNT; i++){      
        CRC_Temp ^= Buf[i];
        for (j=0;j<8;j++) {
            if (CRC_Temp & 0x01)
                CRC_Temp = (CRC_Temp >>1 ) ^ 0xa001;
            else
                CRC_Temp = CRC_Temp >> 1;
        }
    }
    return(CRC_Temp);
}
