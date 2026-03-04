#include "gaspardos_user_x86.h"

void outb(uint16_t port, uint8_t value)
{
    __asm__ volatile ("outb %b0, %w1"
                      :
                      : "a"(value), "Nd"(port)
                      : "memory");
}

void gmain() {



    outb(0xFF, 0x3F8);



}