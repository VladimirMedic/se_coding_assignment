#ifndef EVENT_MONITOR_H
#define EVENT_MONITOR_H

#include <stdint.h>

/*Initialize the GPIO event monitoring with desired bit mask*/
void init_gpio_event_monitor(uint32_t bit_mask);


#endif // EVENT_MONITOR_H