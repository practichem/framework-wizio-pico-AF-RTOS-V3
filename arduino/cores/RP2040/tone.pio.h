// -------------------------------------------------- //
// This file is autogenerated by pioasm; do not edit! //
// -------------------------------------------------- //

#if !PICO_NO_HARDWARE
#include "hardware/pio.h"
#endif

// ---- //
// tone //
// ---- //

#define tone_wrap_target 0
#define tone_wrap 7

static const uint16_t tone_program_instructions[] = {
    0x80a0, //  0: pull   block
    0xa027, //  1: mov    x, osr
    0xb846, //  2: mov    y, isr          side 1
    0x0083, //  3: jmp    y--, 3
    0x0045, //  4: jmp    x--, 5
    0xb046, //  5: mov    y, isr          side 0
    0x0086, //  6: jmp    y--, 6
    0x0042, //  7: jmp    x--, 2
};

#if !PICO_NO_HARDWARE
static const struct pio_program tone_program = {
    .instructions = tone_program_instructions,
    .length = 8,
    .origin = -1,
};

static inline pio_sm_config tone_program_get_default_config(uint offset)
{
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + tone_wrap_target, offset + tone_wrap);
    sm_config_set_sideset(&c, 2, true, false);
    return c;
}

static inline void tone_program_init(PIO pio, uint sm, uint offset, uint pin)
{
    pio_gpio_init(pio, pin);
    pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, true);
    pio_sm_config c = tone_program_get_default_config(offset);
    sm_config_set_sideset_pins(&c, pin);
    pio_sm_init(pio, sm, offset, &c);
}

#endif
