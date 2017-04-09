#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <wait.h>
#include <errno.h>

#include "buff_and_file.h"
#include "detecter.h"

#define BUFFT_SIZE 256
#define CONVERT_USEC 1000
#define NUMBER_OF_STRINGS 42
#define STRING_LENGTH 2
#define ZERO_TERMINATOR 1

/**
 * @def GRUMBLE(msg)
 *
 * @brief Grumbles and exits
 * @details Checks for a system call error via errno.
 * If there was one, it uses perror. 
 * Otherwise, it prints to stderr and exits.
 * 
 * This is to make sure that if you choose to grumble after 
 * something that didn't create an errno, 
 * you don't end up writing "Error: Success !" in your output.
 *
 * @param msg Error message
 */
#define GRUMBLE(msg)                                       \
	if (errno){                                            \
		perror(msg);                                       \
	}                                                      \
	else{                                                  \
		fprintf(stderr, "%s\n", msg);                      \
	}                                                      \
	exit(EXIT_FAILURE);

/**
 * @def ALLOC_NULL(alloc, msg, ptr, OPERATION)
 *
 * @brief Checks if the malloc function didi not fail.
 * @brief If so, free, grumble and exits.
 * 
 * @param alloc the return pointer of a malloc
 * @param msg Error pessage
 * @param ptr a pointer to check if is NULL or not
 * @param OPERATION can be an instruction, a fonction or a macro
 */
#define ALLOC_NULL(alloc, msg, ptr, OPERATION)             \
	if (alloc == NULL){                                    \
		if (ptr == NULL){                                  \
			GRUMBLE(msg)                                   \
		}                                                  \
		else{                                              \
			OPERATION;                                     \
			GRUMBLE(msg)                                   \
		}                                                  \
	}

/**
 * @def ASSERT(val, OPERATION)
 *
 * @brief Checks if val is -1. If so, call OPERATION.
 * 
 * @param val int
 * @param OPERATION can be an instruction, a fonction or a macro
 */
#define ASSERT(val, OPERATION)                             \
	if (val == -1){                                        \
		OPERATION;                                         \
	}

/**
 * @def CHECK_NULL(ptr, OPERATION)
 *
 * @brief Checks if ptr is NULL. If so, call OPERATION.
 * 
 * @param ptr a pointer
 * @param OPERATION can be an instruction, a fonction or a macro
 */
#define CHECK_NULL(ptr, OPERATION)                         \
	if (ptr == NULL){                                      \
		OPERATION;                                         \
	}


// Converts a string to an int and errors out if not possible
int safe_atoi(char const* str){
	// If a user enters "test", 
	// I don't want atoi to say "test" means 0. I want an error.

	for (unsigned int i = 0; i < strlen(str); ++i)
		if (str[i] < '0' || str[i] > '9'){
			GRUMBLE("String to number conversion fail")
		}

	return atoi(str);
}

// Prints a nice reminder of how to use the program
void usage(char * const command) {
	printf("Usage: %s [-t format] [-i range] "
		   "[-l limit] [-c] prog arg ... arg\n\n", command);

	printf("Periodically executes a program and detects changes "
		   "in its output.\n\n");

	printf("Options:\n"
		   "  -i   Specify time interval (in milliseconds) between each call "
		   "(default value: 10,000 ms)\n"
		   "  -l   Specify the number of calls (default value: 0, no limit)\n"
		   "  -c   Detects changes in the return value too (default value: 0)\n"
		   "  -t   Causes the date and time of each launch to be displayed, "
		   "with the format specified, compatible with the strftime library "
		   "function (default: no display)\n"
		   "\tExample: %s -t '%%H:%%M:%%S'\n"
		   "  -h   Display this help and exit\n", command);

	exit(EXIT_FAILURE);
}

bool output_delta(int fd, Buffer* cache){
	char new = '_';
	char old = '_';
	//unsigned int i;
	bool retvalue = false;

	sFile* f = my_open(fd);
	ALLOC_NULL(f, "output_delta malloc", NULL, my_close(NULL))

	buff_reset(cache);

	while (new != EOF){
		// Compare last output with what comes out of fd
		// As we compare, we replace buff[i] with fd[i]
		new = my_getc(f);
		old = buff_getc(cache);
		if (old != new){
			retvalue = true;
		}
		
		ASSERT(buff_putc(cache, new), 
				buff_free(cache); my_close(f); GRUMBLE("buff_putc"))
	}
	buff_unputc(cache);
	my_close(f);
	
	if (retvalue){
		return true;
	}
	else {
		return false;
	}
}

