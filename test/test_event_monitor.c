#include "event_monitor.h"
#include "gpio_hal.h"
#include "rtos_api.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>


extern void (*registered_cb)(gpio_mask_t);
extern uint32_t gpio_counter;
extern uint32_t reported_counter;
extern void report_event_count(uint32_t count);

// Helper to trigger the interrupt
void trigger_gpio_isr(gpio_mask_t new_state) {
    if (registered_cb) {
        registered_cb(new_state);
    }
}

/*
 Read and reset the counter, report event count. Test version without eternal loop.
 */
void simulate_check_counter_task(void* parameter)
{
	uint32_t local_counter_copy = 0;
	//while(1)
	{
		rtos_task_delay_ms(1000);
		rtos_mutex_lock();
		local_counter_copy = gpio_counter;
		gpio_counter = 0;		
	    rtos_mutex_unlock();
		report_event_count(local_counter_copy);
	}
	
}

// --- Test Cases ---
/*Only 1 pin has rising edge, so result should be 1*/
void test_single_rising_edge() {
    
    init_gpio_event_monitor(0xFFFFFFFF);  // Monitor all gpio's

    trigger_gpio_isr(0x01);  // GPIO0: 0 -> 1
    assert(gpio_counter == 1);

    /*Reset the counter and report count*/
    simulate_check_counter_task(NULL);
    assert(reported_counter == 1);
    printf("test_single_rising_edge passed.\n");
}

/*Only 1 pin has rising edge, one is falling, so result should be 1*/
void test_single_falling_edge() {
    
    init_gpio_event_monitor(0xFFFFFFFF);  // Monitor all gpio's

    trigger_gpio_isr(0x01);  // GPIO0: 0 -> 1
    assert(gpio_counter == 1);
    trigger_gpio_isr(0x00);  // GPIO0: 1 -> 0
    assert(gpio_counter == 1);
   
    /*Reset the counter and report count*/
    simulate_check_counter_task(NULL);
    assert(reported_counter == 1);
    printf("test_single_falling_edge passed.\n");
}

/*Four pins have rising edges, so result should be 4*/
void test_multiple_rising_edges() {
    
    init_gpio_event_monitor(0xFFFFFFFF);  // Monitor all gpio's

    trigger_gpio_isr(0x01010101);  // GPIO0,8,16,24: 0 -> 1
    assert(gpio_counter == 4);

    /*Reset the counter and report count*/
    simulate_check_counter_task(NULL);
    assert(reported_counter == 4);
    printf("test_multiple_rising_edges passed.\n");
}

void test_multiple_rising_edges_repeated() {
    
    init_gpio_event_monitor(0xFFFFFFFF);  // Monitor all gpio's

    trigger_gpio_isr(0x01010101);  // GPIO0,8,16,24: 0 -> 1
    assert(gpio_counter == 4);
    trigger_gpio_isr(0x01010101);  // GPIO0,8,16,24: 0 -> 1, should ne no change
    assert(gpio_counter == 4);

    /*Reset the counter and report count*/
    simulate_check_counter_task(NULL);
    assert(reported_counter == 4);
    printf("test_multiple_rising_edges_repeated passed.\n");
}

void test_multiple_rising_edges_sequential() {
    
    init_gpio_event_monitor(0xFFFFFFFF);  // Monitor all gpio's

    trigger_gpio_isr(0x01010101);  // GPIO0,8,16,24: 0 -> 1
    assert(gpio_counter == 4);
    trigger_gpio_isr(0x02020202);  // GPIO0,8,16,24: 1 -> 0 GPIO1,9,17,25: 0 -> 1
    assert(gpio_counter == 8);

    /*Reset the counter and report count*/
    simulate_check_counter_task(NULL);
    assert(reported_counter == 8);
    printf("test_multiple_rising_edges_sequential passed.\n");
}

void test_multiple_rising_edges_with_mask() {
    
    init_gpio_event_monitor(0x0000FFFF);  // Monitor lower part of gpio

    trigger_gpio_isr(0x01010101);  // GPIO0,8,16,24: 0 -> 1, but we are monitoring only lower part
    assert(gpio_counter == 2);

    /*Reset the counter and report count*/
    simulate_check_counter_task(NULL);
    assert(reported_counter == 2);
    printf("test_multiple_rising_edges_with_mask passed.\n");
}

void test_monitor_single_pin() {
    
    init_gpio_event_monitor(0x00000008);  // Monitor only GPIO3

    trigger_gpio_isr(0xFF);  // GPIO0-7: 0 -> 1
    assert(gpio_counter == 1);

    /*Reset the counter and report count*/
    simulate_check_counter_task(NULL);
    assert(reported_counter == 1);
    printf("test_monitor_single_pin passed.\n");
}

int main() {
    test_single_rising_edge();
    test_single_falling_edge();
    test_multiple_rising_edges();
    test_multiple_rising_edges_with_mask();
    test_multiple_rising_edges_repeated();
    test_multiple_rising_edges_sequential();
    test_monitor_single_pin();
    printf("All tests passed.\n");
    return 0;
}