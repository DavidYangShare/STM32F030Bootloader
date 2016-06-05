/*****************************************************************

File			:	YD-Module\BMP180\bmp180.h
Fuction		:	BMP180 chip with Digital pressure sensor
Author		:	@hiyangdong
Version		:	V1.0
Time			:	12 Nov. 2015
Thanks		:	https://github.com/kriswiner/MPU-9250/blob/master/STM32F401/MPU9250.h

*****************************************************************/
#include "mpu9250.h"


uint8_t Ascale = AFS_2G;     // AFS_2G, AFS_4G, AFS_8G, AFS_16G
uint8_t Gscale = GFS_250DPS; // GFS_250DPS, GFS_500DPS, GFS_1000DPS, GFS_2000DPS
uint8_t Mscale = MFS_16BITS; // MFS_14BITS or MFS_16BITS, 14-bit or 16-bit magnetometer resolution
uint8_t Mmode = 0x06;        // Either 8 Hz 0x02) or 100 Hz (0x06) magnetometer data ODR  
float aRes, gRes, mRes;      // scale resolutions per LSB for the sensors


int16_t accelCount[3];  // Stores the 16-bit signed accelerometer sensor output
int16_t gyroCount[3];   // Stores the 16-bit signed gyro sensor output
int16_t magCount[3];    // Stores the 16-bit signed magnetometer sensor output

float magCalibration[3] = {0, 0, 0}, magbias[3] = {0, 0, 0};  // Factory mag calibration and mag bias
float gyroBias[3] = {0, 0, 0}, accelBias[3] = {0, 0, 0}; // Bias corrections for gyro and accelerometer
float ax, ay, az, gx, gy, gz, mx, my, mz; // variables to hold latest sensor data values 
int16_t tempCount;   // Stores the real internal chip temperature in degrees Celsius
float temperature;
float SelfTest[6];

int delt_t = 0; // used to control display output rate
int count = 0;  // used to control display output rate
float pitch, yaw, roll;
float deltat = 0.0f;                             // integration interval for both filter schemes
int lastUpdate = 0, firstUpdate = 0, Now = 0;    // used to calculate integration interval                               // used to calculate integration interval
float q[4] = {1.0f, 0.0f, 0.0f, 0.0f};           // vector to hold quaternion
float eInt[3] = {0.0f, 0.0f, 0.0f};              // vector to hold integral error for Mahony method



void getMres() 
{
  switch (Mscale)
  {
    // Possible magnetometer scales (and their register bit settings) are:
    // 14 bit resolution (0) and 16 bit resolution (1)
    case MFS_14BITS:
          mRes = 10.0*4219.0/8190.0; // Proper scale to return milliGauss
          break;
    case MFS_16BITS:
          mRes = 10.0*4219.0/32760.0; // Proper scale to return milliGauss
          break;
  }
}

void getGres() 
{
  switch (Gscale)
  {
    // Possible gyro scales (and their register bit settings) are:
    // 250 DPS (00), 500 DPS (01), 1000 DPS (10), and 2000 DPS  (11). 
        // Here's a bit of an algorith to calculate DPS/(ADC tick) based on that 2-bit value:
    case GFS_250DPS:
          gRes = 250.0/32768.0;
          break;
    case GFS_500DPS:
          gRes = 500.0/32768.0;
          break;
    case GFS_1000DPS:
          gRes = 1000.0/32768.0;
          break;
    case GFS_2000DPS:
          gRes = 2000.0/32768.0;
          break;
  }
}

void getAres() {
  switch (Ascale)
  {
    // Possible accelerometer scales (and their register bit settings) are:
    // 2 Gs (00), 4 Gs (01), 8 Gs (10), and 16 Gs  (11). 
        // Here's a bit of an algorith to calculate DPS/(ADC tick) based on that 2-bit value:
    case AFS_2G:
          aRes = 2.0/32768.0;
          break;
    case AFS_4G:
          aRes = 4.0/32768.0;
          break;
    case AFS_8G:
          aRes = 8.0/32768.0;
          break;
    case AFS_16G:
          aRes = 16.0/32768.0;
          break;
  }
}


void readAccelData(int16_t * destination)
{
  uint8_t rawData[6];  // x/y/z accel register data stored here
  I2C_ReadBytes(&MPUi2c,MPU9250_ADDRESS, ACCEL_XOUT_H, rawData,6);  			// Read the six raw data registers into data array
  destination[0] = (int16_t)(((int16_t)rawData[0] << 8) | rawData[1]) ;		// Turn the MSB and LSB into a signed 16-bit value
  destination[1] = (int16_t)(((int16_t)rawData[2] << 8) | rawData[3]) ;  
  destination[2] = (int16_t)(((int16_t)rawData[4] << 8) | rawData[5]) ; 
}

void readGyroData(int16_t * destination)
{
  uint8_t rawData[6];  // x/y/z gyro register data stored here
  I2C_ReadBytes(&MPUi2c,MPU9250_ADDRESS, GYRO_XOUT_H, rawData,6);				 	// Read the six raw data registers sequentially into data array
  destination[0] = (int16_t)(((int16_t)rawData[0] << 8) | rawData[1]) ;  	// Turn the MSB and LSB into a signed 16-bit value
  destination[1] = (int16_t)(((int16_t)rawData[2] << 8) | rawData[3]) ;  
  destination[2] = (int16_t)(((int16_t)rawData[4] << 8) | rawData[5]) ; 
}

