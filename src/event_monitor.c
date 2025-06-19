#include "gpio_hal.h"
#include "rtos_api.h"
#include <stddef.h>


volatile uint32_t gpio_counter = 0;
static  uint32_t gpio_pin_mask = 0;
uint32_t reported_counter = 0;
static volatile gpio_mask_t gpio_state = 0;
rtos_task_t reporting_task_handler;

/*Forward declarations*/
void gpio_change_callback(gpio_mask_t new_state);
void check_counter_task(void* parameter);

/* Reporting function */
void report_event_count(uint32_t count)
{
  reported_counter = count;
};

/*
	Initialize the current GPIO state by reading the input,
	register the callback and start monitoring task.
*/
void init_gpio_event_monitor(uint32_t bit_mask)
{
	rtos_mutex_lock();
	gpio_counter = 0;
	rtos_mutex_unlock();
	gpio_pin_mask = bit_mask;
	gpio_state = gpio_read_input();
	gpio_register_callback(gpio_change_callback);
	rtos_task_create(&reporting_task_handler, check_counter_task, NULL);
};

/*
 Read and reset the counter, report event count
 */
void check_counter_task(void* parameter)
{
	uint32_t local_counter_copy = 0;
	while(1)
	{
		rtos_task_delay_ms(1000);
		rtos_mutex_lock();
		local_counter_copy = gpio_counter;
		gpio_counter = 0;		
	    rtos_mutex_unlock();
		report_event_count(local_counter_copy);
	}
}

/*
	Called on GPIO change. Detect rising edges and increment a counter.
	Only count rising edges (0 → 1) on GPIO pins specified by a 32-bit bitmask.
*/
void gpio_change_callback(gpio_mask_t new_state)
{
	uint8_t change_counter = 0;
	/* Negate the current state, which will give us value 1 on pins that were low.
	   If the result of the bitwise and between negated old state and new state is 1,
	   it means that this pin had rising edge. Additionaly apply the filter for chosen
	   pins.
	*/
	uint32_t rising_egdes = ((~gpio_state) & new_state) & gpio_pin_mask;
	for(uint8_t i = 0; i < 32; i++)
	{
		if ((1U << i) & rising_egdes )
		  change_counter++;

	}
	gpio_state = new_state;
	rtos_mutex_lock();
	gpio_counter += change_counter;		
	rtos_mutex_unlock();
}
