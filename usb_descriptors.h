/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

 #ifndef USB_DESCRIPTORS_H_
 #define USB_DESCRIPTORS_H_
 
 // I am only using gamepad, so I can eliminate other report ID's.
 enum
 {
    REPORT_ID_GAMEPAD = 1,
 };

 // Because I use my own HID descriptor, I must also create my own struct for containing HID packets
 // This MUST match the HID descriptor defined in desc_hid_report (see usb_descriptors.c)
typedef struct TU_ATTR_PACKED
{
    int8_t  nudgeX;     // Accelerometer X-Axis reduced to 8-bit precision for faster transmission
    int8_t  nudgeY;     // Accelerometer Y-Axis reduced to 8-bit precision for faster transmission
    int8_t  nudgeZ;     // Accelerometer Z-Axis reduced to 8-bit precision for faster transmission
    int8_t  plunger;    // Analog in reading for an analog plunger, modified to the range [-127, 127]
    uint16_t buttons;   // Buttons mask for currently pressed buttons
} hid_pinball_report_t;
 
 #endif /* USB_DESCRIPTORS_H_ */
 