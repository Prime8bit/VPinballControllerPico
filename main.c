#include <stdio.h>

// My custom code
#include "ADXL345_I2C.h"
#include "Buttons.h"
#include "Helper.h"
#include "Led.h"
#include "Nudge.h"
#include "Plunger.h"
#include "Usb.h"

// My custom usb descriptor
#include "usb_descriptors.h"

// STATE ---------------------------------------------------------------
// This is used for button debouncing and analog threshold enforcement.
// Once a new state has been read, it is also used to send via USB.
bool hasStateUpdated = false; // Packets are only sent over USB if a change has been detected.
hid_pinball_report_t currentState;

int main()
{
    // Init pins GP0 and GP1 for UART debugging
    stdio_init_all();

    printf("Begin Pinball Controller.\n");

    initLed();    
    initButtons();  // Look at the "buttons" array to see which GPIO's are used for buttons.   
    initPlunger();
    initNudge();
    initUsb();     

    printf("Initialization Complete.\n");

    while (true) {
        // Blink the LED if not in production mode, otherwise just light the LED
        updateLed();
        // I moved nudge to first because it takes much longer than the other tasks.
        // This ensures that we have as current of readings as possible. 
        hasStateUpdated |= updateNudge(&currentState);
        hasStateUpdated |= updateButtons(&currentState);
        hasStateUpdated |= updatePlunger(&currentState);
        // If the USB packet sends successfully, I want to reset the stateUpdated flag because
        // it is ready for new state.
        hasStateUpdated = !updateUsb(&currentState, hasStateUpdated);                
    }
}