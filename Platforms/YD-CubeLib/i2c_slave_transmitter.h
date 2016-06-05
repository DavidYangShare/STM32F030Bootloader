#include "stm32f0xx_hal.h"
#include <stdint.h>

HAL_StatusTypeDef init_i2c_transmitter(I2C_HandleTypeDef *hi2c, uint8_t * _my_full_addre);
int stop_receiving_i2c(I2C_HandleTypeDef *);
void i2c_receive_callback(I2C_HandleTypeDef *);
uint8_t check_i2c_status(void);
int send_i2c_data(I2C_HandleTypeDef * hi2c, uint8_t _type, uint8_t _size, uint8_t* _data);//dst_addre is string
int get_i2c_data(uint8_t * _type, uint8_t * _size, uint8_t * _content);
int cmp_addr(uint8_t * addr_a, uint8_t * addr_b);
