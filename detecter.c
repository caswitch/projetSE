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
void usage(char* const command){
	printf("Usage: %s [-t format] [-i range] ", command);
	printf("[-l limit] [-c] prog arg ... arg\n");
	printf("Periodically "
			"executes a program and detects changes in its output.\n\n");
	printf("Options:\n");
	printf("  -i    Specify time interval(in milliseconds) between each call");
	printf("(default value: 10,000 ms)\n");
	printf("  -l    Specify the number of calls");
	printf("(default value: 0(no limit))\n");
	printf("  -c    Detects changes in the return value too");
	printf("(default value: 0)");
	printf("  -t    Causes the date and time of each launch to be displayed, ");
	printf("with the format specified, compatible with the strftime library ");
	printf("function(default: no display)\n");
	printf("Example: %s -t '%%H:%%M:%%S'\n", command);
	printf("  -h    Display this help and exit\n");

	exit(EXIT_FAILURE);
}

Buffer* output_delta(int fd){
	static Buffer* cache = NULL;
	char new = '_';
	char old = '_';
	//unsigned int i;
	bool retvalue = false;

	FICHIER f = my_fdtof(fd, 0);

	buff_reset(cache);

	while (new != EOF || old != EOF){
		// Compare last output with what comes out of fd
		// As we compare, we replace buff[i] with fd[i]
		new = my_getc(f);
		old = buff_getc(cache);

		if (old != new)
			retvalue = true;

		cache = buff_putc(cache, new);
	}
	
	if (retvalue){
		return cache;
	} else{
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
			// WHAT ???
			assert(tube[0], "callProgram father read /tmp/tata");

			return tube[0];
	}
}

void exit_code(int i){
	static int wstatus_old;
	int wstatus;

	if (i == 0){
		assert(wait(&wstatus_old), "wait");
		wstatus_old = WEXITSTATUS(wstatus_old);
		printf("exit %d\n", wstatus_old);
	}
	else{
		assert(wait(&wstatus), "wait");
		if (WIFEXITED(wstatus) && WEXITSTATUS(wstatus) != wstatus_old){
			wstatus_old = WEXITSTATUS(wstatus);
			printf("exit %d\n", wstatus_old);
		}
	}

}

void interval(char const *prog, 
			  char *const args[], 
			  int opt_i, 
			  int opt_l, 
			  bool opt_c, 
			  bool opt_t, 
			  char* format){
	int i = 0;
	Buffer* output;
	int fd;
	int limite;

	if (opt_l == 0)
		limite = 0;

	 while (!limite || i < opt_l){
		assert(usleep(opt_i* CONVERT_USEC), "usleep");

		if (opt_t)
			print_time(format);

		fd = callProgram(prog, args);
		output = output_delta(fd);

		if (output != NULL)
			assert(
				write(1, buff_toString(output), buff_getSize(output)-3), 
			"interval write stdout");

		if (opt_c)
			exit_code(i);

		printf("\n");
		
		

		i++;
	 }
}

int main(int argc, char* const argv[]){
	int option;
	int rest; // Arguments that are not options
	char *args[argc];
	// Globals
	bool opt_t = false; // Time is not printed
	int opt_i = 10000;
	int opt_l = 0;
	bool opt_c = false;
	bool opt_h = true;
	//char* prog_name;
	char* format = false;

	//	prog_name = argv[0];

	while ((option = getopt(argc, argv, "+:t:i:l:ch")) != -1){
		switch(option){
			case 't':
				opt_t = true;
				format = optarg;
				/*
				printf("t=%d\n", opt_t);
				print_time(optarg);
				*/
				break;
			case 'i':
				//opt_i = *optarg;
				opt_i = safe_atoi(optarg);
				//printf("i=%d\n", opt_i);
				break;
			case 'l':
				opt_l = safe_atoi(optarg);
				printf("l=%d\n", opt_l);
				break;
			case 'c':
				opt_c = true;
				printf("c=%d\n", opt_c);
				break;
			case 'h':
				opt_h = true;
				usage(argv[0]);
				printf("h=%d\n", opt_h);
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

	rest = argc - optind;

	printf("rest = %d\n", rest); //CHOU

	if (rest == 0)
		usage(argv[0]);

	for (int i = 0; i < rest; i++)
		args[i] =(char *) argv[optind + i];

	args[rest] = NULL;
	printf("\n");

	interval(args[0], args, opt_i, opt_l, opt_c, opt_t, format);
	printf("\n");

	return EXIT_SUCCESS;
}
