/**

Software License Agreement (BSD License)

Copyright (c) 2020 Kevin (KTOWN) Townsend for Adafruit Industries
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
3. Neither the name of the copyright holders nor the
names of its contributors may be used to endorse or promote products
derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**/

/*
 * First, I want to thank the folks at Adafruit. They did the heavy lifting of writing this driver. I simply adapted their code for the Pi Pico.
 * Their code can be found at https://github.com/adafruit/Adafruit_ADXL345
 * 
 * To use this driver there are some important connections that need to be made or changes will need to be made to this code.
 * Because this driver uses I2C rather than SPI, the CS pin should be set to GND.
 * Because this driver assumes that the alternate address of 0x53 will be used, the SDO pin needs to be connected to ground.
 * If you wish to use the primary address of 0x1D, then you will need to set SDO to 3v3 and you will need to update ADXL345_ADDR to 0x1D
 * 
 * sensor max value = -156.9064F | -16g = 156.9064 m/s^2 
 * sensor min value = 156.9064F | 16g = 156.9064 m/s^2
 * sensor resolution = 0.03923F | 4mg = 0.0392266 m/s^2
 */

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"

#include "hardware/i2c.h"

/*=========================================================================
I2C ADDRESS/BITS
-----------------------------------------------------------------------*/
#define ADXL345_ADDR 0x53 ///< Assumes ALT address pin low
/*=========================================================================
REGISTERS
-----------------------------------------------------------------------*/
#define ADXL345_REG_DEVID 0x00            ///< Device ID
#define ADXL345_REG_THRESH_TAP 0x1D       ///< Tap threshold
#define ADXL345_REG_OFSX 0x1E             ///< X-axis offset
#define ADXL345_REG_OFSY 0x1F             ///< Y-axis offset
#define ADXL345_REG_OFSZ 0x20             ///< Z-axis offset
#define ADXL345_REG_DUR 0x21              ///< Tap duration
#define ADXL345_REG_LATENT 0x22           ///< Tap latency
#define ADXL345_REG_WINDOW 0x23           ///< Tap window
#define ADXL345_REG_THRESH_ACT 0x24       ///< Activity threshold
#define ADXL345_REG_THRESH_INACT 0x25     ///< Inactivity threshold
#define ADXL345_REG_TIME_INACT 0x26       ///< Inactivity time
#define ADXL345_REG_ACT_INACT_CTL 0x27    ///< Axis enable control for activity and inactivity detection
#define ADXL345_REG_THRESH_FF 0x28        ///< Free-fall threshold
#define ADXL345_REG_TIME_FF 0x29          ///< Free-fall time
#define ADXL345_REG_TAP_AXES 0x2A         ///< Axis control for single/double tap
#define ADXL345_REG_ACT_TAP_STATUS 0x2B   ///< Source for single/double tap
#define ADXL345_REG_BW_RATE 0x2C          ///< Data rate and power mode control
#define ADXL345_REG_POWER_CTL 0x2D        ///< Power-saving features control
#define ADXL345_REG_INT_ENABLE 0x2E       ///< Interrupt enable control
#define ADXL345_REG_INT_MAP 0x2F          ///< Interrupt mapping control
#define ADXL345_REG_INT_SOURCE 0x30       ///< Source of interrupts
#define ADXL345_REG_DATA_FORMAT 0x31      ///< Data format control
#define ADXL345_REG_DATAX0 0x32           ///< X-axis data 0
#define ADXL345_REG_DATAX1 0x33           ///< X-axis data 1
#define ADXL345_REG_DATAY0 0x34           ///< Y-axis data 0
#define ADXL345_REG_DATAY1 0x35           ///< Y-axis data 1
#define ADXL345_REG_DATAZ0 0x36           ///< Z-axis data 0
#define ADXL345_REG_DATAZ1 0x37           ///< Z-axis data 1
#define ADXL345_REG_FIFO_CTL 0x38         ///< FIFO control
#define ADXL345_REG_FIFO_STATUS 0x39      ///< FIFO status
#define ADXL345_REG_READ 0x3B             ///< Read Register

