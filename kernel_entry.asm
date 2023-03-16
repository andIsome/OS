section .text
    [bits 32]
    [extern main]
    call main
    cli
    hlt
    jmp $

