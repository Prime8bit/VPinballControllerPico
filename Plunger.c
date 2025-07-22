#include <stdlib.h>
#include <stdio.h>
#include "hardware/adc.h"

#include "usb_descriptors.h"

#include "Plunger.h"

void initPlunger()
{
#ifdef PLUNGER_DEBUG
    printf("Initializing Analog Plunger Input.\n");
#endif    
    adc_init();
    adc_gpio_init(PIN_ANALOG);
    adc_select_input(ADC_NUM_ANALOG); // I am only using one analog input. If you want to use more than one, then this will need to move appropriately.
}

bool updatePlunger(hid_pinball_report_t* pReport)
{
    // The ADC returns an unsigned 12-bit int in range [0, 4095] 
    // I need to convert this to a signed 8-bit int in range [-127, 127]
    
    // I am surprised the compiler does this implicit conversion correctly.
    // I trust it is more efficient at casting than I would be had I done it manually.
    int8_t newPlungerState = (int8_t)(adc_read() >> 4) - 128;
    
    if (abs(pReport->plunger - newPlungerState) >= ANALOG_DEAD_THRESHOLD)
    {
#ifdef PLUNGER_DEBUG
        printf("Prev Plunger value: %d\n", pReport->plunger);
        printf("New Plunger value: %d\n", newPlungerState);
#endif
        pReport->plunger = newPlungerState;
        return true;
    }

    return false;
}