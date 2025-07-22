#pragma once

#define LED_BLINK_DURATION 500

// Status LED
// Inits the onboard LED for alerting about the debug level
// Solid = PRODUCTION, Blinking = DEBUG
int initLed();

// Turn the led on or off
// Parameter ledOn: true = turn on, false = turn off
void setLed(bool ledOn);

// Blink the led once every LED_BLINK_DURATION milliseconds
void blinkLed();

// I want to be able to know at a glance if the device is not in production mode
// I use the onboard LED to alert me if it is not by blinking. There is no reason to waste 
// precious CPU cycles printing debug messages unless they are needed.
void updateLed();