 
 #include "../gaspardos_user_x86.h"
#include <stdint.h>
 void* memcpy(void*  dstptr, const void*  srcptr, size_t size) {
	unsigned char* dst = (unsigned char*) dstptr;
	const unsigned char* src = (const unsigned char*) srcptr;
	for (size_t i = 0; i < size; i++)
		dst[i] = src[i];
	return dstptr;
}



static const int16_t sin_table[91] = {
     0,  18,  36,  54,  71,  89, 107, 125, 142, 160,
   178, 195, 213, 230, 248, 265, 282, 299, 316, 333,
   350, 367, 384, 400, 417, 433, 449, 465, 481, 496,
   512, 527, 542, 557, 572, 587, 601, 616, 630, 644,
   658, 671, 685, 698, 711, 724, 737, 749, 761, 773,
   785, 797, 808, 819, 829, 840, 850, 860, 870, 879,
   887, 896, 904, 912, 919, 927, 934, 940, 947, 953,
   959, 965, 970, 975, 980, 985, 989, 993, 997,1000,
  1004,1007,1009,1012,1014,1016,1018,1020,1021,1023,
  1024
};

int32_t sin_scaled(uint32_t angle)
{
    int32_t a = angle % 360;

    if (a <= 90)
        return sin_table[a];
    else if (a <= 180)
        return sin_table[180 - a];
    else if (a <= 270)
        return -sin_table[a - 180];
    else
        return -sin_table[360 - a];
}

int32_t cos_scaled(uint32_t angle)
{
    return sin_scaled(angle + 90);
}
int
wcscmp (const wchar_t *s1, const wchar_t *s2)
{
  wchar_t c1, c2;

  do
    {
      c1 = *s1++;
      c2 = *s2++;
      if (c2 == L'\0')
	return c1 - c2;
    }
  while (c1 == c2);

  return c1 < c2 ? -1 : 1;
}

int
memcmp (const void *str1, const void *str2, size_t count)
{
  const unsigned char *s1 = (const unsigned char *)str1;
  const unsigned char *s2 = (const unsigned char *)str2;

  while (count-- > 0)
    {
      if (*s1++ != *s2++)
	  return s1[-1] < s2[-1] ? -1 : 1;
    }
  return 0;
}



 void *
gaspardapi_memset (void *dest, int val, size_t len)
{
  unsigned char *ptr = (unsigned char*)dest;
  while (len-- > 0)
    *ptr++ = val;
  return dest;
}

wchar_t *wcscat(wchar_t *dest, const wchar_t *src)
{
    wchar_t *d = dest;

    while (*d)
        d++;
    while ((*d++ = *src++))
        ;
    return dest;
}

size_t strlen(const char *str)
{
    size_t n = 0;
    
    while (str[n] != '\0') {
        n++;
    }
    
    return n;
}


size_t wcslen(const wchar_t *str)
{
    if (!str) return 0;

    const wchar_t *s = str;
    while (*s)
        ++s;
    return (size_t)(s - str);
}


static void itoa_dec(int value, char *buf) {
    char tmp[16];
    int i = 0, j = 0;
    int neg = (value < 0);
    unsigned int v = neg ? -value : value;

    if (v == 0) buf[j++] = '0';
    else {
        while (v > 0) {
            tmp[i++] = '0' + (v % 10);
            v /= 10;
        }
        if (neg) buf[j++] = '-';
        while (i--) buf[j++] = tmp[i];
    }
    buf[j] = '\0';
}

static void itoa_hex(unsigned int value, char *buf) {
    const char *hex = "0123456789abcdef";
    char tmp[16];
    int i = 0, j = 0;

    if (value == 0) buf[j++] = '0';
    else {
        while (value > 0) {
            tmp[i++] = hex[value & 0xF];
            value >>= 4;
        }
        while (i--) buf[j++] = tmp[i];
    }
    buf[j] = '\0';
}



