#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>

/**
 * @defgroup GaspardOSAPI The GaspardOS API's
 * @{
 */




/** @brief Copies a block of memory from source to destination.
    *
    * This function copies the values of num bytes from the location pointed to by source directly to the memory block pointed to by destination. Both blocks are assumed to have appropriate sizes and must not overlap.
    *
    * @param dstptr Pointer to the destination array where the content is to be copied, must be allocated with a size of at least 'size' bytes.
    * @param srcptr Pointer to the source of data to be copied.
    * @param size Number of bytes to copy.
    *
    * @return This function returns a pointer to destination which is dstptr.
    */
 void* memcpy(void*  dstptr, const void*  srcptr, size_t size);


/**

    @brief Compares two blocks of memory.
    This function compares the first count bytes of the block of memory pointed by str1 to the first count bytes pointed by str2, returning zero if they all match or a value different from zero representing which is greater if they do not.
    @param str1 Pointer to block of memory.
    @param str2 Pointer to block of memory.
    @param count Number of bytes to compare.
    @return This function returns an integer less than, equal to or greater than zero if the first count bytes of str1 is found, respectively, to be less than, to match or be greater than the first count bytes of str2.
     */

static int
memcmp (const void *str1, const void *str2, size_t count);


/** 

    @brief Fills a block of memory with a particular value.
    This function fills the first count bytes of the memory area pointed to by dest with the constant byte val.
    @param dest Pointer to the block of memory to fill.
    @param val Value to be set. The value is passed as an int, but the function fills the block of memory using the unsigned char conversion of this value.
    @param len Number of bytes to be set to the value.
    @return This function returns a pointer to dest.
     */
static void *
memset (void *dest, int val, size_t len);





/// @brief Return the size of a string
/// @param str 
/// @return the size or 0 if empty
size_t strlen(const char *str);

/// @brief Return the size of a wide string
/// @param str 
/// @return the size
size_t wcslen(const wchar_t *str);



/// @brief Print a ASCII STR to available terminal
/// @param str 
extern void gaspard_printf_term( const char *str); //Ok DOC


/// @brief Print in a hexadecimal way to available terminal
/// @param data 
extern void gaspard_printf_hex(uint32_t data); // ok doc 
/// @brief Print in a decimal way to available terminal
/// @param data 
extern void gaspard_printf_number(uint32_t data); // ok doc



 /// @brief print a char to available terminal
 /// @param key 
 extern void gaspard_printf_char(uint32_t key); // ok doc





 /// @brief Alloc a 4086 bytes memory block
 /// @param size 
 /// @return the ptr or NULL
 extern void *gaspardapi_alloc(uint32_t size); // OK DOC


/// @brief Free a buffer
/// @param ptr 
extern void gaspardapi_free(void *ptr); // OK DOC





#include "common_defs.h"
/// @brief Return the last GaspardOS event
extern enum QueueType gaspardapi_getstatus(); // a faire 



/// @brief Clear the screen output
extern void gaspardapi_clearscreen(); // text mode 


/// @brief load an app and start the gmain's of the app
/// @param index must be equal to >= 1
/// @return  -1 = failed; 1 =  ok
extern int gaspardapi_load_app(int index,bool admin); // statut en return ADMIN FONCTION

/// @brief stop an app already started 
/// @param index must be equal to >= 1
/// @return -1 = failed; 1 =  ok
extern int gaspardapi_stop_app(int index); // statut en return ADMIN FONCTION
// gaspardapi_getstatus l'ipc passe par la 

/// @brief Send a message to a another started app
/// @param buffer 
/// @return -1 = task is not active; 
/// 0 = task is not loaded; 
/// 1 = SUCCESS;
extern int gaspardapi_send_ipc(void *buffer); // a faire


/// @brief Receive a message to a another started app
/// @param buffer 
/// @return -1 = No message available; 
/// 0 = SUCCESS; 
extern int gaspardapi_receive_ipc(void *buffer); // a faire



void gaspardos_printf(const char *format, ...);





extern void gaspardapi_init_video() ;
extern void gaspardapi_demo_3d();

extern void gaspardapi_put_pixel(uint32_t x,uint32_t y,uint32_t color);
/** @} */
