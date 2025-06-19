# Event Monitor Module

## Overview

This project implementing a firmware module that counts GPIO input events triggered by an interrupt, 
filters them using bit manipulation (rising edges), and communicates the count to a task running under an RTOS.

* Detects rising edges on specified GPIO pins.
* Counts events in an thread-safe way.
* Periodically reports the event count from an RTOS task.

## Files

* `event_monitor.h/c`: Event monitoring implementation.
* `test_event_monitor.c`: Unit tests for edge detection and event filtering.
* `gpio_hal.h/c` : Header file and dummy implementations of custom hal api.
* `rtos_api.h/c` : Header file and dummy implementations of rtos api.
* `Makefile`: build file if using make.
* `README.md`: This document.

## Design Details

### Rising Edge Detection

Rising edges are detected by comparing the previous GPIO state with the current one. 
The transition `0 -> 1` is identified using bit manipulation:

```c
rising_edges = (~gpio_state & new_state) & gpio_pin_mask;
```

Explanation:

* `~gpio_state negates the current state, which will give us value 1 on pins that were low.
* `~gpio_state & new_state` identifies bits that transitioned from 0 to 1.
* `& gpio_pin_mask` ensures only configured pins are counted.

Example: only bit 0 was high in previous state, so gpio_state is 0x00000001. Negating it gives us 0xFFFFFFFE.
If the new state is 0x00000003 ~gpio_state & new_state wil give us result 0x00000002.
This means that GPIO1 has a rising edge and counter should be increased by 1.
If the given mask is including GPIO1 counter will be increased by 1.


### Interrupt Safety

* The event counter is shared between the interrupt handler and the RTOS task.
* Access to the counter is guarded by `rtos_mutex_lock()` and `rtos_mutex_unlock()`.

### RTOS Task

* A periodic task runs every 1000 ms.
* It reads and resets the event count.
* The count is passed to `report_event_count(uint32_t count)`.

### Stubbed Function

```c
void report_event_count(uint32_t count);
```

The stubbed implementation of this function transmiting the counter to global variable which can be used in unit tests.

## Build Instructions

### Build

The project is created in Microsoft Visual Studio Code using command prompt and gcc compiler.
Compile with:

gcc -Iinclude src/event_monitor.c src/gpio_hal.c src/rtos_api.c test/test_event_monitor.c -o test_event_monitor.exe

or alternatively use make command with provided Makefile. If some other terminal is used, clean command should be changed accordingly.
### Running

```sh
./test_event_monitor.exe
```

## Testing

* Edge detection and filtering are tested in isolation.
* Simulated GPIO changes validate correct counting logic.

## Notes

* This implementation is designed for a cooperative RTOS and uses mutexes to protect shared data.
* `event_monitor_init()` must be called during initialization.
* GPIO callbacks and state transitions are simulated for testing.

## Limitations

* Actual GPIO ISR timing and latency are not simulated in tests.
* Real hardware timing may vary and should be validated on target.

---

**Author:** Vladimir Medic
**Date:** 19.06.2025.