void readMagData(int16_t * destination)
{
  uint8_t rawData[7];  // x/y/z gyro register data, ST2 register stored here, must read ST2 at end of data acquisition
  if(I2C_ReadByte(&MPUi2c,AK8963_ADDRESS, AK8963_ST1) & 0x01) { // wait for magnetometer data ready bit to be set
  I2C_ReadBytes(&MPUi2c,AK8963_ADDRESS, AK8963_XOUT_L,rawData, 7);  // Read the six raw data and ST2 registers sequentially into data array
  uint8_t c = rawData[6]; // End data read by reading ST2 register
    if(!(c & 0x08)) 
		{ // Check if magnetic sensor overflow set, if not then report data
			destination[0] = (int16_t)(((int16_t)rawData[1] << 8) | rawData[0]);  // Turn the MSB and LSB into a signed 16-bit value
			destination[1] = (int16_t)(((int16_t)rawData[3] << 8) | rawData[2]) ;  // Data stored as little Endian
			destination[2] = (int16_t)(((int16_t)rawData[5] << 8) | rawData[4]) ; 
		}

	}
}

int16_t readTempData()
{
  uint8_t rawData[2];  // x/y/z gyro register data stored here
  I2C_ReadBytes(&MPUi2c,MPU9250_ADDRESS, TEMP_OUT_H, rawData, 2);  // Read the two raw data registers sequentially into data array 
  return (int16_t)(((int16_t)rawData[0]) << 8 | rawData[1]) ;  // Turn the MSB and LSB into a 16-bit value
}

void resetMPU9250() 
{
  // reset device
  I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, PWR_MGMT_1, 0x80); // Write a one to bit 7 reset bit; toggle reset device
  HAL_Delay(100);	//wait(0.1);
}

void initAK8963(float * destination)
{
  // First extract the factory calibration for each magnetometer axis
  uint8_t rawData[3];  // x/y/z gyro calibration data stored here
  printf("AK_read0");
	I2C_WriteByte(&MPUi2c,AK8963_ADDRESS, AK8963_CNTL, 0x00); // Power down magnetometer  
  printf("AK_read1");
	HAL_Delay(10);//wait(0.01);
  I2C_WriteByte(&MPUi2c,AK8963_ADDRESS, AK8963_CNTL, 0x0F); // Enter Fuse ROM access mode
  HAL_Delay(10);//wait(0.01);
  I2C_ReadBytes(&MPUi2c,AK8963_ADDRESS, AK8963_ASAX, rawData, 3);  // Read the x-, y-, and z-axis calibration values
  destination[0] =  (float)(rawData[0] - 128)/256.0f + 1.0f;   // Return x-axis sensitivity adjustment values, etc.
  destination[1] =  (float)(rawData[1] - 128)/256.0f + 1.0f;  
  destination[2] =  (float)(rawData[2] - 128)/256.0f + 1.0f; 
//			for(int ii=0;ii<3;ii++)
//		{
//			printf("Value%f\r\n",destination[ii]);
//		}
  I2C_WriteByte(&MPUi2c,AK8963_ADDRESS, AK8963_CNTL, 0x00); // Power down magnetometer  
  HAL_Delay(10);//wait(0.01);
  // Configure the magnetometer for continuous read and highest resolution
  // set Mscale bit 4 to 1 (0) to enable 16 (14) bit resolution in CNTL register,
  // and enable continuous mode data acquisition Mmode (bits [3:0]), 0010 for 8 Hz and 0110 for 100 Hz sample rates
  I2C_WriteByte(&MPUi2c,AK8963_ADDRESS, AK8963_CNTL, Mscale << 4 | Mmode); // Set magnetometer data resolution and sample ODR
  HAL_Delay(10);//wait(0.01);
}

void initMPU9250(void)
{  
 // Initialize MPU9250 device
 // wake up device
  I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, PWR_MGMT_1, 0x00); // Clear sleep mode bit (6), enable all sensors 
  HAL_Delay(100);	//wait(0.1); // Delay 100 ms for PLL to get established on x-axis gyro; should check for PLL ready interrupt  

 // get stable time source
  I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, PWR_MGMT_1, 0x01);  // Set clock source to be PLL with x-axis gyroscope reference, bits 2:0 = 001

 // Configure Gyro and Accelerometer
 // Disable FSYNC and set accelerometer and gyro bandwidth to 44 and 42 Hz, respectively; 
 // DLPF_CFG = bits 2:0 = 010; this sets the sample rate at 1 kHz for both
 // Maximum delay is 4.9 ms which is just over a 200 Hz maximum rate
  I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, CONFIG, 0x03);  
 
 // Set sample rate = gyroscope output rate/(1 + SMPLRT_DIV)
  I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, SMPLRT_DIV, 0x04);  // Use a 200 Hz rate; the same rate set in CONFIG above
 
 // Set gyroscope full scale range
 // Range selects FS_SEL and AFS_SEL are 0 - 3, so 2-bit values are left-shifted into positions 4:3
  uint8_t c =  I2C_ReadByte(&MPUi2c,MPU9250_ADDRESS, GYRO_CONFIG);
  I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, GYRO_CONFIG, c & ~0xE0); // Clear self-test bits [7:5] 
  I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, GYRO_CONFIG, c & ~0x18); // Clear AFS bits [4:3]
  I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, GYRO_CONFIG, c | Gscale << 3); // Set full scale range for the gyro
   
 // Set accelerometer configuration
  c =  I2C_ReadByte(&MPUi2c,MPU9250_ADDRESS, ACCEL_CONFIG);
  I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, ACCEL_CONFIG, c & ~0xE0); // Clear self-test bits [7:5] 
  I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, ACCEL_CONFIG, c & ~0x18); // Clear AFS bits [4:3]
  I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, ACCEL_CONFIG, c | Ascale << 3); // Set full scale range for the accelerometer 

 // Set accelerometer sample rate configuration
 // It is possible to get a 4 kHz sample rate from the accelerometer by choosing 1 for
 // accel_fchoice_b bit [3]; in this case the bandwidth is 1.13 kHz
  c = I2C_ReadByte(&MPUi2c,MPU9250_ADDRESS, ACCEL_CONFIG2);
  I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, ACCEL_CONFIG2, c & ~0x0F); // Clear accel_fchoice_b (bit 3) and A_DLPFG (bits [2:0])  
  I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, ACCEL_CONFIG2, c | 0x03); // Set accelerometer rate to 1 kHz and bandwidth to 41 Hz

 // The accelerometer, gyro, and thermometer are set to 1 kHz sample rates, 
 // but all these rates are further reduced by a factor of 5 to 200 Hz because of the SMPLRT_DIV setting

  // Configure Interrupts and Bypass Enable
  // Set interrupt pin active high, push-pull, and clear on read of INT_STATUS, enable I2C_BYPASS_EN so additional chips 
  // can join the I2C bus and all can be controlled by the Arduino as master
   I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, INT_PIN_CFG, 0x22);    
   I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, INT_ENABLE, 0x01);  // Enable data ready (bit 0) interrupt
}


