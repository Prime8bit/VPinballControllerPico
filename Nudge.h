#pragma once

// #DEFINE NUDGE_DEBUG

// I2C and ADXL345 ------------------------------------------------------
#define PIN_SDA0 4
#define PIN_SCL0 5
// The ADXL345 uses 10 bit precision for nudge readings
// My USB HID descriptor uses 8-bit precision to reduce packet size
#define ACCEL_DEAD_THRESHOLD 2 // I want to filter out small incidental bumps to the machine and only focus on intentional nudges.
static bool accelInitSuccess = false; // The Nudge sensor is optional. This tracks whether a sensor is connected to I2C0.
// These are added to the raw readings of the accelerometers. Change these to suit your cabinet once everything has been mounted.
// Don't forget to account for gravity if using up/down nudge (which isn't possible in Visual Pinball at this time.)
#define ACCEL_X_OFFSET 0
#define ACCEL_Y_OFFSET 0
#define ACCEL_Z_OFFSET 0
// Set these to 1 if inversion is not needed, set them to -1 if inverting the axis is desired.
#define ACCEL_X_INVERT 1
#define ACCEL_Y_INVERT 1
#define ACCEL_Z_INVERT -1

void initNudge();

// This will take the largest reading of the two accelerometer that are plugged in.
// Any accelerometer that is not plugged defautls to 0 for all readings.
// While summing all three forces is not the most accurate way of calculating the greatest overall force
bool updateNudge(hid_pinball_report_t* pReport);

// The accelerometers return values in the range [-511, 511]
// We need to convert them to the range [-127, 127]. Because the sensitivity 
// is rather low when connected to the pinball chassis. we will just convert 
// any reading lower than -127 to -127 and any reading higher than 127 to 127
// This essentially makes the sensor 4x as sensitive as it originally was.
static int8_t normalizeNudgeReading(int16_t accelReading, int16_t invert, int16_t offset);