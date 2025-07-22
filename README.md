# Pi Pico Virtual Pinball Controller
I created this project because while I love pinball and have a virtual pinball table at home. Most virtual pinball forums are still talking about using the NXP FRDM-KL25Z for the controller and it is a fine board, but I already had some pi picos lying around and I figured I could save some cash by using them. 

A solderable breadboard - This is recommended because the buttons all require a connection to a GPIO pin on one side and a connection to ground on the other. Using a solderable breadboard means I can connect the ground rails on both sides of the breadboard to a ground pin and use its numerous solderable points for ground connections with the buttons. 
An accelerometer board - The FRDM-KL25Z has a built in accelerometer but the pi pico does not. While I am certain that some custom RP2040/RP2350 boards may have an accelerometer built in, they probably cost as much as the FRDM-KL25Z. I looked on Amazon and found an ADXL345 breakout board for $7 for two. 

Everything else you need would still be the same for both the FRDM-KL25Z and the Pi Pico. You need some arcade style buttons, although leaf switch buttons are the most like real pinball machines. You can optionally use a virtual analog plunger. I didn't want to buy one for this project so I tested the analog code using 10k ohm analog stick for gamepads that I had lying around. It is possible that this code may still require some tweaking. If you don't want to spend big bucks on a virtual analog plunger, then just use a button for plunge.

## Using the Pinball Controller
When wiring up the project, the easiest way to tell what to do is by looking at Schematic.PDF. For the ADXL345 board to work with the included driver, the following connections must be made:
 * Because this driver uses I2C rather than SPI, the CS pin should be set to GND.
 * Because this driver assumes that the alternate address of 0x53 will be used, the SDO pin needs to be connected to ground.
 * If you wish to use the primary address of 0x1D, then you will need to wire SDO to 3v3 and you will need to update ADXL345_ADDR to 0x1D in ADXL345_I2C.h.

The code for this project creates a standard USB HID gamepad from a Pi Pico that will work on Windows, Mac OS, and Linux. If you want to see that it is working appropriately in Windows, I suggest you connect the various buttons, plunger, and/or nudge sensor and open the Windows Game Controllers test utility. Nudging the controller acts like an analog stick with values in the range [-127, 127] on the X, Y, and Z axes. The plunger is read from the Rx axis. The buttons are just buttons and this code supports up to 16 of them. See the pin assignments section below for details about which pins correspond to which buttons.

## Pin assignments
I used the following pin assignments for this project. When choosing between SPI and I2C for the sensors I went with I2C for two reasons. First was that the adafruit library for the sensor, upon which my code is heavily based, uses I2C exclusively. Second was so there would be fewer wires to manage for the sensor. Using SPI will theoretically allow you to read accelerometer values faster, but will require two additional wires and rewriting the driver for SPI communication. 

GP0 - UART0 TX - For debugging.
GP1 - UART0 RX - For debugging.
GP2 - NC - Left unconnected in case you want to use SPI instead of I2C for nudge sensors. I chose I2C so there would be fewer wires to manage.
GP3 - NC - Left unconnected in case you want to use SPI instead of I2C for nudge sensors. I chose I2C so there would be fewer wires to manage.
GP4 - I2C0_SDA - For nudge sensor 0.
GP5 - I2C0_SCL - For nudge sensor 0.
GP6 - BTN 1
GP7 - BTN 2
GP8 - BTN 3
GP9 - BTN 4
GP10 - NC - I had originally designated this for a second nudge sensor, but found that trying to use two sensors to be more hassle than it was worth.
GP11 - NC - I had originally designated this for a second nudge sensor, but found that trying to use two sensors to be more hassle than it was worth.
GP12 - NC - I had originally designated this for a second nudge sensor, but found that trying to use two sensors to be more hassle than it was worth.
GP13 - BTN 16 - Made this a higher button number in case someone wanted to use this for a second SPI bus by sacrificing one button. 
GP14 - BTN 5
GP15 - BTN 6
GP16 - BTN 7
GP17 - BTN 8
GP18 - BTN 9
GP19 - BTN 10
GP20 - BTN 11
GP21 - BTN 12
GP22 - BTN 13
GP26 - BTN 14 - I recommend against using this for buttons if you can avoid it. For some of my analog tests, I discovered that analog inputs on pin 28 would cause false positives on this pin with some potentiometers, but not others.
GP27 - BTN 15 - I recommend against using this for buttons if you can avoid it. For some of my analog tests, I discovered that analog inputs on pin 28 would cause false positives on this pin with some potentiometers, but not others.
GP28 - Analog In - Plunger

## A note about the nudge sensor
The ADXL345 sensor has a range of [-511, 511], but I wanted to be efficient with my USB backets so I reduce this range to [-127, 127]. Rather than doing a division conversion, I simply read the value as is and if it is outsizde the [-127, 127] range then I truncate it. This effectively multiplies the reading 4x. This is intentional. I found that when I divided the reading by 4, the reading was too insensitive on my pinball cabinet. Truncating the reading gave me values I liked better. Feel free to adjust the Nudge.c code if you prefer another strategy.

## Debugging the pinball controller
Since the controller itself uses USB for communicating the the host PC, I opted to use the UART pins for standard output print statements. Unfortunately, this means you will need a UART to USB adapter, pico probe, or second raspberry pi pico to debug this. This was my first Pi Pico project and I was unaware of this limitation when I started, but I prefer to use a hardware debugger anyway so I found it an acceptable compromise. For those who want to save as much money as possible while working on projects like this, using a board with a built in hardware debugger might be best. I hear the ESP32 boards are like this, but haven't played around with one yet to verify.

All debug statements are inside ifdef blocks that correspond to a custom defined flag for debugging each part of the pinball controller, i.e. NUDGE_DEBUG for debugging the nudge sensor. I have added some code that will blink the onboard led if any of the debug flags are defined. This is for the sake of efficiency. If you see the LED flashing, then it means you still have one of the debug flags defined. It is recommended that you undefine these for regular use, but isn't necessary. When none of the debug flags are defined, the LED should display solid green. 