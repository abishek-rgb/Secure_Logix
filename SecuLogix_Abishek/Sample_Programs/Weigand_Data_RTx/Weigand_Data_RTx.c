
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/irq.h"

/* ============================================================================
 * CONFIGURATION - User may modify these based on actual pin connections
 * ============================================================================ */

#define D0_PIN          14      // GPIO pin for Wiegand D0 line (RP2350-Zero)
#define D1_PIN          15      // GPIO pin for Wiegand D1 line (RP2350-Zero)
#define MAX_BITS        64      // Maximum expected bit length (26 or 34 typical)
#define TIMEOUT_US      50000   // 50ms timeout in microseconds

/* ============================================================================
 * GLOBAL STATE - Wiegand bit capture buffer and control variables
 * ============================================================================ */

volatile uint8_t wiegand_bits[MAX_BITS];        // Raw bit array (0 or 1)
volatile uint32_t bit_count = 0;                // Number of bits captured
volatile bool transmission_complete = false;    // Flag indicating frame complete
volatile alarm_id_t timeout_alarm_id = -1;      // ID of timeout alarm

/* ============================================================================
 * TIMEOUT CALLBACK - Called after 50ms of no activity
 * ============================================================================
 * This function triggers when the timeout expires, indicating the end of a
 * Wiegand transmission. It sets a flag that the main loop checks.
 */
static int64_t wiegand_timeout_callback(alarm_id_t id, void *user_data) {
    (void)user_data;  // Unused parameter

    if (bit_count > 0) {
        transmission_complete = true;
    }
    return 0;  // Do not reschedule
}

static void reset_timeout(void) {
    // Cancel existing alarm if it's pending
    if (timeout_alarm_id != -1) {
        cancel_alarm(timeout_alarm_id);
    }

    // Schedule new timeout 50ms from now
    timeout_alarm_id = add_alarm_in_us(TIMEOUT_US, wiegand_timeout_callback, NULL, false);
}

static void wiegand_gpio_callback(uint gpio, uint32_t events) {
    if (bit_count >= MAX_BITS || transmission_complete) {
        return;
    }

    if (!(events & GPIO_IRQ_EDGE_FALL)) {
        return;
    }

    if (gpio == D0_PIN) {
        wiegand_bits[bit_count] = 0;  // D0 pulse = bit 0
    } else if (gpio == D1_PIN) {
        wiegand_bits[bit_count] = 1;  // D1 pulse = bit 1
    } else {
        return;  // Unknown GPIO, ignore
    }

    bit_count++;

    // Reset the 50ms timeout timer
    reset_timeout();
}

static void init_wiegand_gpio(void) {
    // Initialize D0 as input
    gpio_init(D0_PIN);
    gpio_set_dir(D0_PIN, GPIO_IN);
    // gpio_pull_up(D0_PIN);  // REMOVE THIS LINE - no pull-up needed

    // Initialize D1 as input
    gpio_init(D1_PIN);
    gpio_set_dir(D1_PIN, GPIO_IN);
    // gpio_pull_up(D1_PIN);  // REMOVE THIS LINE - no pull-up needed

    // Register the same callback for both GPIO pins
    // This callback will be invoked on GPIO_IRQ_EDGE_FALL for both pins
    gpio_set_irq_enabled_with_callback(D0_PIN, GPIO_IRQ_EDGE_FALL, true,
                                       &wiegand_gpio_callback);
    gpio_set_irq_enabled(D1_PIN, GPIO_IRQ_EDGE_FALL, true);
}

static void print_bits_binary(void) {
    printf("Binary: ");
    for (uint32_t i = 0; i < bit_count; i++) {
        printf("%d", wiegand_bits[i]);
    }
    printf("\n");
}

static void print_bits_hexadecimal(void) {
    printf("Hexadecimal: 0x");

    for (uint32_t i = 0; i < bit_count; i += 4) {
        uint8_t nibble = 0;
        uint32_t remaining_bits = bit_count - i;
        uint32_t bits_to_process = (remaining_bits >= 4) ? 4 : remaining_bits;

        // Pack up to 4 bits into a nibble
        for (uint32_t j = 0; j < bits_to_process; j++) {
            nibble = (nibble << 1) | wiegand_bits[i + j];
        }

        // If fewer than 4 bits, shift left to align
        if (bits_to_process < 4) {
            nibble <<= (4 - bits_to_process);
        }

        printf("%X", nibble);
    }
    printf("\n");
}


static void print_26bit_interpretation(void) {
    if (bit_count != 26) {
        return;  // Not a 26-bit format
    }

    // Extract facility code (bits 1-8)
    uint16_t facility_code = 0;
    for (int i = 1; i <= 8; i++) {
        facility_code = (facility_code << 1) | wiegand_bits[i];
    }

    // Extract card number (bits 9-24)
    uint32_t card_number = 0;
    for (int i = 9; i <= 24; i++) {
        card_number = (card_number << 1) | wiegand_bits[i];
    }

    printf("26-bit Wiegand: Facility=%d, Card Number=%d\n", facility_code, card_number);
}

int main(void) {
    // Initialize USB serial communication for printf
    stdio_init_all();

    // Wait a moment for USB connection to stabilize
    sleep_ms(500);

    printf("Wiegand RFID Reader Ready\n");

    // Initialize Wiegand GPIO pins and interrupts
    init_wiegand_gpio();

    // Main event loop
    while (true) {
        // Check if a complete transmission has been received
        if (transmission_complete) {
            printf("\n--- RFID Tag Detected ---\n");
            printf("Bits: %d\n", bit_count);

            // Output the captured data in multiple formats
            print_bits_binary();
            print_bits_hexadecimal();
            print_26bit_interpretation();

            // Reset for next transmission
            transmission_complete = false;
            bit_count = 0;
            timeout_alarm_id = -1;
        }

        // Small delay to prevent tight loop
        sleep_ms(1);
    }

    return 0;
}
