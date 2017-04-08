/**
 * @file file.h
 * @brief Anything sFile related
 */

#ifndef __FILE__H_
#define __FILE__H_
#define BUFFER_SIZE 256

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
