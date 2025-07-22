#pragma once

// USB headers provided by tinyUSB
#include "bsp/board_api.h"
#include "tusb.h"

#include "usb_descriptors.h"

#define USB_POLLING_PERIOD_MS 5 // USB packets will be sent no fewer than this many milliseconds apart. 

void initUsb();

bool updateUsb(hid_pinball_report_t* pReport, bool hasStateUpdated);

static bool sendHidReport(hid_pinball_report_t* pReport);

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
// Returns true if the USB packet sent successfully
// Returns false otherwise.
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint16_t len);

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen);

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize);