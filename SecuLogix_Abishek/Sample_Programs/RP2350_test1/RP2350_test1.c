// #include <stdio.h>
// #include "pico/stdlib.h"
// #include "pico/multicore.h"

// // This is the "Job" for Core 1
// void core1_entry() {
//     while (1) {
//         // Core 1 "pops" the data from the hardware FIFO mailbox.
//         // This function blocks (waits) until data arrives from Core 0.
//         uint32_t count = multicore_fifo_pop_blocking();
        
//         // Print the data to the USB Serial Monitor
//         printf("Core 1 received: %lu\n", count);
//     }
// }

// int main() {
//     // 1. Initialize all standard I/O (USB/UART)
//     stdio_init_all();

//     // 2. Launch Core 1 and tell it to run 'core1_entry'
//     multicore_launch_core1(core1_entry);

//     uint32_t count = 0;

//     // 3. The main loop for Core 0
//     while (1) {
//         count++;
        
//         // Core 0 "pushes" the count into the hardware FIFO mailbox[cite: 111].
//         multicore_fifo_push_blocking(count);
        
//         // Wait for 1 second
//         sleep_ms(1000);
//     }

//     return 0;
// }

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"      

// This is the "Job" for Core 1
void core1_entry() {
    while (1) {
        // 1. Pop the 32-bit number, then CAST it back to a String Pointer (char*)
        char* received_string = (char*)multicore_fifo_pop_blocking();
        
        // 2. Print the data to the USB Serial Monitor
        printf("Core 1 received: %s\n", received_string);
    }
}

int main() {
    stdio_init_all();
    multicore_launch_core1(core1_entry);

    // This string literal is stored in shared read-only memory
    char* my_message = "hello";

    while (1) {
        // 3. CAST the String Pointer into a 32-bit number (uint32_t) so it fits in the FIFO
        multicore_fifo_push_blocking((uint32_t)my_message);
        
        sleep_ms(1000);
    }

    return 0;
}