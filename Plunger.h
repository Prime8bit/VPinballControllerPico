#pragma once

// #define PLUNGER_DEBUG

// ANALOG PLUNGER -------------------------------------------------------
#define PIN_ANALOG 28
#define ADC_NUM_ANALOG 2
#define ANALOG_DEAD_THRESHOLD 5 // I need to filter out ADC noise from the plunger to keep it from constantly moving. This will keep the plunger from registering movement unless there is significant movement. 

// Initializes the plunger.
void initPlunger();

// Updates the usb report with the current plunger state.
bool updatePlunger(hid_pinball_report_t* pReport);