void gaspard_printf(const wchar_t *format, ...) {

    va_list args;
    va_start(args, format);

    for (const wchar_t *p = format; *p != '\0'; p++) {
        if (*p == '%') {
            p++;
            switch (*p) {
                case 'd': { // Affichage d'un entier
                    int num = va_arg(args, int);
                    if (num < 0) {
                        gaspard_printf_char(L'-');
                        num = -num;
                    }
                    wchar_t buffer[12]; // Increased buffer size for safety
                    int i = 0;
                    // Handle case for num == 0
                    if (num == 0) {
                        buffer[i++] = L'0';
                    } else {
                        do {
                            buffer[i++] = L'0' + (num % 10);
                            num /= 10;
                        } while (num > 0);
                    }
                    while (i > 0) {
                        gaspard_printf_char(buffer[--i]);
                    }
                    break;
                }
                case 'x': { // Affichage d'un entier en hexadécimal
                    unsigned int num = va_arg(args, unsigned int);
                    wchar_t *hex_chars = L"0123456789ABCDEF";
                    gaspard_printf_char(L'0');
                    gaspard_printf_char(L'x');
                    // This loop will print leading zeros for smaller hex numbers.
                    // If you want to avoid leading zeros, you'll need to modify the logic
                    // to find the first non-zero nibble or format it into a string first.
                    for (int i = 7; i >= 0; i--) { // For a 32-bit unsigned int, 8 hex digits
                        gaspard_printf_char(hex_chars[(num >> (i * 4)) & 0xF]);
                    }
                    break;
                }
                case 's': { // Affichage d'une chaîne de caractères
                    wchar_t *str = va_arg(args, wchar_t*);
                    while (*str) {
                        gaspard_printf_char(*str++);
                    }
                    break;
                }
                case 'c': { // Affichage d'un caractère
                    wchar_t c = (wchar_t)va_arg(args, wchar_t);
                    gaspard_printf_char(c);
                    break;
                }
                default:   // Si le format n'est pas reconnu
                    gaspard_printf_char(L'%');
                    gaspard_printf_char(*p);
                    break;
            }
        } else {
            gaspard_printf_char(*p); // Affichage du caractère normal
        }
    }

    va_end(args);
}


/*
void printf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    for (const char *p = format; p[0] != '\0'; p++) {
        if (p[0] == '%') {
            p++;
            switch (p[0]) {
                case 'd': { // Affichage d'un entier
                    int num = va_arg(args, int);
                    if (num < 0) {
                        gaspard_printf_char('-');
                        num = -num;
                    }
                    char buffer[10];
                    int i = 0;
                    do {
                        buffer[i++] = '0' + (num % 10);
                        num /= 10;
                    } while (num > 0);
                    while (i > 0) {
                        gaspard_printf_char(buffer[--i]);
                    }
                    break;
                }
                case 'x': { // Affichage d'un entier en hexadécimal
                    unsigned int num = va_arg(args, unsigned int);
                    char *hex_chars = "0123456789ABCDEF";
                    gaspard_printf_char('0');
                    gaspard_printf_char('x');
                    for (int i = 28; i >= 0; i -= 4) {
                        gaspard_printf_char(hex_chars[(num >> i) & 0xF]);
                    }
                    break;
                }
                case 's': { // Affichage d'une chaîne de caractères
                    char *str = va_arg(args, char*);
                    while (*str) {
                        gaspard_printf_char(*str++);
                    }
                    break;
                }
                case 'c': { // Affichage d'un caractère
                    char c = (char)va_arg(args, int);
                    gaspard_printf_char(c);
                    break;
                }
                default:   // Si le format n'est pas reconnu
                    gaspard_printf_char('%');
                    gaspard_printf_char(p[0]);
                    break;
            }
        } else {
            gaspard_printf_char(p[0]); // Affichage du caractère normal
        }
    }

    va_end(args);
}
*/