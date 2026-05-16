#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h" // The new library for Hardware UART

// Define which UART block and pins we are using
#define UART_ID uart0
#define BAUD_RATE 115200
#define UART_TX_PIN 0
#define UART_RX_PIN 1

int main() {
    // Optional: Keep stdio init if you also want USB serial
    stdio_init_all();

    // 1. Initialize the UART hardware block at 115200 baud
    uart_init(UART_ID, BAUD_RATE);

    // 2. Configure the GPIO pins to act as UART pins
    // This connects physical pins 0 and 1 to the internal UART0 peripheral
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    // Give it a moment to stabilize
    sleep_ms(100); 

    while (1) {
        // Forward all bytes from TTL UART to USB serial
        while (uart_is_readable(UART_ID)) {
            char rx_ttl = uart_getc(UART_ID);  // store received byte first
            char tx_usb = rx_ttl;              // move to transmit variable
            putchar(tx_usb);
            fflush(stdout);
        }

        // Forward all bytes from USB serial to TTL UART
        int usb_char;
        while ((usb_char = stdio_getchar_timeout_us(0)) != PICO_ERROR_TIMEOUT) {
            char rx_usb = (char)usb_char;      // store received byte first
            char tx_ttl = rx_usb;              // move to transmit variable
            uart_putc(UART_ID, tx_ttl);
        }

        tight_loop_contents();
    }

    return 0;
}