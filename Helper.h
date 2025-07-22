#pragma once

//--------------------------------------------------------------------+
// Helper functions
//--------------------------------------------------------------------+
#include "pico/time.h"

static inline uint64_t getCurTimeMs()
{
    // For reasons I do not understand to_ms_since_boot only returns a uint32_t
    // This means if your pi pico runs for more than 49.7 days consecutively, you will get an overflow error
    // This is the uint64_t equivalent.
    return to_us_since_boot(get_absolute_time()) / 1000ULL;
}