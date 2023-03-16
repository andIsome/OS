#include "idt.h"

#define PACKED __attribute__((packed))
#define NUM_INTERRUPTS 256

typedef struct {
   uint16_t offset_1;        // offset bits 0..15
   uint16_t selector;        // a code segment selector in GDT or LDT
   uint8_t  zero;            // unused, set to 0
   uint8_t  flags; // gate type, dpl, and p fields
   uint16_t offset_2;        // offset bits 16..31
} PACKED interrupt_descriptor32;

typedef struct {
    uint16_t limit;
    interrupt_descriptor32* ptr;
} PACKED idt_record32;

typedef struct {
   uint16_t offset_1;        // offset bits 0..15
   uint16_t selector;        // a code segment selector in GDT or LDT
   uint8_t  ist;             // bits 0..2 holds Interrupt Stack Table offset, rest of bits zero.
   uint8_t  flags; // gate type, dpl, and p fields
   uint16_t offset_2;        // offset bits 16..31
   uint32_t offset_3;        // offset bits 32..63
   uint32_t zero;            // reserved
} PACKED interrupt_descriptor64;



static_assert(sizeof(interrupt_descriptor32) == 8);
static_assert(sizeof(interrupt_descriptor64) == 16);

interrupt_descriptor32   g_idt32[NUM_INTERRUPTS];
//interrupt_descriptor64 g_idt64[NUM_INTERRUPTS];

idt_record32 g_idt_record32 = { sizeof(g_idt32) - 1, g_idt32};

void load_idt32(){
    __asm__ ("lidt %0" :: "m"(g_idt_record32));
}


bool setIdtEntry32(uint32_t interrupt, void* base, uint16_t segmentDescriptor, uint8_t flags){
    g_idt32[interrupt].offset_1 = (uint16_t)(((uint32_t)base    ) & (uint32_t)0xFFFF);
    g_idt32[interrupt].offset_2 = (uint16_t)(((uint32_t)base>>16) & (uint32_t)0xFFFF);
    g_idt32[interrupt].selector = segmentDescriptor;
    g_idt32[interrupt].flags = flags;

    return true;
}

void enableGate32(int interrupt){
    g_idt32[interrupt].flags |= IDT_FLAG_PRESENT;
}

void disableGate32(int interrupt){
    g_idt32[interrupt].flags &= ~IDT_FLAG_PRESENT;
}


//void setIdtEntry64(){}