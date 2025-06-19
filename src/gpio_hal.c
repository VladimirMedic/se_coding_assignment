#include "gpio_hal.h"
#include <stddef.h>

/*This file contains mocks implementations for GPIO HAL functions*/

void (*registered_cb)(gpio_mask_t) = NULL;
gpio_mask_t current_state = 0;

gpio_mask_t gpio_read_input(void){return current_state;}
// Registers a callback to be called on GPIO change
void gpio_register_callback(void (*callback)(gpio_mask_t new_state))
{
    registered_cb = callback;
}