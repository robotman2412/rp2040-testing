
#include <stddef.h>

typedef uint64_t absolute_time_t;

// Get current time in microseconds. [custom]
uint64_t micros();

// Get current time in milliseconds. [custom]
uint32_t millis();

// Sleep until given timestamp in microseconds has passed. [pico sdk]
void sleep_until(absolute_time_t target);

// Sleep for a given amount of microseconds. [pico sdk]
void sleep_us(uint64_t us);

// Sleep for a given amount of milliseconds. [pico sdk]
void sleep_ms(uint32_t ms);