// Function which accumulates gyro and accelerometer data after device initialization. It calculates the average
// of the at-rest readings and then loads the resulting offsets into accelerometer and gyro bias registers.
void calibrateMPU9250(float * dest1, float * dest2)
{  
  uint8_t data[12]; // data array to hold accelerometer and gyro x, y, z, data
  uint16_t ii, packet_count, fifo_count;
  int32_t gyro_bias[3] = {0, 0, 0}, accel_bias[3] = {0, 0, 0};
  
// reset device, reset all registers, clear gyro and accelerometer bias registers
  I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, PWR_MGMT_1, 0x80); // Write a one to bit 7 reset bit; toggle reset device
  HAL_Delay(100);//wait(0.1);  
   
// get stable time source
// Set clock source to be PLL with x-axis gyroscope reference, bits 2:0 = 001
  I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, PWR_MGMT_1, 0x01);  
  I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, PWR_MGMT_2, 0x00); 
  HAL_Delay(200);//wait(0.2);
  
// Configure device for bias calculation
  I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, INT_ENABLE, 0x00);   // Disable all interrupts
  I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, FIFO_EN, 0x00);      // Disable FIFO
  I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, PWR_MGMT_1, 0x00);   // Turn on internal clock source
  I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, I2C_MST_CTRL, 0x00); // Disable I2C master
  I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, USER_CTRL, 0x00);    // Disable FIFO and I2C master modes
  I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, USER_CTRL, 0x0C);    // Reset FIFO and DMP
  HAL_Delay(15);//wait(0.015);
  
// Configure MPU9250 gyro and accelerometer for bias calculation
  I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, CONFIG, 0x01);      // Set low-pass filter to 188 Hz
  I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, SMPLRT_DIV, 0x00);  // Set sample rate to 1 kHz
  I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, GYRO_CONFIG, 0x00);  // Set gyro full-scale to 250 degrees per second, maximum sensitivity
  I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, ACCEL_CONFIG, 0x00); // Set accelerometer full-scale to 2 g, maximum sensitivity
 
  uint16_t  gyrosensitivity  = 131;   // = 131 LSB/degrees/sec
  uint16_t  accelsensitivity = 16384;  // = 16384 LSB/g

// Configure FIFO to capture accelerometer and gyro data for bias calculation
  I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, USER_CTRL, 0x40);   // Enable FIFO  
  I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, FIFO_EN, 0x78);     // Enable gyro and accelerometer sensors for FIFO (max size 512 bytes in MPU-9250)
  HAL_Delay(40);//wait(0.04); // accumulate 40 samples in 80 milliseconds = 480 bytes

// At end of sample accumulation, turn off FIFO sensor read
  I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, FIFO_EN, 0x00);        // Disable gyro and accelerometer sensors for FIFO
  I2C_ReadBytes(&MPUi2c,MPU9250_ADDRESS, FIFO_COUNTH, data, 2); // read FIFO sample count
  fifo_count = ((uint16_t)data[0] << 8) | data[1];
  packet_count = fifo_count/12;// How many sets of full gyro and accelerometer data for averaging

  for (ii = 0; ii < packet_count; ii++) {
    int16_t accel_temp[3] = {0, 0, 0}, gyro_temp[3] = {0, 0, 0};
    I2C_ReadBytes(&MPUi2c,MPU9250_ADDRESS, FIFO_R_W, data, 12); // read data for averaging
    accel_temp[0] = (int16_t) (((int16_t)data[0] << 8) | data[1]  ) ;  // Form signed 16-bit integer for each sample in FIFO
    accel_temp[1] = (int16_t) (((int16_t)data[2] << 8) | data[3]  ) ;
    accel_temp[2] = (int16_t) (((int16_t)data[4] << 8) | data[5]  ) ;    
    gyro_temp[0]  = (int16_t) (((int16_t)data[6] << 8) | data[7]  ) ;
    gyro_temp[1]  = (int16_t) (((int16_t)data[8] << 8) | data[9]  ) ;
    gyro_temp[2]  = (int16_t) (((int16_t)data[10] << 8) | data[11]) ;
    
    accel_bias[0] += (int32_t) accel_temp[0]; // Sum individual signed 16-bit biases to get accumulated signed 32-bit biases
    accel_bias[1] += (int32_t) accel_temp[1];
    accel_bias[2] += (int32_t) accel_temp[2];
    gyro_bias[0]  += (int32_t) gyro_temp[0];
    gyro_bias[1]  += (int32_t) gyro_temp[1];
    gyro_bias[2]  += (int32_t) gyro_temp[2];
            
}
    accel_bias[0] /= (int32_t) packet_count; // Normalize sums to get average count biases
    accel_bias[1] /= (int32_t) packet_count;
    accel_bias[2] /= (int32_t) packet_count;
    gyro_bias[0]  /= (int32_t) packet_count;
    gyro_bias[1]  /= (int32_t) packet_count;
    gyro_bias[2]  /= (int32_t) packet_count;
    
  if(accel_bias[2] > 0L) {accel_bias[2] -= (int32_t) accelsensitivity;}  // Remove gravity from the z-axis accelerometer bias calculation
  else {accel_bias[2] += (int32_t) accelsensitivity;}
 
