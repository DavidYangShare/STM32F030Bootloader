#include "i2c_slave_transmitter.h"
#include <string.h>

volatile int flag_i2c_Recv =0;
volatile int flag_i2c_Transmit =0;

uint8_t DataI2CRecv[32];
uint8_t DataI2CSend[32];
uint8_t echo_str[32];

uint16_t master_pre_addr=0x00;
uint8_t master_full_addr[6]="000000";

uint16_t my_pre_addre    = 0x00;
uint8_t my_full_addre[6]  = "00XXXX";


HAL_StatusTypeDef init_i2c_transmitter(I2C_HandleTypeDef *hi2c, uint8_t * _my_full_addre)
{
	my_pre_addre=(_my_full_addre[0]-0x30)*16+_my_full_addre[1]-0x30;
	memcpy(my_full_addre, _my_full_addre, 6);
	memcpy(DataI2CSend, my_full_addre, 6);
	memcpy(echo_str, my_full_addre, 6);
	memcpy(echo_str+6, master_full_addr, 6);
	echo_str[12]=0x00;
	return HAL_I2C_Slave_Receive_IT(hi2c,DataI2CRecv,32);
}

int stop_receiving_i2c(I2C_HandleTypeDef * hi2c)
{
	__HAL_LOCK(hi2c);
	hi2c->State = HAL_I2C_STATE_READY;
  hi2c->ErrorCode   = HAL_I2C_ERROR_NONE;
    
  /* Enable Address Acknowledge */
  hi2c->Instance->CR2 &= ~I2C_CR2_NACK;

  hi2c->pBuffPtr = DataI2CRecv;
  hi2c->XferSize = 16;
  hi2c->XferCount = 16;
    
  /* Process Unlocked */
  __HAL_UNLOCK(hi2c); 
	__HAL_I2C_DISABLE_IT(hi2c,I2C_IT_ERRI | I2C_IT_TCI | I2C_IT_STOPI | I2C_IT_NACKI | I2C_IT_ADDRI | I2C_IT_RXI);
	return 0;
}

void i2c_receive_callback(I2C_HandleTypeDef *hi2c)
{
	if(cmp_addr(DataI2CRecv+6, my_full_addre) == 0x01){
		if(DataI2CRecv[12]==0){	//receive echo
			flag_i2c_Transmit = 1;

		}else{									//receive data, send echo
			if(DataI2CRecv[12]==0xFE){//receive address of master
				master_pre_addr=(DataI2CRecv[0]-0x30)*16+DataI2CRecv[1]-0x30;
				memcpy(master_full_addr, DataI2CRecv, 6);
				memcpy(echo_str+6, master_full_addr, 6);
				HAL_I2C_Master_Transmit(hi2c, master_pre_addr, echo_str, 32, 10);
			}else{
				HAL_I2C_Master_Transmit(hi2c, master_pre_addr, echo_str, 32, 10);
				flag_i2c_Recv = 1;
			}

		}
	}
	HAL_I2C_Slave_Receive_IT(hi2c, DataI2CRecv, 32);
}

uint8_t check_i2c_status(void)
{
	return flag_i2c_Recv;
}

int send_i2c_data(I2C_HandleTypeDef * hi2c, uint8_t _type, uint8_t _size, uint8_t* _data)//dst_addre is string
{
	stop_receiving_i2c(hi2c);
	
	memcpy(DataI2CSend, my_full_addre, 6);
	memcpy(DataI2CSend+6, master_full_addr, 6);
	DataI2CSend[12]=_type;
	DataI2CSend[13]=_size;
	memcpy(DataI2CSend+14, _data, _size);
	
	HAL_I2C_Master_Transmit(hi2c, master_pre_addr, DataI2CSend, 32,10);
	
	HAL_I2C_Slave_Receive_IT(hi2c, DataI2CRecv, 32);
	
	flag_i2c_Transmit = 0;
	uint8_t loop_num=0;
	while(flag_i2c_Transmit==0){
		HAL_Delay(100);
		if(++loop_num > 10){
			return 0;
		}
	}

	return 1;
}

int get_i2c_data(uint8_t * _type, uint8_t * _size, uint8_t * _content)
{
	if(flag_i2c_Recv==1){
		*_size=DataI2CRecv[13];
		memcpy(_content, DataI2CRecv + 14, 18);
		*_type=DataI2CRecv[12];
		flag_i2c_Recv=0;
		return 1;
	}else{
		return 0;
	}
}

int cmp_addr(uint8_t * addr_a, uint8_t * addr_b)
{
	int j=0;
	for(int i = 0; i<6; i++){
		j += addr_a[i] == addr_b[i] ? 1 : 0;
	}
	return j == 6;
}

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	if(hi2c->Instance == I2C1)
	{
		i2c_receive_callback(hi2c);
		HAL_I2C_Slave_Receive_IT(hi2c,DataI2CRecv,32);
	}
}
