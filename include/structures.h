/**
 * @file structures.h
 * @brief s_buff and sFile structure and related methods
 */

#ifndef __STRUCTURES__H_
#define __STRUCTURES__H_

#define BUFF_SIZE 256
#define WRITE 1
//#define READ 0
#define BUFFER_SIZE 256


/**
 * s can be any data type. Currently, it is an alias for char.
 */
typedef char s;

/**
 * @brief [For internal use] A node in a linked list
 * @details Internal use only. Use s_buff.
 */
typedef struct s_node {
	unsigned int size;		/*!< Capacity of the node. Should be BUFF_SIZE */
	unsigned int readAddr;	/*!< Index of the element that will be read 
	                      	 *   next by getc */
	unsigned int writeAddr;	/*!< Index in which putc will write */
	s* mem;					/*!< The actual array */
	struct s_node* next;	/*!< Pointer to the next node */
	struct s_node* prec;	/*!< Pointer to the last node */
} node;

/**
 * @brief Linked list; Self-expanding data buffer
 * @details Linked list of nodes, which are short buffers of 256 elements each
 * An element can be of any type, as defined by 'typedef <type> s'. 
 * It is by default a linked list of characters 
 */
typedef struct s_buff{
	node* start;			/*!< First node of the chain */
	node* readNode;			/*!< Node to read from */
	node* writeNode;		/*!< Node to write to */
} Buffer;

/**
 * @brief Frees the buffer
 * And each of its nodes
 * 
 * @param b Buffer* pointer to a buffer structure
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
 * @param b Buffer* pointer to a buffer structure 
 * @param c Element (of type s) to place in the buffer
 * 
 * @return -1 if error, 0 otherwise
 */
int buff_putc(Buffer* b, s c);

/**
 * @brief [Unused] Get the length of the text inside the buffer
 * @details Not the buffer's capacity
 * 
 * @param b Buffer* pointer to a buffer structure 
 * @return Length of the buffer's content
 */
int buff_getSize(Buffer* b);

/**
 * @brief Prints the contents of the buffer
 * @details Uses write
 * 
 * @param b Buffer* pointer to a buffer structure 
 * @return -1 if error
 */
int buff_print(Buffer* b);

/**
 * @brief Fetch the individual characters from a buffer in order
 * @details One at a time, from 0 to size-1. Returns -1 if you reached the end
 * of the buffer. Reset with buff_setpos(b, 0, 0)
 * 
 * @param b Buffer* pointer to a buffer structure 
 * @return A character
 */
s buff_getc(Buffer* b);

/**
 * @brief Removes the last character from a buffer
 * @details Does not down-size the buffer
 * 
 * @param b Buffer* pointer to a buffer structure 
 * @return The character removed
 */
s buff_unputc(Buffer* b);

/**
 * @brief Sets the reading and writing positions to 0.
 * @details Does not 
 * Does not downsize the buffer. 
 * 
 * @param b Buffer* pointer to a buffer structure 
 * @return 0 if no errors
 */
void buff_reset(Buffer* b);

/**
 * @brief File structure as returned by my_open
 * @details Contains informations about your file that my_getc needs
 */
typedef struct s_file {
	int fd;			/*!< File descriptor */
	char* buffer;	/*!< An array */
	int length;		/*!< Length of the buffer */
	int index;		/*!< Index of the character that will be read by getc */
} sFile;

/**
 * @brief Opens a sFile structure from a file descriptor
 * @details Allows to use my_getc
 * 
 * @param fd sFile descriptor as returned by open
 * 
 * @return Pointer to sFile structure
 */
sFile* my_open(int fd);

/**
 * @brief Bufferized getchar
 * @details To minimize disk access, caches 512 characters from 
 * your file at once in an internal buffer and 
 * returns one character from your file at a time.
 * 
 * @param f 
 * @return The next character, EOF if you reached the end of your file,
 * -1 if something went wrong.
 */
char my_getc(sFile* f);

/**
 * @brief Closes a file
 * @details Frees some memory, closes the file descriptors
 * 
 * @param f 
 * @return 0 if no errors
 */
int my_close(sFile* f);

#endif
