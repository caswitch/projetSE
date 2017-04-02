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

#include "file.c"

#define BUFF_SIZE 256
#define CONVERT_USEC 1000


int opt_t = 0;
int opt_i = 10000;
int opt_l = 0;
bool opt_c = false;
bool opt_h = true;


// Grumbles and exits
void grumble(char const * msg) {
	// Small 'if' to avoid the "Error: Success" problem
	if (errno)
		perror(msg);
	else
		fprintf(stderr, "%s\n", msg);

	exit(EXIT_FAILURE);
}

// Checks for error. If there's one, grumble and exit 
void assert(int return_value, char const * msg) {
	if (return_value == -1)
		grumble(msg);
}

// Converts a string to an int and errors out if not possible
int safe_atoi(char const* str) {
	for (unsigned int i = 0; i < strlen(str); ++i)
		if (str[i] < '0' || str[i] > '9')
			grumble("String to number conversion fail");

	return atoi(str);
}

// Prints a nice reminder of how to use the program
void usage(char * const command) {
	printf("Usage: %s [-t format] [-i intervalle] ", command);
	printf("[-l limite] [-c] prog arg ... arg\n");
	printf("Periodically starts a program and detects changes in state.\n\n");
	printf("Options:\n");
	printf("  -i    Gives the interval (in milliseconds) between two program ");
	printf("launches (default value: 10,000 milliseconds)\n");
	printf("  -l    Gives the limit of the number of launches, or 0 ");
	printf("for no limit (default value: 0, ie no limit)\n");
	printf("  -c    Also detects the return code changes, ie the exit ");
	printf("argument of the called program (default value: no acknowledgment ");
	printf("of the return code)\n");
	printf("  -t    Causes the date and time of each launch to be displayed, ");
	printf("with the format specified, compatible with the strftime library ");
	printf("function (default: no display)\n");
	printf("  -h    Display this help and exit\n");

	exit(EXIT_FAILURE);
}

void print_time(char *format) { //TODO: return char *
	char buffer[BUFF_SIZE]; //TODO: Allocation dynamique
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
	switch (pid) {
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
			assert(tube[0], "callProgram father read /tmp/tata");

			return tube[0];
	}
}

void interval (char const *prog, char *const args[]) {
	int i = 0;
	char buf[BUFF_SIZE];
	int bytes_read;
	int fd;

	if (opt_l == 0) 

	 while (1) {
		assert (usleep (opt_i * CONVERT_USEC), "usleep");

		fd = callProgram (prog, args);
		
		while ((bytes_read = read(fd, &buf, BUFF_SIZE)) > 0)
			assert(write(1, buf, bytes_read), "callProgram father write");

		assert(close(fd), "callProgram father close tube[0]");

		i++;
	 }
}

int main(int argc, char * const argv[]) {
	int option;
	int rest; // Arguments that are not options
	char *args[argc];

	while ((option = getopt(argc, argv, "+:t:i:l:ch")) != -1) {
		switch (option) {
			case 't':
				printf("t=%d\n", opt_t);
				print_time(optarg);
				break;
			case 'i':
				opt_i = *optarg;
				opt_i = safe_atoi(optarg);
				printf("i=%d\n", opt_i);
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
		args[i] = (char *) argv[optind + i];

	args[rest] = NULL;

	interval (args[0], args);
	/*
	//char *const args[] = {"ls", "-l", NULL};
	char buf[BUFF_SIZE];
	int bytes_read;

	interval (args[0], args);

	int fd = callProgram(args[0], args);
	
	while ( (bytes_read = read(fd, &buf, BUFF_SIZE)) > 0)
		assert(write(1, buf, bytes_read), "callProgram father write");

	assert(close(fd), "callProgram father close tube[0]");
	*/

	return EXIT_SUCCESS;
}
