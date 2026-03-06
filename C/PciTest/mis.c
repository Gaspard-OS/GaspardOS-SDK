#include "PciTest.h"

void outb(uint16_t port, uint8_t value)
{
    __asm__ volatile ("outb %b0, %w1"
                      :
                      : "a"(value), "Nd"(port)
                      : "memory");
}

 uint8_t inb(uint16_t port)
{
    uint8_t value;
    __asm__ volatile ("inb %w1, %b0"
                      : "=a"(value)
                      : "Nd"(port)
                      : "memory");
    return value;
}

 void outw(uint16_t port, uint16_t value)
{
    __asm__ volatile ("outw %w0, %w1"
                      :
                      : "a"(value), "Nd"(port)
                      : "memory");
}

uint16_t inw(uint16_t port)
{
    uint16_t value;
    __asm__ volatile ("inw %w1, %w0"
                      : "=a"(value)
                      : "Nd"(port)
                      : "memory");
    return value;
}

 void outl(uint16_t port, uint32_t value)
{
    __asm__ volatile ("outl %0, %w1"
                      :
                      : "a"(value), "Nd"(port)
                      : "memory");
}

 uint32_t inl(uint16_t port)
{
    uint32_t value;
    __asm__ volatile ("inl %w1, %0"
                      : "=a"(value)
                      : "Nd"(port)
                      : "memory");
    return value;
}