// Construct the gyro biases for push to the hardware gyro bias registers, which are reset to zero upon device startup
  data[0] = (-gyro_bias[0]/4  >> 8) & 0xFF; // Divide by 4 to get 32.9 LSB per deg/s to conform to expected bias input format
  data[1] = (-gyro_bias[0]/4)       & 0xFF; // Biases are additive, so change sign on calculated average gyro biases
  data[2] = (-gyro_bias[1]/4  >> 8) & 0xFF;
  data[3] = (-gyro_bias[1]/4)       & 0xFF;
  data[4] = (-gyro_bias[2]/4  >> 8) & 0xFF;
  data[5] = (-gyro_bias[2]/4)       & 0xFF;

/// Push gyro biases to hardware registers
/*  I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, XG_OFFSET_H, data[0]);
  I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, XG_OFFSET_L, data[1]);
  I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, YG_OFFSET_H, data[2]);
  I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, YG_OFFSET_L, data[3]);
  I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, ZG_OFFSET_H, data[4]);
  I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, ZG_OFFSET_L, data[5]);
*/
  dest1[0] = (float) gyro_bias[0]/(float) gyrosensitivity; // construct gyro bias in deg/s for later manual subtraction
  dest1[1] = (float) gyro_bias[1]/(float) gyrosensitivity;
  dest1[2] = (float) gyro_bias[2]/(float) gyrosensitivity;

// Construct the accelerometer biases for push to the hardware accelerometer bias registers. These registers contain
// factory trim values which must be added to the calculated accelerometer biases; on boot up these registers will hold
// non-zero values. In addition, bit 0 of the lower byte must be preserved since it is used for temperature
// compensation calculations. Accelerometer bias registers expect bias input as 2048 LSB per g, so that
// the accelerometer biases calculated above must be divided by 8.

  int32_t accel_bias_reg[3] = {0, 0, 0}; // A place to hold the factory accelerometer trim biases
  I2C_ReadBytes(&MPUi2c,MPU9250_ADDRESS, XA_OFFSET_H, data, 2); // Read factory accelerometer trim values
  accel_bias_reg[0] = (int16_t) ((int16_t)data[0] << 8) | data[1];
  I2C_ReadBytes(&MPUi2c,MPU9250_ADDRESS, YA_OFFSET_H, data, 2);
  accel_bias_reg[1] = (int16_t) ((int16_t)data[0] << 8) | data[1];
  I2C_ReadBytes(&MPUi2c,MPU9250_ADDRESS, ZA_OFFSET_H, data, 2);
  accel_bias_reg[2] = (int16_t) ((int16_t)data[0] << 8) | data[1];
  
  uint32_t mask = 1uL; // Define mask for temperature compensation bit 0 of lower byte of accelerometer bias registers
  uint8_t mask_bit[3] = {0, 0, 0}; // Define array to hold mask bit for each accelerometer bias axis
  
  for(ii = 0; ii < 3; ii++) {
    if(accel_bias_reg[ii] & mask) mask_bit[ii] = 0x01; // If temperature compensation bit is set, record that fact in mask_bit
  }

  // Construct total accelerometer bias, including calculated average accelerometer bias from above
  accel_bias_reg[0] -= (accel_bias[0]/8); // Subtract calculated averaged accelerometer bias scaled to 2048 LSB/g (16 g full scale)
  accel_bias_reg[1] -= (accel_bias[1]/8);
  accel_bias_reg[2] -= (accel_bias[2]/8);
 
  data[0] = (accel_bias_reg[0] >> 8) & 0xFF;
  data[1] = (accel_bias_reg[0])      & 0xFF;
  data[1] = data[1] | mask_bit[0]; // preserve temperature compensation bit when writing back to accelerometer bias registers
  data[2] = (accel_bias_reg[1] >> 8) & 0xFF;
  data[3] = (accel_bias_reg[1])      & 0xFF;
  data[3] = data[3] | mask_bit[1]; // preserve temperature compensation bit when writing back to accelerometer bias registers
  data[4] = (accel_bias_reg[2] >> 8) & 0xFF;
  data[5] = (accel_bias_reg[2])      & 0xFF;
  data[5] = data[5] | mask_bit[2]; // preserve temperature compensation bit when writing back to accelerometer bias registers

// Apparently this is not working for the acceleration biases in the MPU-9250
// Are we handling the temperature correction bit properly?
// Push accelerometer biases to hardware registers
/*  I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, XA_OFFSET_H, data[0]);
  I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, XA_OFFSET_L, data[1]);
  I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, YA_OFFSET_H, data[2]);
  I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, YA_OFFSET_L, data[3]);
  I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, ZA_OFFSET_H, data[4]);
  I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, ZA_OFFSET_L, data[5]);
*/
// Output scaled accelerometer biases for manual subtraction in the main program
   dest2[0] = (float)accel_bias[0]/(float)accelsensitivity; 
   dest2[1] = (float)accel_bias[1]/(float)accelsensitivity;
   dest2[2] = (float)accel_bias[2]/(float)accelsensitivity;
}

