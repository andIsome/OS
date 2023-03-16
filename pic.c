#include "pic.h"
#include "io_operations.h"

uint8_t pic1_interrupt_mask = 0xFF;
uint8_t pic2_interrupt_mask = 0xFF;

void pic_send_eoi(uint8_t irq){
	if(irq >= 8)
		outb(PIC2_COMMAND_PORT, PIC_EOI);
 
	outb(PIC1_COMMAND_PORT, PIC_EOI);
}

void setup_pic(){
    // Send ICWs - Initialization Command Words
    outb(PIC1_COMMAND_PORT, 0x11);
	outb(PIC2_COMMAND_PORT, 0x11);

    // ICW2: Vector Offset (this is what we are fixing)
	// Start PIC1 at 32 (0x20 in hex) (IRQ0=0x20, ..., IRQ7=0x27)
	// Start PIC2 right after, at 40 (0x28 in hex)
	outb(PIC1_DATA_PORT, 0x20);
	outb(PIC2_DATA_PORT, 0x28);
	// ICW3: Cascading (how master/slave PICs are wired/daisy chained)
	// Tell PIC1 there is a slave PIC at IRQ2 (why 4? don't ask me - https://wiki.osdev.org/8259_PIC)
	// Tell PIC2 "its cascade identity" - again, I'm shaky on this concept. More resources in notes
	outb(PIC1_DATA_PORT, 0x0);
	outb(PIC2_DATA_PORT, 0x0);
	// ICW4: "Gives additional information about the environemnt"
	// See notes for some potential values
	// We are using 8086/8088 (MCS-80/85) mode
	// Not sure if that's relevant, but there it is.
	// Other modes appear to be special slave/master configurations (see wiki)
	outb(PIC1_DATA_PORT, 0x1);
	outb(PIC2_DATA_PORT, 0x1);

	// Mask all interrupts
	outb(PIC1_DATA_PORT, pic1_interrupt_mask);
	outb(PIC2_DATA_PORT, pic2_interrupt_mask);

	// Set a timer @ 60hz
	uint32_t divisor = 1193180 / 60; // Set frequency to 60Hz

    // Set the PIT to generate interrupts at the desired frequency
    outb(0x43, 0x36);
    outb(0x40, (uint8_t)(divisor & 0xFF));
    outb(0x40, (uint8_t)((divisor >> 8) & 0xFF));
}

void pic1_mask_interrupt(uint8_t interrupt){}
void pic2_mask_interrupt(uint8_t interrupt){}

void pic1_unmask_interrupt(uint8_t interrupt){}
void pic2_unmask_interrupt(uint8_t interrupt){}