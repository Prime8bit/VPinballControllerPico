#include <stdio.h>
#include "hardware/gpio.h"

#include "Helper.h"

#include "Buttons.h"

// I tried using IRQ's for the buttons to be more efficient, but there were two problems
// 1. Due to bouncing inputs, sometimes the IRQ would trigger with both RISE and FALL signals, making it impossible
// to know the current state from interrupts' parameters alone
// 2. Even when I used an IRQ to set a flag the triggered polling for input, by the time input polled, the bounce would
// occur and the value polled would be incorrect some of the time. The bounce would also not trigger additional events for some reason.
// In short, I resorted to polling instead of using IRQ's.

/*
void buttonCallback(uint gpio, uint32_t events) {
    // As much as I would like to print something here, it is unwise to print in your IRQ's
    newGpioBankState = gpio_get_all();
}
*/

void initButtons()
{    
#ifdef BUTTONS_DEBUG
    printf("Initializing Buttons.\n");
#endif
    for (uint i = 0; i < NUM_BUTTONS; i ++)
    {
        uint gpio = buttons[i];        
        
        gpio_init(gpio);                            // Enable the GPIO for the button
        gpio_set_dir(gpio, GPIO_IN);    // Set the GPIO to input
        gpio_pull_up(gpio);                     // Configure the input to pull up.    
    }
}

void printGpioBankState(uint16_t gpioBankState)
{
    uint16_t buttonMask = 1 << (NUM_BUTTONS - 1);

    while (buttonMask > 0)
    {
        if (buttonMask & gpioBankState)
        {
            printf("1");
        }
        else
        {
            printf("0");
        }
        buttonMask >>= 1;
    }
}

// Gets the value of all of the buttons used by the pinball controller and puts them in button order
// The LSB is button 1 and the MSB is button 16
bool updateButtons(hid_pinball_report_t* pReport)
{
    if (pReport == NULL) 
        return false;

    uint16_t result = 0;
    // Buttons have updated is set in the buttonCallback IRQ when any button is pressed or released.
    uint64_t curTime = getCurTimeMs();

    // Processing buttons in reverse order so BTN 16 is added first and then left shifting occurs
    for (int i = NUM_BUTTONS - 1; i > -1; i--) 
    {
        uint16_t buttonMask = 1 << i;
        uint64_t timeSinceLastUpdate = curTime - prevButtonUpdateTime[i];
        uint16_t newButtonState = 0;
        
        if (timeSinceLastUpdate < BUTTON_DEBOUNCE_DURATION)
        {
            newButtonState = pReport->buttons & buttonMask;
        }
        // Remember that button GPIO's are pulled high. When the GPIO is low, the button is pressed.
        else if (!gpio_get(buttons[i]))
        {
            newButtonState = buttonMask;
            prevButtonUpdateTime[i] = curTime;
        }

        result += newButtonState;
    }

#ifdef BUTTONS_DEBUG
    if (pReport->buttons != result)
    {
        printf("Prev Button Bank State: ");
        printGpioBankState(pReport->buttons);
        printf("\n");
        printf("New Button Bank State: ");
        printGpioBankState(result);
        printf("\n");
    }
#endif

    pReport->buttons = result;
    return true;
}