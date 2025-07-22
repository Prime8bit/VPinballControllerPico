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
 */

#include "ADXL345_I2C.h"

bool initAccel(i2c_inst_t *i2cPort, uint sda, uint scl)
{
    return initAccelWithRange(i2cPort, sda, scl, ADXL345_RANGE_2_G);
}

bool initAccelWithRange(i2c_inst_t *i2cPort, uint sda, uint scl, range_t range)
{
    // The ADXL345 supports 100khz standard speed and 400khz fast speed. GOTTA GO FAST!
    i2c_init(i2cPort, 400000);
    gpio_set_function(sda, GPIO_FUNC_I2C);
    gpio_set_function(scl, GPIO_FUNC_I2C);
    gpio_pull_up(sda);
    gpio_pull_up(scl);

    // Check connection
    uint8_t deviceid = getIDAccel(i2cPort);
    if (deviceid != ADXL345_DEFAULT_DEVICE_ID) {
        return false;
    }

    // Enable measurements
    writeRegisterAccel(i2cPort, ADXL345_REG_POWER_CTL, 0x08);

    return true;
}

/**************************************************************************/
/*!
    @brief  Writes one byte to the specified destination register
    @param reg The address of the register to write to
    @param value The value to set the register to
*/
/**************************************************************************/
void writeRegisterAccel(i2c_inst_t *i2cPort, uint8_t reg, uint8_t value) {
    uint8_t buffer[2] = {reg, value};
    i2c_write_blocking(i2cPort, ADXL345_ADDR, buffer, 2, false);
}

/**************************************************************************/
/*!
    @brief Reads one byte from the specified register
    @param reg The address of the register to read from
    @returns The single byte value of the requested register
*/
/**************************************************************************/
uint8_t readRegisterAccel(i2c_inst_t *i2cPort, uint8_t reg) {
    uint8_t buffer[1] = {reg};
    i2c_write_blocking(i2cPort, ADXL345_ADDR, buffer, 1, true); // true to keep master control of bus
    i2c_read_blocking(i2cPort, ADXL345_ADDR, buffer, 1, false);
    return buffer[0];
}

/**************************************************************************/
/*!
    @brief Reads two bytes from the specified register
    @param reg The address of the register to read from
    @return The two bytes read from the sensor starting at the given address
*/
/**************************************************************************/
int16_t read16Accel(i2c_inst_t *i2cPort, uint8_t reg) {
    uint8_t buffer[2] = {reg, 0};
    i2c_write_blocking(i2cPort, ADXL345_ADDR, buffer, 1, true); // true to keep master control of bus
    i2c_read_blocking(i2cPort, ADXL345_ADDR, buffer, 2, false);
    
    return ((uint16_t)buffer[1]) << 8 | ((uint16_t)buffer[0]);
}

/**************************************************************************/
/*!
    @brief  Reads the device ID (can be used to check connection)
    @return The Device ID of the connected sensor
*/
/**************************************************************************/
uint8_t getIDAccel(i2c_inst_t *i2cPort) {
    // Check device ID register
    return readRegisterAccel(i2cPort, ADXL345_REG_DEVID);
}

/**************************************************************************/
/*!
    @brief  Gets the most recent X axis value
    @return The raw `int16_t` unscaled x-axis acceleration value
*/
/**************************************************************************/
int16_t getXAccel(i2c_inst_t *i2cPort) {
    return read16Accel(i2cPort, ADXL345_REG_DATAX0);
}

/**************************************************************************/
/*!
    @brief  Gets the most recent Y axis value
    @return The raw `int16_t` unscaled y-axis acceleration value
*/
/**************************************************************************/
int16_t getYAccel(i2c_inst_t *i2cPort) {
    return read16Accel(i2cPort, ADXL345_REG_DATAY0);
}

/**************************************************************************/
/*!
    @brief  Gets the most recent Z axis value
    @return The raw `int16_t` unscaled z-axis acceleration value
*/
/**************************************************************************/
int16_t getZAccel(i2c_inst_t *i2cPort) {
    return read16Accel(i2cPort, ADXL345_REG_DATAZ0);
}

/**************************************************************************/
/*!
    @brief  Sets the g range for the accelerometer
    @param range The new `range_t` to set the accelerometer to
*/
/**************************************************************************/
void setRangeAccel(i2c_inst_t *i2cPort, range_t range) {
    /* Read the data format register to preserve bits */
    uint8_t format = readRegisterAccel(i2cPort, ADXL345_REG_DATA_FORMAT);

    /* Update the data rate */
    format &= ~0x0F;
    format |= range;

    /* Make sure that the FULL-RES bit is enabled for range scaling */
    format |= 0x08;
    /* Write the register back to the IC */
    writeRegisterAccel(i2cPort, ADXL345_REG_DATA_FORMAT, format);
}

/**************************************************************************/
/*!
    @brief  Gets the g range for the accelerometer
    @return The current `range_t` value
*/
/**************************************************************************/
range_t getRangeAccel(i2c_inst_t *i2cPort) {
    /* Read the data format register to preserve bits */
    return (range_t)(readRegisterAccel(i2cPort, ADXL345_REG_DATA_FORMAT) & 0x03);
}

/**************************************************************************/
/*!
    @brief  Sets the data rate for the ADXL345 (controls power consumption)
    @param dataRate The `dataRate_t` to set
*/
/**************************************************************************/
void setDataRateAccel(i2c_inst_t *i2cPort, dataRate_t dataRate) {
    // Note: The LOW_POWER bits are currently ignored and we always keep the device in 'normal' mode
    writeRegisterAccel(i2cPort, ADXL345_REG_BW_RATE, dataRate);
}

/**************************************************************************/
/*!
    @brief  Gets the data rate for the ADXL345 (controls power consumption)
    @return The current data rate
*/
/**************************************************************************/
dataRate_t getDataRateAccel(i2c_inst_t *i2cPort) {
    return (dataRate_t)(readRegisterAccel(i2cPort, ADXL345_REG_BW_RATE) & 0x0F);
}