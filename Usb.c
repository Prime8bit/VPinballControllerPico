#include "Helper.h"

#include "Usb.h"

// #define USB_DEBUG

void initUsb()
{
#ifdef USB_DEBUG
    printf("Initializing USB.\n");
#endif 
    // USB initialisation.
    board_init();
    // init device stack on configured roothub port    
    tud_init(BOARD_TUD_RHPORT);

    if (board_init_after_tusb) {
        board_init_after_tusb();
    }
}


bool updateUsb(hid_pinball_report_t* pReport, bool hasStateUpdated)
{
#ifdef USB_DEBUG
    printf("Begin update usb.\n");
#endif
    static uint64_t start_ms = 0;

    // tinyusb device task. At the beginning of updateUsb, according to TinyUSB examples.
    // This should happen even if there is no data to send, according to TinyUSB examples.
    tud_task(); 

    uint64_t curTime = getCurTimeMs();
    if ( curTime - start_ms < USB_POLLING_PERIOD_MS) 
    {
        return false; // not enough time
    }

    start_ms += USB_POLLING_PERIOD_MS;

#ifdef USB_DEBUG
    printf("USB Polling period has elapsed, sending data.\n");
#endif
    // Remote wakeup by pressing any button if we are in suspend mode
    // and REMOTE_WAKEUP feature is enabled by host
    if ( tud_suspended() && pReport->buttons )
    {
#ifdef USB_DEBUG
        printf("TinyUSB suspended and button pressed. Waking up the host.\n");
#endif
        tud_remote_wakeup();
    }else if (hasStateUpdated)
    {
#ifdef USB_DEBUG
        printf("Sending HID report.\n");
#endif
        if (sendHidReport(pReport))
        {
            // If the report has sent successfully, then we are ready for a new state
            return true;
        }
    }

    return false;
}

static bool sendHidReport(hid_pinball_report_t* pReport)
{
    // skip if hid is not ready yet
    if ( !tud_hid_ready() ) 
    {
#ifdef USB_DEBUG
        printf("TinyUSB HID is not ready\n");
#endif
        return false;
    }
#ifdef USB_DEBUG
    printf("TinyUSB HID is Ready\n");
#endif
    tud_hid_report(REPORT_ID_GAMEPAD, pReport, sizeof(hid_pinball_report_t));
    return true;
}

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint16_t len)
{
    // It should be noted that I tried using this as the example suggested, but even in the example code
    // this function is NEVER called. I am ignoring it for now and ignoring it does seem to work.
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
    // TODO not Implemented in Pico Example project
    (void) instance;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) reqlen;

    return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
    (void) instance;

    // The only thing the example project does in this case is to update the
    // CAPS lock LED based on the keyboard state, since I am not using 
    // keyboard functionality, this function does nothing.
}