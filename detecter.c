#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <wait.h>

#define BUFF_SIZE 256
#define CONVERT_USEC 1000
#define READ 0
#define WRITE 1

#include "file.c"
#include "buff.c"
//#include "detecter.h"

// Grumbles and exits
void grumble(char* msg){
	// Small 'if' to avoid the "Error: Success" problem
	if (errno)
		perror(msg);
	else
		fprintf(stderr, "%s\n", msg);

	exit(EXIT_FAILURE);
}

// Checks for error. If there's one, grumble and exit 
void assert(int return_value, char* msg){
	if (return_value == -1)
		grumble(msg);
}

// Converts a string to an int and errors out if not possible
int safe_atoi(char const* str){
	// If a user enters "test", 
	// I don't want atoi to say "test" means 0. I want an error.

	for (unsigned int i = 0; i < strlen(str); ++i)
		if (str[i] < '0' || str[i] > '9')
			grumble("String to number conversion fail");

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

Buffer* output_delta(int fd, Buffer* cache){
	char new = '_';
	char old = '_';
	//unsigned int i;
	bool retvalue = false;

	FICHIER f = my_fdtof(fd, 0);

	buff_reset(cache);

	while (new != EOF){
		// Compare last output with what comes out of fd
		// As we compare, we replace buff[i] with fd[i]
		new = my_getc(f);
		old = buff_getc(cache);
		if (old != new){
			retvalue = true;
		}
		
		cache = buff_putc(cache, new);
	}
	buff_unputc(cache);
	
	if (retvalue){
		return cache;
	}
	else {
		return NULL;
	}
}

void print_time(char *format){
	char buffer[BUFF_SIZE];
	struct timeval tv;
	struct timezone tz;
	struct tm *info;

	assert(gettimeofday(&tv, &tz), "gettimofday");
	info = localtime(&tv.tv_sec);

	if (info == NULL)
		grumble("localtime");

	strftime(buffer, BUFF_SIZE, format, info);

	printf("%s\n", buffer);
}

int callProgram(char const *prog, char *const args[]){
	//int status;
	//int devNull;
	int tube[2];
	
	assert(pipe(tube), "callProgram pipe");
	
	int pid = fork();
	switch(pid){
		case 0:
			//Case enfant
			assert(close(tube[0]), "callProgram child close tube[0]");
			assert(tube[1], "callProgram child write tube[1]");

			assert(dup2(tube[1], 1), 
				"callProgram child redirect stdout > tube[1]");

			execvp(prog, args);
			grumble("callProgram execlp");
		case -1:
			//Case erreur
			grumble("callProgram fork");
		default:
			//Case parent
			assert(close(tube[1]), "callProgram father close tube[1]");

			return tube[0];
	}
}

void exit_code(int i){
	static int wstatus_old;
	int wstatus;

	if (i == 0) {
		assert(wait(&wstatus_old), "wait");
		wstatus_old = WEXITSTATUS(wstatus_old);
		printf("exit %d\n", wstatus_old);
	}
	else {
		assert(wait(&wstatus), "wait");
		if (WIFEXITED(wstatus) && WEXITSTATUS(wstatus) != wstatus_old){
			wstatus_old = WEXITSTATUS(wstatus);
			printf("exit %d\n", wstatus_old);
		}
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
		output = output_delta(fd, output);

		if (output != NULL)
			if (write(1, buff_toString(output), buff_getSize(output)) == -1){
				buff_free(output);
				grumble("interval write to stdout fail");
			}

		if (opt_c)
			exit_code(i);

		assert(close(fd), "close fd");
		
		i++;
		assert(usleep(opt_i* CONVERT_USEC), "usleep");
	}
	buff_free(output);
}

int main(int argc, char* const argv[]){
	int option;
	// Globals
	bool opt_t = false; // Time is not printed
	int opt_i = 10000;
	int opt_l = 0;
	bool opt_c = false;
	char* format = false;
	char* prog_name = argv[0];

	while ((option = getopt(argc, argv, "+:t:i:l:ch")) != -1){
		switch(option){
			case 't':
				opt_t = true;
				format = optarg;
#ifdef DEBUG
					printf("t=%d\n", opt_t);
					print_time(optarg);
#endif
				break;
			case 'i':
				opt_i = safe_atoi(optarg);
#ifdef DEBUG
					printf("i=%d\n", opt_i);
#endif
				break;
			case 'l':
				opt_l = safe_atoi(optarg);
#ifdef DEBUG
					printf("l=%d\n", opt_l);
#endif
				break;
			case 'c':
				opt_c = true;
#ifdef DEBUG
					printf("c=%d\n", opt_c);
#endif
				break;
			case 'h':
				usage(prog_name);
				break;
			case '?':
				printf("Unknown option: %c\n", optopt);
				break;
			case ':':
				printf("Option -%c takes an argument\n", optopt);
				break;
			default :
				grumble("getopt");
				break;
		}
	}


	argc -= optind;
	argv += optind;

#ifdef DEBUG
		printf("rest = %d\n", rest); //CHOU
#endif

	if (argc == 0)
		usage(prog_name);

	interval(argv[0], argv, opt_i, opt_l, opt_c, opt_t, format);

	return EXIT_SUCCESS;
}