#define ADXL345_DEFAULT_DEVICE_ID 0xE5    ///< This is the expected value of the device ID register.
/*=========================================================================*/

typedef enum {
    ADXL345_DATARATE_3200_HZ = 0b1111, ///< 1600Hz Bandwidth   140�A IDD
    ADXL345_DATARATE_1600_HZ = 0b1110, ///<  800Hz Bandwidth    90�A IDD
    ADXL345_DATARATE_800_HZ = 0b1101,  ///<  400Hz Bandwidth   140�A IDD
    ADXL345_DATARATE_400_HZ = 0b1100,  ///<  200Hz Bandwidth   140�A IDD
    ADXL345_DATARATE_200_HZ = 0b1011,  ///<  100Hz Bandwidth   140�A IDD
    ADXL345_DATARATE_100_HZ = 0b1010,  ///<   50Hz Bandwidth   140�A IDD
    ADXL345_DATARATE_50_HZ = 0b1001,   ///<   25Hz Bandwidth    90�A IDD
    ADXL345_DATARATE_25_HZ = 0b1000,   ///< 12.5Hz Bandwidth    60�A IDD
    ADXL345_DATARATE_12_5_HZ = 0b0111, ///< 6.25Hz Bandwidth    50�A IDD
    ADXL345_DATARATE_6_25HZ = 0b0110,  ///< 3.13Hz Bandwidth    45�A IDD
    ADXL345_DATARATE_3_13_HZ = 0b0101, ///< 1.56Hz Bandwidth    40�A IDD
    ADXL345_DATARATE_1_56_HZ = 0b0100, ///< 0.78Hz Bandwidth    34�A IDD
    ADXL345_DATARATE_0_78_HZ = 0b0011, ///< 0.39Hz Bandwidth    23�A IDD
    ADXL345_DATARATE_0_39_HZ = 0b0010, ///< 0.20Hz Bandwidth    23�A IDD
    ADXL345_DATARATE_0_20_HZ = 0b0001, ///< 0.10Hz Bandwidth    23�A IDD
    ADXL345_DATARATE_0_10_HZ = 0b0000  ///< 0.05Hz Bandwidth    23�A IDD (default value)
    } dataRate_t;

// Used with register 0x31 (ADXL345_REG_DATA_FORMAT) to set g range
typedef enum {
    ADXL345_RANGE_16_G = 0b11, ///< +/- 16g
    ADXL345_RANGE_8_G = 0b10,  ///< +/- 8g
    ADXL345_RANGE_4_G = 0b01,  ///< +/- 4g
    ADXL345_RANGE_2_G = 0b00   ///< +/- 2g (default value)
} range_t;

bool initAccel(i2c_inst_t *i2cPort, uint sda, uint scl);
bool initAccelWithRange(i2c_inst_t *i2cPort, uint sda, uint scl, range_t range);
void setRangeAccel(i2c_inst_t *i2cPort, range_t range);
range_t getRangeAccel(i2c_inst_t *i2cPort);
void setDataRateAccel(i2c_inst_t *i2cPort, dataRate_t dataRate);
dataRate_t getDataRateAccel(i2c_inst_t *i2cPort);

uint8_t getIDAccel(i2c_inst_t *i2cPort);
void writeRegisterAccel(i2c_inst_t *i2cPort, uint8_t reg, uint8_t value);
uint8_t readRegisterAccel(i2c_inst_t *i2cPort, uint8_t reg);
int16_t read16Accel(i2c_inst_t *i2cPort, uint8_t reg);

int16_t getXAccel(i2c_inst_t *i2cPort), getYAccel(i2c_inst_t *i2cPort), getZAccel(i2c_inst_t *i2cPort);