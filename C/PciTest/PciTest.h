#pragma once 



#include "../gaspardos_user_x86.h"


void     outb(uint16_t port, uint8_t value);
uint8_t  inb(uint16_t port);

void     outw(uint16_t port, uint16_t value);
uint16_t inw(uint16_t port);

void     outl(uint16_t port, uint32_t value);
uint32_t inl(uint16_t port);


#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC

static void _pci_scan_bus_for_class(uint8_t bus);