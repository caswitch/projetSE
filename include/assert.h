/**
 * @file assert.h
 * @brief Error handling, input checking and paranoia utilities
 */

#ifndef __ASSERT__H_
#define __ASSERT__H_

/**
 * @brief Grumbles and exits
 * @details Checks for a system call error via errno. If there was one, it uses 
 * perror. 
 * Otherwise, it prints to stderr and exits
 * 
 * This is to make sure that if you choose to grumble after something that 
 * didn't create an errno, you don't end up writing "Error: Success !" in your
 * output.
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
