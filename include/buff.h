/**
 * @file buff.h
 * @brief s_buff structure and related methods
 */

#ifndef __BUFF__H_
#define __BUFF__H_
#define BUFF_SIZE 256
#define WRITE 1
//#define READ 0

/**
 * @brief Infinite text buffer
 * @details Buffer scales up as it fills. Scales by 256 bytes every time the
 * write pointer reaches the end of the buffer using realloc.
 * 
 */
typedef char s;
typedef struct s_node {
	unsigned int size;
	unsigned int readAddr;
	unsigned int writeAddr;
	s* mem;
	struct s_node* next;
	struct s_node* prec;
} node;

typedef struct s_buff{
	unsigned int length;
	node* start;
	node* readNode;
	node* writeNode;
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
 * @brief Prints the contents of the buffer
 * @details Uses write
 * 
 * @param b 
 * @return -1 if error
 */
int buff_print(Buffer* b);

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
 * @brief Resets the buffer entirely
 * @details By setting the reading and writing positions to 0.
 * Does not downsize the buffer. 
 * 
 * @param b 
 * @return 0 if no errors
 */
void buff_reset(Buffer* b);

#endif
