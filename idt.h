#pragma once
#include <stdint.h>

typedef enum {
   IDT_FLAG_GATE_TASK       = 0x5,
   IDT_FLAG_GATE_16BIT_INT  = 0x6,
   IDT_FLAG_GATE_16BIT_TRAP = 0x7,
   IDT_FLAG_GATE_32BIT_INT  = 0xE, // Normal interrupt
   IDT_FLAG_GATE_32BIT_TRAP = 0xF, // Used when the instruction that triggered should be restarted

   IDT_FLAG_RING0           = (0<<5),
   IDT_FLAG_RING1           = (1<<5),
   IDT_FLAG_RING2           = (2<<5),
   IDT_FLAG_RING3           = (3<<5),

   IDT_FLAG_PRESENT         = 0x80,
} IDT_FLAGS;


bool setIdtEntry32(uint32_t interrupt, void* base, uint16_t segmentDescriptor, uint8_t flags);

void enableGate32(int interrupt);

void disableGate32(int interrupt);

void load_idt32();