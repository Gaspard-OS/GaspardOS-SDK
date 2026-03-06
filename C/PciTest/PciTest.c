#include "PciTest.h"

static inline uint32_t pci_cfg_addr_x(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset) {
    return 0x80000000u
         | ((uint32_t)bus  << 16)
         | ((uint32_t)dev  << 11)
         | ((uint32_t)func <<  8)
         | (offset & 0xFC);
}
/* Lecture 32 bits brute à offset aligné (offset % 4 == 0) */
static inline uint32_t pci_read32(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset_aligned) {
    uint32_t addr = pci_cfg_addr_x(bus, dev, func, offset_aligned);
    outl(addr, PCI_CONFIG_ADDRESS);
    return inl(PCI_CONFIG_DATA);
}

uint8_t pci_read_config8(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset) {
    uint32_t addr  = pci_cfg_addr_x(bus, dev, func, offset);
    uint8_t  shift = (offset & 3) * 8;
    outl(PCI_CONFIG_ADDRESS, addr);
    uint32_t dw = inl(PCI_CONFIG_DATA);
    return (uint8_t)((dw >> shift) & 0xFFu);
}

uint16_t pci_read_config16(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset) {
    uint32_t addr  = pci_cfg_addr_x(bus, dev, func, offset);
    uint8_t  sh    = (offset & 2) * 8; // offset pair
    outl(PCI_CONFIG_ADDRESS, addr);
    uint32_t dw = inl(PCI_CONFIG_DATA);
    return (uint16_t)((dw >> sh) & 0xFFFFu);
}

uint32_t pci_read_config32(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset) {
    uint32_t addr = pci_cfg_addr_x(bus, dev, func, offset & 0xFC);
    outl(PCI_CONFIG_ADDRESS, addr);
    return inl(PCI_CONFIG_DATA);
}

void pci_write_config8(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset, uint8_t value) {
    uint32_t addr = pci_cfg_addr_x(bus, dev, func, offset);
    uint8_t  shift = (offset & 3) * 8;

    outl(PCI_CONFIG_ADDRESS, addr);
    uint32_t dw = inl(PCI_CONFIG_DATA);          // lire le DWORD existant
    dw &= ~(0xFFu << shift);                     // efface l’ancien octet
    dw |= ((uint32_t)value << shift);            // insère le nouvel octet

    outl(PCI_CONFIG_ADDRESS, addr);
    outl(PCI_CONFIG_DATA, dw);                   // réécrit le DWORD complet
}

void pci_write_config16(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset, uint16_t value) {
    uint32_t addr = pci_cfg_addr_x(bus, dev, func, offset);
    uint8_t  sh   = (offset & 2) * 8;

    outl(PCI_CONFIG_ADDRESS, addr);
    uint32_t dw = inl(PCI_CONFIG_DATA);
    dw &= ~(0xFFFFu << sh);
    dw |= ((uint32_t)value << sh);

    outl(PCI_CONFIG_ADDRESS, addr);
    outl(PCI_CONFIG_DATA, dw);
}

void pci_write_config32(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset, uint32_t value) {
    uint32_t addr = pci_cfg_addr_x(bus, dev, func, offset & 0xFC);
    outl(PCI_CONFIG_ADDRESS, addr);
    outl(PCI_CONFIG_DATA, value);
}
static void pci_scan_bus(uint8_t bus);
const char *pci_class_name(uint8_t classCode) {
    switch (classCode) {
        case 0x00: return "Unclassified";
        case 0x01: return "Mass Storage Controller";
        case 0x02: return "Network Controller";
        case 0x03: return "Display Controller";
        case 0x04: return "Multimedia Controller";
        case 0x05: return "Memory Controller";
        case 0x06: return "Bridge Device";
        case 0x07: return "Simple Communication Controller";
        case 0x08: return "Base System Peripheral";
        case 0x09: return "Input Device Controller";
        case 0x0A: return "Docking Station";
        case 0x0B: return "Processor";
        case 0x0C: return "Serial Bus Controller";
        default:   return "Unknown Class";
    }
}