// Accelerometer and gyroscope self test; check calibration wrt factory settings
void MPU9250SelfTest(float * destination) // Should return percent deviation from factory trim values, +/- 14 or less deviation is a pass
{
   uint8_t rawData[6] = {0, 0, 0, 0, 0, 0};
   uint8_t selfTest[6];
   int16_t gAvg[3], aAvg[3], aSTAvg[3], gSTAvg[3];
   float factoryTrim[6];
   uint8_t FS = 0;
   
  I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, SMPLRT_DIV, 0x00); // Set gyro sample rate to 1 kHz
  I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, CONFIG, 0x02); // Set gyro sample rate to 1 kHz and DLPF to 92 Hz
  I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, GYRO_CONFIG, 1<<FS); // Set full scale range for the gyro to 250 dps
  I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, ACCEL_CONFIG2, 0x02); // Set accelerometer rate to 1 kHz and bandwidth to 92 Hz
  I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, ACCEL_CONFIG, 1<<FS); // Set full scale range for the accelerometer to 2 g

  for( int ii = 0; ii < 200; ii++) { // get average current values of gyro and acclerometer
  
  I2C_ReadBytes(&MPUi2c,MPU9250_ADDRESS, ACCEL_XOUT_H, rawData, 6); // Read the six raw data registers into data array
  aAvg[0] += (int16_t)(((int16_t)rawData[0] << 8) | rawData[1]) ; // Turn the MSB and LSB into a signed 16-bit value
  aAvg[1] += (int16_t)(((int16_t)rawData[2] << 8) | rawData[3]) ;
  aAvg[2] += (int16_t)(((int16_t)rawData[4] << 8) | rawData[5]) ;
  
  I2C_ReadBytes(&MPUi2c,MPU9250_ADDRESS, GYRO_XOUT_H, rawData, 6); // Read the six raw data registers sequentially into data array
  gAvg[0] += (int16_t)(((int16_t)rawData[0] << 8) | rawData[1]) ; // Turn the MSB and LSB into a signed 16-bit value
  gAvg[1] += (int16_t)(((int16_t)rawData[2] << 8) | rawData[3]) ;
  gAvg[2] += (int16_t)(((int16_t)rawData[4] << 8) | rawData[5]) ;
  }
  
  for (int ii =0; ii < 3; ii++) { // Get average of 200 values and store as average current readings
  aAvg[ii] /= 200;
  gAvg[ii] /= 200;
  }
  
// Configure the accelerometer for self-test
   I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, ACCEL_CONFIG, 0xE0); // Enable self test on all three axes and set accelerometer range to +/- 2 g
   I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, GYRO_CONFIG, 0xE0); // Enable self test on all three axes and set gyro range to +/- 250 degrees/s
   HAL_Delay(200); //wait(25); // Delay a while to let the device stabilize

  for( int ii = 0; ii < 200; ii++) { // get average self-test values of gyro and acclerometer
  
  I2C_ReadBytes(&MPUi2c,MPU9250_ADDRESS, ACCEL_XOUT_H, rawData, 6); // Read the six raw data registers into data array
  aSTAvg[0] += (int16_t)(((int16_t)rawData[0] << 8) | rawData[1]) ; // Turn the MSB and LSB into a signed 16-bit value
  aSTAvg[1] += (int16_t)(((int16_t)rawData[2] << 8) | rawData[3]) ;
  aSTAvg[2] += (int16_t)(((int16_t)rawData[4] << 8) | rawData[5]) ;
  
  I2C_ReadBytes(&MPUi2c,MPU9250_ADDRESS, GYRO_XOUT_H, rawData, 6); // Read the six raw data registers sequentially into data array
  gSTAvg[0] += (int16_t)(((int16_t)rawData[0] << 8) | rawData[1]) ; // Turn the MSB and LSB into a signed 16-bit value
  gSTAvg[1] += (int16_t)(((int16_t)rawData[2] << 8) | rawData[3]) ;
  gSTAvg[2] += (int16_t)(((int16_t)rawData[4] << 8) | rawData[5]) ;
  }
  
  for (int ii =0; ii < 3; ii++) { // Get average of 200 values and store as average self-test readings
  aSTAvg[ii] /= 200;
  gSTAvg[ii] /= 200;
  }
  
 // Configure the gyro and accelerometer for normal operation
   I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, ACCEL_CONFIG, 0x00);
   I2C_WriteByte(&MPUi2c,MPU9250_ADDRESS, GYRO_CONFIG, 0x00);
   HAL_Delay(500); //delay(25); // Delay a while to let the device stabilize
   
   // Retrieve accelerometer and gyro factory Self-Test Code from USR_Reg
   selfTest[0] = I2C_ReadByte(&MPUi2c,MPU9250_ADDRESS, SELF_TEST_X_ACCEL); // X-axis accel self-test results
   selfTest[1] = I2C_ReadByte(&MPUi2c,MPU9250_ADDRESS, SELF_TEST_Y_ACCEL); // Y-axis accel self-test results
   selfTest[2] = I2C_ReadByte(&MPUi2c,MPU9250_ADDRESS, SELF_TEST_Z_ACCEL); // Z-axis accel self-test results
   selfTest[3] = I2C_ReadByte(&MPUi2c,MPU9250_ADDRESS, SELF_TEST_X_GYRO); // X-axis gyro self-test results
   selfTest[4] = I2C_ReadByte(&MPUi2c,MPU9250_ADDRESS, SELF_TEST_Y_GYRO); // Y-axis gyro self-test results
   selfTest[5] = I2C_ReadByte(&MPUi2c,MPU9250_ADDRESS, SELF_TEST_Z_GYRO); // Z-axis gyro self-test results

  // Retrieve factory self-test value from self-test code reads
   factoryTrim[0] = (float)(2620/1<<FS)*(pow( 1.01 , ((float)selfTest[0] - 1.0) )); // FT[Xa] factory trim calculation
   factoryTrim[1] = (float)(2620/1<<FS)*(pow( 1.01 , ((float)selfTest[1] - 1.0) )); // FT[Ya] factory trim calculation
   factoryTrim[2] = (float)(2620/1<<FS)*(pow( 1.01 , ((float)selfTest[2] - 1.0) )); // FT[Za] factory trim calculation
   factoryTrim[3] = (float)(2620/1<<FS)*(pow( 1.01 , ((float)selfTest[3] - 1.0) )); // FT[Xg] factory trim calculation
   factoryTrim[4] = (float)(2620/1<<FS)*(pow( 1.01 , ((float)selfTest[4] - 1.0) )); // FT[Yg] factory trim calculation
   factoryTrim[5] = (float)(2620/1<<FS)*(pow( 1.01 , ((float)selfTest[5] - 1.0) )); // FT[Zg] factory trim calculation
		for(int ii=0;ii<6;ii++)
		{
			printf("Value%f\r\n",factoryTrim[ii]);
		}