void print_time(char *format){
	char buffer[BUFFT_SIZE];
	struct timeval tv;
	struct timezone tz;
	struct tm *info;

	ASSERT(gettimeofday(&tv, &tz), 
			GRUMBLE("gettimofday"))
	info = localtime(&tv.tv_sec);

	CHECK_NULL(info, 
			GRUMBLE("localtime"))

	strftime(buffer, BUFFT_SIZE, format, info);

	printf("%s\n", buffer);
}

int callProgram(char const *prog, char *const args[]){
	int tube[2];
	pid_t pid;
	
	ASSERT(pipe(tube), 
			GRUMBLE("callProgram pipe"))
	pid = fork();

	ASSERT(pid, 
			GRUMBLE("callProgram frork"))
	if (pid == 0) { //Child
		ASSERT(close(tube[0]),
				GRUMBLE("callProgram child close tube[0]"))

		ASSERT(dup2(tube[1], 1),
			GRUMBLE("callProgram child redirect stdout > tube[1]"))

		execvp(prog, args);
		ASSERT(kill(getpid(), SIGUSR1),
				GRUMBLE("kill failure child"))
	}

	//Parent
	ASSERT(close(tube[1]),
			GRUMBLE("callProgram father close tube[1]"))

	return tube[0];
}

void exit_code(bool verbose){
	int wstatus;
	static int status = -1;
	
	ASSERT(wait(&wstatus),
			GRUMBLE("wait"))

	if (!WIFEXITED(wstatus)) {
		GRUMBLE("callProgram execvp")
	}

	if (verbose)
		if (status != WEXITSTATUS(wstatus)) {
			status = WEXITSTATUS(wstatus);
			printf("exit %d\n", status);
		}

	fflush (stdout);
}

void interval(char const *prog, char *const args[], int opt_i, 
			  int opt_l, bool opt_c, bool opt_t, char* format){
	int i = 0;
	Buffer* output = NULL;
	int fd;
	int limite = (opt_l != 0);

	output = buff_new(output);

	while (!limite || i < opt_l){
		if (opt_t)
			print_time(format);

		fd = callProgram(prog, args);

		if (output_delta(fd, output))
			ASSERT(buff_print(output),
					buff_free(output); GRUMBLE("interval write to stdout fail"))

		exit_code(opt_c);

		ASSERT(close(fd),
				GRUMBLE("close fd"))
		
		i++;
		ASSERT(usleep(opt_i* CONVERT_USEC),
				GRUMBLE("usleep"))
	}
	buff_free(output);
}

void check_format (const char * optarg, char * const command) {
	char format_spec [NUMBER_OF_STRINGS][STRING_LENGTH + ZERO_TERMINATOR] = {
		"%a","%A","%b","%B","%c","%C","%d","%D","%e","%E","%F","%G","%g",
		"%h","%H","%I","%j","%k","%l","%m","%M","%n","%O","%p","%P","%r",
		"%R","%s","%S","%t","%T","%u","%U","%V","%w","%W","%x","%X","%y",
		"%Y","%Z","%+"};

	for (int i = 0; i < 42; i++) {
		if (strstr(optarg, format_spec[i]) != NULL)
			return;
	}
	
	printf ("Invalid format for -t option\n\n");
	usage(command);
}

int main(int argc, char* const argv[]){
	int option;
	bool opt_t = false;        // Time is not printed
	int opt_i = 10000;         // Default value for -i (milliseconds)
	int opt_l = 0;             // Default value for -l 
	                           // (no limit number of execution)
	bool opt_c = false;        // Change of return code is not detected
	char* format = NULL;       // -t argument
	char* prog_name = argv[0]; // programme to execute

	while ((option = getopt(argc, argv, "+:t:i:l:ch")) != -1){
		switch(option){
			case 't':
				check_format(optarg, prog_name);
				opt_t = true;
				format = optarg;
				break;
			case 'i':
				opt_i = safe_atoi(optarg);
				if (opt_i <= 0) {
					GRUMBLE("Interval nul")
				}
				break;
			case 'l':
				opt_l = safe_atoi(optarg);
				break;
			case 'c':
				opt_c = true;
				break;
			case 'h':
				usage(prog_name);
			case '?':
				fprintf(stderr, "Unknown option: %c\n\n", optopt);
				usage(prog_name);
			case ':':
				fprintf(stderr, "Option -%c takes an argument\n", optopt);
				usage(prog_name);
			/* We have already cases of ? and :
			default :
				GRUMBLE("getopt")
			*/
		}
	}

	argc -= optind;
	argv += optind;

	if (argc == 0)
		usage(prog_name);

	interval(argv[0], argv, opt_i, opt_l, opt_c, opt_t, format);

	return EXIT_SUCCESS;
}