const char *pci_subclass_name(uint8_t classCode, uint8_t subClass) {
    switch (classCode) {

        case 0x01: /* Mass Storage */
            switch (subClass) {
                case 0x00: return "SCSI Controller";
                case 0x01: return "IDE Controller";
                case 0x06: return "SATA Controller";
                case 0x08: return "NVMe Controller";
                default:   return "Unknown Storage Subclass";
            }

        case 0x02: /* Network */
            switch (subClass) {
                case 0x00: return "Ethernet Controller";
                case 0x01: return "Token Ring Controller";
                case 0x80: return "Other Network Controller";
                default:   return "Unknown Network Subclass";
            }

        case 0x03: /* Display */
            switch (subClass) {
                case 0x00: return "VGA Compatible Controller";
                case 0x02: return "3D Controller";
                default:   return "Unknown Display Subclass";
            }

        case 0x06: /* Bridge */
            switch (subClass) {
                case 0x00: return "Host Bridge";
                case 0x01: return "ISA Bridge";
                case 0x04: return "PCI-to-PCI Bridge";
                default:   return "Unknown Bridge Subclass";
            }

        case 0x0C: /* Serial Bus */
            switch (subClass) {
                case 0x03: return "USB Controller";
                case 0x05: return "SMBus Controller";
                default:   return "Unknown Serial Bus Subclass";
            }

        default:
            return "Unknown Subclass";
    }
}

void pci_print_class(uint8_t classCode, uint8_t subClass) {
    gaspard_printf_term("pci_class_name(classCode) : ");
    gaspard_printf_term(pci_class_name(classCode));
    gaspard_printf_term("\n");
}

static void pci_scan_function(uint8_t bus, uint8_t dev, uint8_t fun) {
    uint16_t vendorId = pci_read_config16(bus, dev, fun, 0x00);
    uint16_t deviceId = pci_read_config16(bus, dev, fun, 0x02);

    uint16_t classSub = pci_read_config16(bus, dev, fun, 0x0A);
    uint8_t classCode = (classSub >> 8) & 0xFF;
    uint8_t subClass  = classSub & 0xFF;
    uint32_t bar0 = pci_read32(bus, dev, fun, 0x10) & ~0xFu;
    /*
    printf("bus=%d dev=%d fun=%d VendorID=%x DeviceID=%x BAR0=%x\n",
        bus, dev, fun, vendorId, deviceId, bar0);
        */
   // pci_print_class(classCode, subClass);
    gaspard_printf_term("vendor id : ");
    gaspard_printf_hex(vendorId);
    gaspard_printf_term("  deviceID: ");
    gaspard_printf_hex(deviceId);
    gaspard_printf_term(" \n");

    // Pont PCI-PCI : class 0x06, subclass 0x04
    if (classCode == 0x06 && subClass == 0x04) {
        // Registre bus numbers à 0x18 : [primary|secondary|subordinate|...]
        uint32_t buses = pci_read_config32(bus, dev, fun, 0x18);
        uint8_t secondary   = (buses >> 8) & 0xFF;
        uint8_t subordinate = (buses >> 16) & 0xFF;

        // Sécurité : évite boucle infinie / valeurs bizarres
        if (secondary != 0 && secondary <= subordinate) {
            pci_scan_bus(secondary);
        }
    }
    
}
static void pci_scan_bus(uint8_t bus) {
    for (uint8_t dev = 0; dev < 32; dev++) {

        // function 0 d’abord : si vendor == FFFF, device absent => skip
        uint16_t vendor0 = pci_read_config16(bus, dev, 0, 0x00);
        if (vendor0 == 0xFFFF) continue;

        // header type pour savoir si multi-fonction
        uint8_t headerType = (uint8_t)(pci_read_config16(bus, dev, 0, 0x0E) & 0xFF);
        uint8_t isMulti = (headerType & 0x80) != 0;

        uint8_t funCount = isMulti ? 8 : 1;
        for (uint8_t fun = 0; fun < funCount; fun++) {
            uint16_t vendor = pci_read_config16(bus, dev, fun, 0x00);
            if (vendor == 0xFFFF) continue;
            pci_scan_function(bus, dev, fun);
        }
    }
}
void pci_scan_custom_recursive(void) {
    // Cas simple : scan la hiérarchie depuis bus 0
    pci_scan_bus(0);

    // Variante (optionnelle) : si host bridge multi-fonction, scanner bus derrière
    // les autres fonctions du device 0:0.* (utile sur certaines machines).
    uint16_t vendor0 = pci_read_config16(0, 0, 0, 0x00);
    if (vendor0 != 0xFFFF) {
        uint8_t headerType = (uint8_t)(pci_read_config16(0, 0, 0, 0x0E) & 0xFF);
        if (headerType & 0x80) {
            for (uint8_t fun = 1; fun < 8; fun++) {
                uint16_t v = pci_read_config16(0, 0, fun, 0x00);
                if (v == 0xFFFF) continue;
                // Souvent, chaque fonction correspond à un bus “root” différent.
                // Le bus number peut être déduit via ACPI/MCFG; en legacy, on scanne
                // parfois directement pci_scan_bus(fun) ou on garde la version simple.
                pci_scan_function(0, 0, fun);
            }
        }
    }
}
// Structure pour stocker un résultat trouvé
typedef struct {
    uint8_t  bus;
    uint8_t  dev;
    uint8_t  fun;
    uint16_t vendorId;
    uint16_t deviceId;
    uint32_t bar0;
} pci_device_t;