//	printf("Z%d\r\n",T_Z);
 // Report results as a ratio of (STR - FT)/FT; the change from Factory Trim of the Self-Test Response
 // To get percent, must multiply by 100
   for (int i = 0; i < 3; i++) {
     destination[i] = 100.0*((float)(aSTAvg[i] - aAvg[i]))/factoryTrim[i]; // Report percent differences
     destination[i+3] = 100.0*((float)(gSTAvg[i] - gAvg[i]))/factoryTrim[i+3]; // Report percent differences
   }
   
}



// Implementation of Sebastian Madgwick's "...efficient orientation filter for... inertial/magnetic sensor arrays"
// (see http://www.x-io.co.uk/category/open-source/ for examples and more details)
// which fuses acceleration, rotation rate, and magnetic moments to produce a quaternion-based estimate of absolute
// device orientation -- which can be converted to yaw, pitch, and roll. Useful for stabilizing quadcopters, etc.
// The performance of the orientation filter is at least as good as conventional Kalman-based filtering algorithms
// but is much less computationally intensive---it can be performed on a 3.3 V Pro Mini operating at 8 MHz!
void MadgwickQuaternionUpdate(float ax, float ay, float az, float gx, float gy, float gz, float mx, float my, float mz)
{
		float q1 = q[0], q2 = q[1], q3 = q[2], q4 = q[3];   // short name local variable for readability
		float norm;
		float hx, hy, _2bx, _2bz;
		float s1, s2, s3, s4;
		float qDot1, qDot2, qDot3, qDot4;

		// Auxiliary variables to avoid repeated arithmetic
		float _2q1mx;
		float _2q1my;
		float _2q1mz;
		float _2q2mx;
		float _4bx;
		float _4bz;
		float _2q1 = 2.0f * q1;
		float _2q2 = 2.0f * q2;
		float _2q3 = 2.0f * q3;
		float _2q4 = 2.0f * q4;
		float _2q1q3 = 2.0f * q1 * q3;
		float _2q3q4 = 2.0f * q3 * q4;
		float q1q1 = q1 * q1;
		float q1q2 = q1 * q2;
		float q1q3 = q1 * q3;
		float q1q4 = q1 * q4;
		float q2q2 = q2 * q2;
		float q2q3 = q2 * q3;
		float q2q4 = q2 * q4;
		float q3q3 = q3 * q3;
		float q3q4 = q3 * q4;
		float q4q4 = q4 * q4;

		// Normalise accelerometer measurement
		norm = sqrt(ax * ax + ay * ay + az * az);
		if (norm == 0.0f) return; // handle NaN
		norm = 1.0f/norm;
		ax *= norm;
		ay *= norm;
		az *= norm;

		// Normalise magnetometer measurement
		norm = sqrt(mx * mx + my * my + mz * mz);
		if (norm == 0.0f) return; // handle NaN
		norm = 1.0f/norm;
		mx *= norm;
		my *= norm;
		mz *= norm;

		// Reference direction of Earth's magnetic field
		_2q1mx = 2.0f * q1 * mx;
		_2q1my = 2.0f * q1 * my;
		_2q1mz = 2.0f * q1 * mz;
		_2q2mx = 2.0f * q2 * mx;
		hx = mx * q1q1 - _2q1my * q4 + _2q1mz * q3 + mx * q2q2 + _2q2 * my * q3 + _2q2 * mz * q4 - mx * q3q3 - mx * q4q4;
		hy = _2q1mx * q4 + my * q1q1 - _2q1mz * q2 + _2q2mx * q3 - my * q2q2 + my * q3q3 + _2q3 * mz * q4 - my * q4q4;
		_2bx = sqrt(hx * hx + hy * hy);
		_2bz = -_2q1mx * q3 + _2q1my * q2 + mz * q1q1 + _2q2mx * q4 - mz * q2q2 + _2q3 * my * q4 - mz * q3q3 + mz * q4q4;
		_4bx = 2.0f * _2bx;
		_4bz = 2.0f * _2bz;

		// Gradient decent algorithm corrective step
		s1 = -_2q3 * (2.0f * q2q4 - _2q1q3 - ax) + _2q2 * (2.0f * q1q2 + _2q3q4 - ay) - _2bz * q3 * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (-_2bx * q4 + _2bz * q2) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + _2bx * q3 * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
		s2 = _2q4 * (2.0f * q2q4 - _2q1q3 - ax) + _2q1 * (2.0f * q1q2 + _2q3q4 - ay) - 4.0f * q2 * (1.0f - 2.0f * q2q2 - 2.0f * q3q3 - az) + _2bz * q4 * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (_2bx * q3 + _2bz * q1) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + (_2bx * q4 - _4bz * q2) * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
		s3 = -_2q1 * (2.0f * q2q4 - _2q1q3 - ax) + _2q4 * (2.0f * q1q2 + _2q3q4 - ay) - 4.0f * q3 * (1.0f - 2.0f * q2q2 - 2.0f * q3q3 - az) + (-_4bx * q3 - _2bz * q1) * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (_2bx * q2 + _2bz * q4) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + (_2bx * q1 - _4bz * q3) * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
		s4 = _2q2 * (2.0f * q2q4 - _2q1q3 - ax) + _2q3 * (2.0f * q1q2 + _2q3q4 - ay) + (-_4bx * q4 + _2bz * q2) * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (-_2bx * q1 + _2bz * q3) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + _2bx * q2 * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
		norm = sqrt(s1 * s1 + s2 * s2 + s3 * s3 + s4 * s4);    // normalise step magnitude
		norm = 1.0f/norm;
		s1 *= norm;
		s2 *= norm;
		s3 *= norm;
		s4 *= norm;

		// Compute rate of change of quaternion
		qDot1 = 0.5f * (-q2 * gx - q3 * gy - q4 * gz) - beta * s1;
		qDot2 = 0.5f * (q1 * gx + q3 * gz - q4 * gy) - beta * s2;
		qDot3 = 0.5f * (q1 * gy - q2 * gz + q4 * gx) - beta * s3;
		qDot4 = 0.5f * (q1 * gz + q2 * gy - q3 * gx) - beta * s4;

		// Integrate to yield quaternion
		q1 += qDot1 * deltat;
		q2 += qDot2 * deltat;
		q3 += qDot3 * deltat;
		q4 += qDot4 * deltat;
		norm = sqrt(q1 * q1 + q2 * q2 + q3 * q3 + q4 * q4);    // normalise quaternion
		norm = 1.0f/norm;
		q[0] = q1 * norm;
		q[1] = q2 * norm;
		q[2] = q3 * norm;
		q[3] = q4 * norm;

}
  
  
  
 // Similar to Madgwick scheme but uses proportional and integral filtering on the error between estimated reference vectors and
 // measured ones. 
