#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <wait.h>

#include "file.h"
#include "buff.h"
#include "assert.h"
#include "detecter.h"

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

	sFile* f = my_open(fd);

	buff_reset(cache);

	while (new != EOF){
		// Compare last output with what comes out of fd
		// As we compare, we replace buff[i] with fd[i]
		new = my_getc(f);
		old = buff_getc(cache);
		if (old != new){
			retvalue = true;
		}
		
		buff_putc(cache, new);
	}
	buff_unputc(cache);
	my_close(f);
	
	if (retvalue){
		return cache;
	}
	else {
		return NULL;
	}
}

void print_time(char *format){
	char buffer[BUFFT_SIZE];
	struct timeval tv;
	struct timezone tz;
	struct tm *info;

	assert(gettimeofday(&tv, &tz), "gettimofday");
	info = localtime(&tv.tv_sec);

	if (info == NULL)
		grumble("localtime");

	strftime(buffer, BUFFT_SIZE, format, info);

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
			if (buff_print(output) == -1){
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
	char* format = NULL;
	char* prog_name = argv[0];

	while ((option = getopt(argc, argv, "+:t:i:l:ch")) != -1){
		switch(option){
			case 't':
				opt_t = true;
				format = optarg;
				break;
			case 'i':
				opt_i = safe_atoi(optarg);
				if (opt_i <= 0)
					grumble("Intervalle nul");
				break;
			case 'l':
				opt_l = safe_atoi(optarg);
				break;
			case 'c':
				opt_c = true;
				break;
			case 'h':
				usage(prog_name);
				break;
			case '?':
				fprintf(stderr, "Unknown option: %c\n", optopt);
				usage(prog_name);
				break;
			case ':':
				fprintf(stderr, "Option -%c takes an argument\n", optopt);
				usage(prog_name);
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
