 
 #include "../gaspardos_user_x86.h"
#include <stdint.h>
 void* memcpy(void*  dstptr, const void*  srcptr, size_t size) {
	unsigned char* dst = (unsigned char*) dstptr;
	const unsigned char* src = (const unsigned char*) srcptr;
	for (size_t i = 0; i < size; i++)
		dst[i] = src[i];
	return dstptr;
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
memset (void *dest, int val, size_t len)
{
  unsigned char *ptr = (unsigned char*)dest;
  while (len-- > 0)
    *ptr++ = val;
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


size_t wcslen(const wchar_t *str) {
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
static char * global_str = NULL;
void gaspardos_printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    if(!global_str) {
        global_str = gaspardapi_alloc(4096);
    }
    char *buff = global_str;
    char *out = (char*)buff;
    const char *p = fmt;

    while (*p) {
        if (*p == '%') {
            p++;
            if (*p == 's') {
                const char *s = va_arg(args, const char*);
                while (*s) *out++ = *s++;
            } else if (*p == 'd') {
                char tmp[32];
                itoa_dec(va_arg(args, int), tmp);
                for (char *t = tmp; *t; t++) *out++ = *t;
            } else if (*p == 'u') {
                char tmp[32];
                itoa_dec((int)va_arg(args, unsigned int), tmp);
                for (char *t = tmp; *t; t++) *out++ = *t;
            } else if (*p == 'x') {
                char tmp[32];
                itoa_hex(va_arg(args, unsigned int), tmp);
                for (char *t = tmp; *t; t++) *out++ = *t;
            } else if (*p == '%') {
                *out++ = '%';
            } else {
                *out++ = '%';
                *out++ = *p; // inconnu : on garde le caractère
            }
        } else {
            *out++ = *p;
        }
        p++;
    }

    *out = '\0';   

    va_end(args);
         gaspard_printf_term(global_str);

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