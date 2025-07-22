#include <stdio.h>
#include <stdlib.h>
#include "pico/binary_info.h"

#include "ADXL345_I2C.h"
#include "usb_descriptors.h"

#include "Nudge.h"

//--------------------------------------------------------------------+
// Nudge - ADXL345 Accelerometer sensor
//--------------------------------------------------------------------+

void initNudge()
{
#ifdef NUDGE_DEBUG
    printf("Initializing Accelerometer.\n");
#endif  
    // Accelerometer sensor on I2C0 -----------------------------------------------------------------
    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(PIN_SDA0, PIN_SCL0, GPIO_FUNC_I2C));
    accelInitSuccess = initAccel(i2c0, PIN_SDA0, PIN_SCL0);
#ifdef NUDGE_DEBUG
    if (accelInitSuccess)
    {
        printf("Successfully initialized accelerometer on i2c0.\n");
    }
    else
    {
        printf("Failed to initialize accelerometer on i2c0.\n");
    }
#endif
}

// This will take the largest reading of the two accelerometer that are plugged in.
// Any accelerometer that is not plugged defautls to 0 for all readings.
// While summing all three forces is not the most accurate way of calculating the greatest overall force
bool updateNudge(hid_pinball_report_t* pReport)
{
#ifdef NUDGE_DEBUG
    printf("Updating accelerometers.\n");
#endif
    int8_t newX, newY, newZ = 0;

    if (accelInitSuccess)
    {
        newX = normalizeNudgeReading(getXAccel(i2c0), ACCEL_X_INVERT, ACCEL_X_OFFSET);
        newY = normalizeNudgeReading(getYAccel(i2c0), ACCEL_Y_INVERT, ACCEL_Y_OFFSET);
        newZ = normalizeNudgeReading(getZAccel(i2c0), ACCEL_Z_INVERT, ACCEL_Z_OFFSET);
#ifdef NUDGE_DEBUG
        printf ("Read values from the accelerometer on I2C0: (%d, %d, %d)\n", newX, newY, newZ);
#endif
    }
    
    if (abs(pReport->nudgeX - newX) > ACCEL_DEAD_THRESHOLD ||
        abs(pReport->nudgeY - newY) > ACCEL_DEAD_THRESHOLD ||
        abs(pReport->nudgeZ - newZ) > ACCEL_DEAD_THRESHOLD
    )
    {
        pReport->nudgeX = newX;
        pReport->nudgeY = newY;
        pReport->nudgeZ = newZ;
        return true;
    }

    return false;
}

// The accelerometer returns a value in the range [-511, 511]
// We need to convert it to the range [-127, 127]. Because the sensitivity of the ADXL345
// is rather low when connected to the pinball chassis. we will just convert 
// any reading lower than -127 to -127 and any reading higher than 127 to 127
// This essentially makes the sensor 4x as sensitive as it originally was.
// In my testing I found this to produce the desired result in VisualPinball best.
static int8_t normalizeNudgeReading(int16_t accelReading, int16_t invert, int16_t offset)
{    
    // The offset should NOT be outside the range [-384, 384]
    if (offset > 384 || offset < -384)
    {
        return 0;
    }

    // We don't want to cause overflow errors when inverting.
    if (accelReading == -32768)
    {
        accelReading = -32767;
    }
    accelReading = accelReading * invert;

    // More readable code would do accelReading + offset > 127, but this has the potential for an overflow
    // Rather than cast to int32_t, we just subtract offset from both sides.
    if (accelReading > 127 - offset)
        return 127;
    if (accelReading < -127 - offset)
        return -127;
        
    // because of the if statements above, we know this will not overflow.
    return (int8_t)(accelReading + offset);
}