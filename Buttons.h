#pragma once

#include "usb_descriptors.h"

// #define BUTTONS_DEBUG

// BUTTONS -------------------------------------------------------------
// Each of the button masks will mast the uint32_t version of the GpioBankState
#define NUM_BUTTONS 16
#define BUTTON_DEBOUNCE_DURATION 5 // The amount of time that must be waited after a button event before a new state is updated, in ms
static uint buttons[] = {6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21}; // I could make this into a function to save on memory, but I love burning memory to save time.
static uint64_t prevButtonUpdateTime[] = {0}; // used for debouncing buttons

// Initializes the buttons.
void initButtons();

// Prints the current state of the buttons to the serial monitor.
void printGpioBankState(uint16_t gpioBankState);

// Gets the value of all of the buttons used by the pinball controller and puts them in button order
// The LSB is button 1 and the MSB is button 16
// Returns true if the state has changed. False otherwise.
bool updateButtons(hid_pinball_report_t* pReport);