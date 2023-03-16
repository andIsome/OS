export PATH=$PATH:/usr/local/i386elfgcc/bin

nasm "boot.asm" -f bin -o "Binaries/boot.bin"
nasm "kernel_entry.asm" -f elf -o "Binaries/kernel_entry.o"
i386-elf-g++ -ffreestanding -mgeneral-regs-only -m32 -c "kernel.cpp" -o "Binaries/kernel.o" -ggdb
i386-elf-g++ -ffreestanding -m32 -g -c "idt.c" -o "Binaries/idt.o" -ggdb
i386-elf-g++ -ffreestanding -m32 -g -c "memory.c" -o "Binaries/memory.o" -ggdb
i386-elf-g++ -ffreestanding -m32 -g -c "io_operations.c" -o "Binaries/io_operations.o" -ggdb
i386-elf-g++ -ffreestanding -m32 -g -c "pic.c" -o "Binaries/pic.o" -ggdb
nasm "zeroes.asm" -f bin -o "Binaries/zeroes.bin"

i386-elf-ld -o "Binaries/full_kernel.bin" -Ttext 0x1000 "Binaries/kernel_entry.o" "Binaries/kernel.o" "Binaries/io_operations.o" "Binaries/pic.o" "Binaries/memory.o" "Binaries/idt.o" --oformat binary

cat "Binaries/boot.bin" "Binaries/full_kernel.bin" "Binaries/zeroes.bin" > "Binaries/OS.bin"

qemu-system-x86_64 -debugcon stdio -soundhw pcspk -drive format=raw,file="Binaries/OS.bin",index=0,if=floppy,  -m 128M

