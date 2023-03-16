#pragma once
#include <stdint.h>

// For more information visit: https://wiki.osdev.org/8259_PIC

#define PIC1_COMMAND_PORT    0x20
#define PIC1_DATA_PORT       0x21
#define PIC2_COMMAND_PORT    0xA0
#define PIC2_DATA_PORT       0xA1
// IO Ports for Keyboard
#define KEYBOARD_DATA_PORT   0x60
#define KEYBOARD_STATUS_PORT 0x64

// RD/WR
#define PIT_DATA_PORT0   0x40
#define PIT_DATA_PORT1   0x41
#define PIT_DATA_PORT2   0x42
// WR ONLY
#define PIT_COMMAND_PORT 0x43

/* End-of-interrupt command code */
#define PIC_EOI		0x20


enum PIT_CONFIG{
    PIT_CHANNEL_SELECT_0         = (0b00  << 6),
    PIT_CHANNEL_SELECT_1         = (0b01  << 6),
    PIT_CHANNEL_SELECT_2         = (0b10  << 6),
    PIT_CHANNEL_SELECT_READ_BACK = (0b11  << 6), // (8254 only)
 
    PIT_ACCESS_MODE_LATCH        = (0b00  << 4),
    PIT_ACCESS_MODE_LOBYTE_ONLY  = (0b01  << 4),
    PIT_ACCESS_MODE_HIBYTE_ONLY  = (0b10  << 4),
    PIT_ACCESS_MODE_LO_HI_BYTE   = (0b11  << 4),

    PIT_OPERATING_MODE_0         = (0b000 << 0), //interrupt on terminal count
    PIT_OPERATING_MODE_1         = (0b001 << 0), //hardware re-triggerable one-shot
    PIT_OPERATING_MODE_2         = (0b010 << 0), //rate generator
    PIT_OPERATING_MODE_3         = (0b011 << 0), //square wave generator
    PIT_OPERATING_MODE_4         = (0b100 << 0), //software triggered strobe
    PIT_OPERATING_MODE_5         = (0b101 << 0), //hardware triggered strobe
};


void pic_send_eoi(uint8_t irq);
void setup_pic();


void pic1_mask_interrupt(uint8_t interrupt);
void pic2_mask_interrupt(uint8_t interrupt);

void pic1_unmask_interrupt(uint8_t interrupt);
void pic2_unmask_interrupt(uint8_t interrupt);