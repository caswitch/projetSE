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
 * @param str 
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
 * @brief Executes the program *prog with the arguments in args[]
 * @details Exécute la commande à lancer par le programme.
 * Si la commande est invalide, pour distinguer une erreur de excvp
 * d'une erreur du programme qui lui est donné en argument 
 * (commande invalide), le processsus fils envoie un signal au père.
 * Ainsi, si le processus fils ne s'est pas terminé avec un exit,  
 * c'est que la commande dans excvp est invalide.
 * 
 * @param prog Path to the executable
 * @param args char *const argv[] table. args[0] should be the executable's name
 * 
 * @return A file descriptor in read mode for the tube the program is 
 * redirected into.
 */
int callProgram(char const *prog, char *const args[]);

/**
 * @brief Prints exit status in "i" if it detects changes in it from the last 
 * call.
 * @details Lors du premier lancement de la commande,
 * si le retour est un exit, le code de retour est affiché,
 * sinon, le processus fils qui exécute le programme 
 * envoie un signal au père pour faire la différence entre
 * une erreur produite lors de l'exécution d'execvp 
 * (indépendamment de ses argumnents) et
 * une erreur du programme donné en argument à execvp 
 * (commande invalide).
 * S'il y a, les lancements de la commande suivants détectent
 * les changements de code de retour.
 * Le premier lancement n'ayant pas quitté le programme, 
 * les arguments de excvp sont valides.
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
 * @brief Check if the format to -t option can formating date and time
 * 
 * @details Doesn't indicate if strftime fail
 * 
 * @param optarg Argument to -t option
 * @param command The executable's file name
 */
void check_format (const char * optarg, char * const command);

#endif
