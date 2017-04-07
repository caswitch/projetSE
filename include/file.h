#ifndef __FILE__H_
#define __FILE__H_

typedef struct FICHIER {
	int fd;
	char* buffer;
	int length;
	int index;
	unsigned int mode : 1;
} *FICHIER;

/**
 * @brief Opens a FICHIER structure from a file descriptor
 * @details Allows to use my_getc
 * 
 * @param fd File descriptor as returned by open
 * 
 * @return Pointer to FICHIER structure
 */
FICHIER my_open(int fd);

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
char my_getc(FICHIER f);

/**
 * @brief Closes a file
 * @details Frees some memory, closes the file descriptors
 * 
 * @param f 
 * @return 0 if no errors
 */
int my_close(FICHIER f);

#endif
