#include <stdio.h>
#include <inttypes.h>
#include "pico/stdlib.h"

#define WG_D0_PIN 2
#define WG_D1_PIN 3

static inline void wiegand_idle(void) {
    gpio_put(WG_D0_PIN, 1);
    gpio_put(WG_D1_PIN, 1);
}

static void wiegand_send_bit(bool bit) {
    if (bit) {
        gpio_put(WG_D1_PIN, 0);
        sleep_us(50);   // pulse width: 50µs (standard Wiegand minimum)
        gpio_put(WG_D1_PIN, 1);
    } else {
        gpio_put(WG_D0_PIN, 0);
        sleep_us(50);   // pulse width: 50µs
        gpio_put(WG_D0_PIN, 1);
    }
    sleep_us(2000);     // inter-bit gap: 2ms (standard Wiegand)
}

static void wiegand_send_u32(uint32_t value) {
    for (int i = 31; i >= 0; i--) {
        bool bit = (value >> i) & 1u;
        wiegand_send_bit(bit);
    }
}

int main() {
    stdio_init_all();

    gpio_init(WG_D0_PIN);
    gpio_init(WG_D1_PIN);
    gpio_set_dir(WG_D0_PIN, GPIO_OUT);
    gpio_set_dir(WG_D1_PIN, GPIO_OUT);
    wiegand_idle();

    uint32_t value = 1;

    while (true) {
        printf("Sending value = %" PRIu32 " 0x%08" PRIX32 "\n", value, value);
        wiegand_send_u32(value);
        wiegand_idle();

        value += 1;
        if (value > 500) {
            value = 1;
        }

        sleep_ms(100);
    }
}