void MahonyQuaternionUpdate(float ax, float ay, float az, float gx, float gy, float gz, float mx, float my, float mz)
{
	float q1 = q[0], q2 = q[1], q3 = q[2], q4 = q[3];   // short name local variable for readability
	float norm;
	float hx, hy, bx, bz;
	float vx, vy, vz, wx, wy, wz;
	float ex, ey, ez;
	float pa, pb, pc;

	// Auxiliary variables to avoid repeated arithmetic
	float q1q1 = q1 * q1;
	float q1q2 = q1 * q2;
	float q1q3 = q1 * q3;
	float q1q4 = q1 * q4;
	float q2q2 = q2 * q2;
	float q2q3 = q2 * q3;
	float q2q4 = q2 * q4;
	float q3q3 = q3 * q3;
	float q3q4 = q3 * q4;
	float q4q4 = q4 * q4;   

	// Normalise accelerometer measurement
	norm = sqrt(ax * ax + ay * ay + az * az);
	if (norm == 0.0f) return; // handle NaN
	norm = 1.0f / norm;        // use reciprocal for division
	ax *= norm;
	ay *= norm;
	az *= norm;

	// Normalise magnetometer measurement
	norm = sqrt(mx * mx + my * my + mz * mz);
	if (norm == 0.0f) return; // handle NaN
	norm = 1.0f / norm;        // use reciprocal for division
	mx *= norm;
	my *= norm;
	mz *= norm;

	// Reference direction of Earth's magnetic field
	hx = 2.0f * mx * (0.5f - q3q3 - q4q4) + 2.0f * my * (q2q3 - q1q4) + 2.0f * mz * (q2q4 + q1q3);
	hy = 2.0f * mx * (q2q3 + q1q4) + 2.0f * my * (0.5f - q2q2 - q4q4) + 2.0f * mz * (q3q4 - q1q2);
	bx = sqrt((hx * hx) + (hy * hy));
	bz = 2.0f * mx * (q2q4 - q1q3) + 2.0f * my * (q3q4 + q1q2) + 2.0f * mz * (0.5f - q2q2 - q3q3);

	// Estimated direction of gravity and magnetic field
	vx = 2.0f * (q2q4 - q1q3);
	vy = 2.0f * (q1q2 + q3q4);
	vz = q1q1 - q2q2 - q3q3 + q4q4;
	wx = 2.0f * bx * (0.5f - q3q3 - q4q4) + 2.0f * bz * (q2q4 - q1q3);
	wy = 2.0f * bx * (q2q3 - q1q4) + 2.0f * bz * (q1q2 + q3q4);
	wz = 2.0f * bx * (q1q3 + q2q4) + 2.0f * bz * (0.5f - q2q2 - q3q3);  

	// Error is cross product between estimated direction and measured direction of gravity
	ex = (ay * vz - az * vy) + (my * wz - mz * wy);
	ey = (az * vx - ax * vz) + (mz * wx - mx * wz);
	ez = (ax * vy - ay * vx) + (mx * wy - my * wx);
	if (Ki > 0.0f)
	{
			eInt[0] += ex;      // accumulate integral error
			eInt[1] += ey;
			eInt[2] += ez;
	}
	else
	{
			eInt[0] = 0.0f;     // prevent integral wind up
			eInt[1] = 0.0f;
			eInt[2] = 0.0f;
	}

	// Apply feedback terms
	gx = gx + Kp * ex + Ki * eInt[0];
	gy = gy + Kp * ey + Ki * eInt[1];
	gz = gz + Kp * ez + Ki * eInt[2];

	// Integrate rate of change of quaternion
	pa = q2;
	pb = q3;
	pc = q4;
	q1 = q1 + (-q2 * gx - q3 * gy - q4 * gz) * (0.5f * deltat);
	q2 = pa + (q1 * gx + pb * gz - pc * gy) * (0.5f * deltat);
	q3 = pb + (q1 * gy - pa * gz + pc * gx) * (0.5f * deltat);
	q4 = pc + (q1 * gz + pa * gy - pb * gx) * (0.5f * deltat);

	// Normalise quaternion
	norm = sqrt(q1 * q1 + q2 * q2 + q3 * q3 + q4 * q4);
	norm = 1.0f / norm;
	q[0] = q1 * norm;
	q[1] = q2 * norm;
	q[2] = q3 * norm;
	q[3] = q4 * norm;
}

	
//uint8_t TX_DATA[4];  	 //��ʾ�ݻ�����
//uint8_t BUF[10];       //�������ݻ�����
//char  test=0; 				 //IIC�õ�
//short T_X,T_Y,T_Z,T_T;		 //X,Y,Z�ᣬ�¶�

