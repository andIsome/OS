#include "io_operations.h"


void outb(uint16_t port, uint8_t val){
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

uint8_t inb(uint16_t port){
    uint8_t ret;
    asm volatile ( "inb %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}

void outw(uint16_t port, uint16_t value){
    asm volatile("outw %0, %1" : : "a"(value), "Nd"(port));
}

uint16_t inw(uint16_t port){
    uint16_t ret;
    asm volatile ( "inw %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}

void enable_interrupts(){
    asm volatile ("sti");
}

void disable_interrupts(){
    asm volatile ("cli");
}

void NMI_enable() {
    outb(0x70, inb(0x70) & 0x7F);
    inb(0x71);
}
 
void NMI_disable() {
    outb(0x70, inb(0x70) | 0x80);
    inb(0x71);
}

void io_wait(){
    outb(0x80, 0);
}