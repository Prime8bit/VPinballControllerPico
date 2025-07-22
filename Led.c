#include "hardware/gpio.h"

#include "Helper.h"

#include "Led.h"

#define LED_DEBUG defined(PINBALL_DEBUG) || defined(NUDGE_DEBUG) || defined(PLUNGER_DEBUG) || defined(NUDGE_DEBUG)

static bool ledState = false; // True if the led is currently lit, false otherwise

//--------------------------------------------------------------------+
// Status LED
// Inits the onboard LED for alerting about the debug level
// Solid = PRODUCTION, Blinking = DEBUG
//--------------------------------------------------------------------+
int initLed() {
#ifdef LED_DEBUG
    printf("Initializing DEBUG indicator LED.\n");
#endif
#if defined(PICO_DEFAULT_LED_PIN)
    // A device like Pico that uses a GPIO for the LED will define PICO_DEFAULT_LED_PIN
    // so we can use normal GPIO functionality to turn the led on and off
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    return PICO_OK;
#elif defined(CYW43_WL_GPIO_LED_PIN)
    // For Pico W devices we need to initialise the driver etc
    return cyw43_arch_init();
#endif
}

// Turn the led on or off
void setLed(bool ledOn) {
    if (ledOn == ledState)
        return;

#if defined(PICO_DEFAULT_LED_PIN)
    // Just set the GPIO on or off
    gpio_put(PICO_DEFAULT_LED_PIN, ledOn);
#elif defined(CYW43_WL_GPIO_LED_PIN)
    // Ask the wifi "driver" to set the GPIO on or off
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);
#endif

        ledState = ledOn;
}

void blinkLed()
{
    static uint64_t prevTimeMs = 0;

    uint64_t curTimeMs = getCurTimeMs();
    uint64_t testLong = 15ull;

    // Blink every LED_BLINK_DURATION ms
    if ( curTimeMs - prevTimeMs < LED_BLINK_DURATION) 
    {
        return; // not enough time
    }

    prevTimeMs = curTimeMs;

    setLed(!ledState);
}

// I want to be able to know at a glance if the device is not in production mode
// I use the onboard LED to alert me if it is not by blinking. There is no reason to waste 
// precious CPU cycles printing debug messages unless they are needed.
void updateLed()
{
#ifdef LED_DEBUG
        blinkLed();
#else                
        setLed(true);
#endif
}