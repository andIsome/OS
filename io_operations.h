#pragma once
#include <stdint.h>

void outb(uint16_t port, uint8_t val);

uint8_t inb(uint16_t port);

void outw(uint16_t port, uint16_t value);

uint16_t inw(uint16_t port);

void enable_interrupts();

void disable_interrupts();

void NMI_enable();

void NMI_disable();

void io_wait();