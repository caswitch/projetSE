#include <stdbool.h>

#ifndef __BUFF__H_
#define __BUFF__H_

/**
 * @brief Infinite text buffer
 * @details Buffer scales up as it fills. Scales by 256 bytes every time the
 * write pointer reaches the end of the buffer using realloc.
 * 
 */
typedef struct s_buff{
	unsigned int size;
	unsigned int readAddr;
	unsigned int writeAddr;
	char* mem;
} Buffer;

/**
 * @brief Frees the buffer
 * 
 * @param b Pointer
 */
void buff_free(Buffer* b);

/**
 * @brief Creates and allocates a new buffer
 * @return Pointer to the buffer. See struct s_buff
 */
Buffer* buff_new();

/**
 * @brief Appends c to the end of the buffer
 * @details The buffer will scale up if there is no room left for c
 * 
 * @param b 
 * @param c 
 * 
 * @return The buffer's new address. Stays unchanged most of the time.
 */
Buffer* buff_putc(Buffer* b, char c);

/**
 * @brief Get the length of the text inside the buffer
 * @details Not the buffer's capacity
 * 
 * @param b 
 * @return Length of the buffer's content
 */
int buff_getSize(Buffer* b);

/**
 * @brief Fetches the content of the buffer
 * @details Turns it into a sequential array
 * 
 * @param b 
 * @return Pointer to a fixed size array
 */
char* buff_toString(Buffer* b);

/**
 * @brief Fetch the individual characters from a buffer in order
 * @details One at a time, from 0 to size-1. Returns -1 if you reached the end
 * of the buffer. Reset with buff_setpos(b, 0, 0)
 * 
 * @param b 
 * @return A character
 */
char buff_getc(Buffer* b);

/**
 * @brief Removes the last character from a buffer
 * @details Does not down-size the buffer
 * 
 * @param b 
 * @return The character removed
 */
char buff_unputc(Buffer* b);

/**
 * @brief Set the reading/writing position in the buffer
 * @details The writing position defines the length of the content. 
 * 
 * @param b Buffer
 * @param mode Boolean. 0=read, 1=write
 * @param int Position
 * @return 0 if no error
 */
bool buff_setpos(Buffer* b, bool mode, unsigned int pos);

/**
 * @brief Resets the buffer entirely
 * @details By setting the reading and writing positions to 0.
 * Does not downsize the buffer. 
 * 
 * @param b 
 * @return 0 if no errors
 */
bool buff_reset(Buffer* b);

#endif