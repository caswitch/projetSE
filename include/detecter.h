/**
 * @file detecter.h
 * @brief Main
 */

/*! @mainpage My Personal Index Page
 *
 * @section Rapport
 *
 * ### Pourquoi doxygen c'est trop bien ###
 * - Notre rapport pourra être fait en markdown et foutu direct dans la doc
 * - C'est trop bien
 * - ROSE !
 * 
 * @section install_sec Deuxième section
 * 
 * ### Les accents marchent ! ###
 * 
 *
 * @subsection step1 Step 1: Licorne
 * 
 * ![Licorne !](http://s-media-cache-ak0.pinimg.com/736x/0e/3d/f6/0e3df60cabeec611be2872b82db57458.jpg)
 *
 * etc...
 */

#ifndef __DETECTER__H_
#define __DETECTER__H_


/**
 * @brief String to number conversion but exits on error instead of returning 0
 * @details Useful for converting user inputs
 * 
 * @param Input string
 * @return 
 */
int safe_atoi(char const* str);

/**
 * @brief Prints usage and exits with a failure
 * @details 
 * 
 * @param command The executable's file name
 */
void usage(char * const command);

/**
 * @brief Compares the contents of a file a buffer
 * @details Replaces the contents of the buffer with the bytes that were
 * read from the file. 
 * 
 * @param fd File descriptor to read from
 * @param cache A Buffer* as returned by new().
 * 
 * @return Either NULL if no differences were found, or a the address of a
 * buffer containing what was read from the file descriptor.
 */
bool output_delta(int fd, Buffer* cache);

/**
 * @brief Prints current time according to a format. See strftime.
 * @details Uses strftime.
 * 
 * @param format The format. See strftime.
 */
void print_time(char *format);

/**
 * @brief Executes the program prog with the arguments in args
 * @details See exit_code() to handle the eventual errors.
 * 
 * Uses the fork() and execvp() functions
 * If execvp() fails, we send a signal to the father so as to notify him that 
 * the program couldn't be launched properly.
 * 
 * This signal can be handled by exit_code() automatically.
 * 
 * @param prog Path to the executable
 * @param args char *const argv[] table. args[0] should be the executable's name
 * 
 * @return A file descriptor in read mode for the tube the program is 
 * redirected into.
 */
int callProgram(char const *prog, char *const args[]);

/**
 * @brief Handles the termination of the child process
 * @details 
 * - Waits for the termination of a child process with wait() and checks its 
 * wstatus
 * - Handles the signals of the child process, it distinguishes the various
 * errors that could have happened to the child;
 * 	- Errors in execvp due to the arguments the user gave to us
 * 	- Plain old errors in execvp 
 * 
 * - Prints the exit code of the child if it is different from the last time we
 * used the function. (uses a static variable to store the last one)
 * 
 * @param i A wstatus as given by wait()
 * @param opt_c Sensitive to changes in the exit code
 */
void exit_code(bool opt_c);

/**
 * @brief Executes a program periodically
 * @details Does too much
 * 
 * @param prog Path to executable file
 * @param args Its arguments in the argv[] format
 * @param opt_i Time interval in milliseconds between each call
 * @param opt_l Limits the number of calls
 * @param opt_c Sensitive to changes in the exit code
 * @param opt_t Prints a timestamp on each call
 * @param format Format of the timestamp
 */
void interval(char const *prog, char *const args[], int opt_i, 
			  int opt_l, bool opt_c, bool opt_t, char* format);

/**
 * @brief Checks if the input is a valid time format as specified by strftime
 * 
 * @details Because strftime won't tell us when it fails
 * 
 * @param optarg Argument of the option -t
 * @param command The executable's file name
 */
void check_format (const char * optarg, char * const command);

#endif