// Callback appelé pour chaque device trouvé
typedef void (*pci_device_callback_t)(pci_device_t* device);

// Variables internes pour la recherche par classe
static uint8_t  _search_class    = 0;
static uint8_t  _search_subclass = 0;
static pci_device_callback_t _search_callback = 0;


void pci_find_by_class(uint8_t target_class,
                       uint8_t target_subclass,
                       pci_device_callback_t callback);

// ─── Implémentation interne ───────────────────────────────────────────────────

static void _pci_check_function_for_class(uint8_t bus, uint8_t dev, uint8_t fun) {
    uint16_t vendor = pci_read_config16(bus, dev, fun, 0x00);
    if (vendor == 0xFFFF) return;

    uint16_t classSub = pci_read_config16(bus, dev, fun, 0x0A);
    uint8_t  classCode = (classSub >> 8) & 0xFF;
    uint8_t  subClass  =  classSub       & 0xFF;

    // Pont PCI-PCI → descente récursive
    if (classCode == 0x06 && subClass == 0x04) {
        uint32_t buses      = pci_read_config32(bus, dev, fun, 0x18);
        uint8_t  secondary  = (buses >>  8) & 0xFF;
        uint8_t  subordinate= (buses >> 16) & 0xFF;
        if (secondary != 0 && secondary <= subordinate)
            _pci_scan_bus_for_class(secondary);
    }

    // Filtre classe / sous-classe
    if (classCode != _search_class || subClass != _search_subclass) return;
    if (!_search_callback) return;

    pci_device_t found = {
        .bus      = bus,
        .dev      = dev,
        .fun      = fun,
        .vendorId = vendor,
        .deviceId = pci_read_config16(bus, dev, fun, 0x02),
        .bar0     = pci_read_config32(bus, dev, fun, 0x10) & ~0xFu,
    };
    _search_callback(&found);
}

static void _pci_scan_bus_for_class(uint8_t bus) {
    for (uint8_t dev = 0; dev < 32; dev++) {
        uint16_t vendor0 = pci_read_config16(bus, dev, 0, 0x00);
        if (vendor0 == 0xFFFF) continue;

        uint8_t headerType = (uint8_t)(pci_read_config16(bus, dev, 0, 0x0E) & 0xFF);
        uint8_t funCount   = (headerType & 0x80) ? 8 : 1;

        for (uint8_t fun = 0; fun < funCount; fun++)
            _pci_check_function_for_class(bus, dev, fun);
    }
}

void pci_find_by_class(uint8_t target_class,
                       uint8_t target_subclass,
                       pci_device_callback_t callback) {
    _search_class    = target_class;
    _search_subclass = target_subclass;
    _search_callback = callback;

    _pci_scan_bus_for_class(0);

    // Host bridge multi-fonction → root buses supplémentaires
    uint16_t vendor0 = pci_read_config16(0, 0, 0, 0x00);
    if (vendor0 != 0xFFFF) {
        uint8_t ht = (uint8_t)(pci_read_config16(0, 0, 0, 0x0E) & 0xFF);
        if (ht & 0x80) {
            for (uint8_t fun = 1; fun < 8; fun++) {
                uint16_t v = pci_read_config16(0, 0, fun, 0x00);
                if (v != 0xFFFF)
                    _pci_check_function_for_class(0, 0, fun);
            }
        }
    }

    // Nettoyage
    _search_callback = 0;
}
static void audio_callback(pci_device_t* d) {
    gaspard_printf_term("audio found → bus=");
    gaspard_printf_hex(d->bus);
    gaspard_printf_term(" dev=");
    gaspard_printf_hex(d->dev);
    gaspard_printf_term(" vendor=");
    gaspard_printf_hex(d->vendorId);
    gaspard_printf_term(" BAR0=");
    gaspard_printf_hex(d->bar0);
    gaspard_printf_term("\n");
}
void gmain() {


    pci_scan_custom_recursive();
    pci_find_by_class(0x4, 0x03, audio_callback);
}