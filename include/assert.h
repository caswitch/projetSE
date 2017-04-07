#ifndef __ASSERT__H_
#define __ASSERT__H_

/**
 * @brief Grumbles and exits
 * @details Checks for a system call error via errno. If there was one, it uses 
 * perror. 
 * Otherwise, it prints to stderr and exits
 * 
 * @param msg Error message
 */
void grumble(char* msg);

/**
 * @brief Checks if return_value is -1. If so, grumbles and exits
 * 
 * @param return_value int
 * @param msg Error message
 */
void assert(int return_value, char* msg);

#endif