//void Init_MPU9250(void)
//{
///*
//   Single_Write(MPU9250_ADDRESS,PWR_M, 0x80);   //
//   Single_Write(MPU9250_ADDRESS,SMPL, 0x07);    //
//   Single_Write(MPU9250_ADDRESS,DLPF, 0x1E);    //��2000��
//   Single_Write(MPU9250_ADDRESS,INT_C, 0x00 );  //
//   Single_Write(MPU9250_ADDRESS,PWR_M, 0x00);   //
//*/
////	printf("OK");
//  Single_Write(MPU9250_ADDRESS,PWR_MGMT_1, 0x00);	//�������״̬
//	printf("OK");
//	Single_Write(MPU9250_ADDRESS,SMPLRT_DIV, 0x07);
//	Single_Write(MPU9250_ADDRESS,CONFIG, 0x06);
//	Single_Write(MPU9250_ADDRESS,GYRO_CONFIG, 0x18);
//	Single_Write(MPU9250_ADDRESS,ACCEL_CONFIG, 0x01);
//  //----------------
//	Single_Write(MPU9250_ADDRESS,0x6A,0x00);//close Master Mode	

//	
////	Single_WriteI2C(PWR_MGMT_1, 0x00);	//�������״̬
////	printf("123OK");
////	Single_WriteI2C(SMPLRT_DIV, 0x07);
////	Single_WriteI2C(CONFIG, 0x06);
////	Single_WriteI2C(GYRO_CONFIG, 0x18);
////	Single_WriteI2C(ACCEL_CONFIG, 0x01);
////  //----------------
////	Single_WriteI2C(0x6A,0x00);//close Master Mode	
//}

////******��ȡMPU9250����****************************************
//void READ_MPU9250_ACCEL(void)
//{ 

//   BUF[0]=Single_Read(MPU9250_ADDRESS,ACCEL_XOUT_L); 
//   BUF[1]=Single_Read(MPU9250_ADDRESS,ACCEL_XOUT_H);
//   T_X=	(BUF[1]<<8)|BUF[0];
//   T_X/=164; 						   //��ȡ����X������

//   BUF[2]=Single_Read(MPU9250_ADDRESS,ACCEL_YOUT_L);
//   BUF[3]=Single_Read(MPU9250_ADDRESS,ACCEL_YOUT_H);
//   T_Y=	(BUF[3]<<8)|BUF[2];
//   T_Y/=164; 						   //��ȡ����Y������
//   BUF[4]=Single_Read(MPU9250_ADDRESS,ACCEL_ZOUT_L);
//   BUF[5]=Single_Read(MPU9250_ADDRESS,ACCEL_ZOUT_H);
//   T_Z=	(BUF[5]<<8)|BUF[4];
//   T_Z/=164; 					       //��ȡ����Z������
//	  printf("X%d\t",T_X);
//	printf("Y%d\t",T_Y);
//	printf("Z%d\r\n",T_Z);
// 
//}

//void READ_MPU9250_GYRO(void)
//{ 

//   BUF[0]=Single_Read(MPU9250_ADDRESS,GYRO_XOUT_L); 
//   BUF[1]=Single_Read(MPU9250_ADDRESS,GYRO_XOUT_H);
//   T_X=	(BUF[1]<<8)|BUF[0];
//   T_X/=16.4; 						   //��ȡ����X������

//   BUF[2]=Single_Read(MPU9250_ADDRESS,GYRO_YOUT_L);
//   BUF[3]=Single_Read(MPU9250_ADDRESS,GYRO_YOUT_H);
//   T_Y=	(BUF[3]<<8)|BUF[2];
//   T_Y/=16.4; 						   //��ȡ����Y������
//   BUF[4]=Single_Read(MPU9250_ADDRESS,GYRO_ZOUT_L);
//   BUF[5]=Single_Read(MPU9250_ADDRESS,GYRO_ZOUT_H);
//   T_Z=	(BUF[5]<<8)|BUF[4];
//   T_Z/=16.4; 					       //��ȡ����Z������
// 
//	  printf("X%d\t",T_X);
//	printf("Y%d\t",T_Y);
//	printf("Z%d\r\n",T_Z);
//  // BUF[6]=Single_Read(MPU9250_ADDRESS,TEMP_OUT_L); 
//  // BUF[7]=Single_Read(MPU9250_ADDRESS,TEMP_OUT_H); 
//  // T_T=(BUF[7]<<8)|BUF[6];
//  // T_T = 35+ ((double) (T_T + 13200)) / 280;// ��ȡ������¶�
//}


//void READ_MPU9250_MAG(void)
//{ 
//   Single_Write(MPU9250_ADDRESS,0x37,0x02);//turn on Bypass Mode 
//   HAL_Delay(10);	
//   Single_Write(MAG_ADDRESS,0x0A,0x01);
//   HAL_Delay(10);	
//   BUF[0]=Single_Read (MAG_ADDRESS,MAG_XOUT_L);
//   BUF[1]=Single_Read (MAG_ADDRESS,MAG_XOUT_H);
//   T_X=(BUF[1]<<8)|BUF[0];

//   BUF[2]=Single_Read(MAG_ADDRESS,MAG_YOUT_L);
//   BUF[3]=Single_Read(MAG_ADDRESS,MAG_YOUT_H);
//   T_Y=	(BUF[3]<<8)|BUF[2];
//   						   //��ȡ����Y������
//	 
//   BUF[4]=Single_Read(MAG_ADDRESS,MAG_ZOUT_L);
//   BUF[5]=Single_Read(MAG_ADDRESS,MAG_ZOUT_H);
//   T_Z=	(BUF[5]<<8)|BUF[4];
//	  printf("X%d\t",T_X);
//	printf("Y%d\t",T_Y);
//	printf("Z%d\r\n",T_Z);
// 					       //��ȡ����Z������
//}

