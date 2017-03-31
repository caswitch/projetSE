#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <wait.h>

#include "file.c"
#define BUFF_SIZE 256

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
	printf("%s [-t format] [-i intervalle] ", command);
	printf("[-l limite] [-c] prog arg ... arg\n");
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

			assert(dup2(tube[1], 1), "callProgram child redirect stdout > tube[1]");

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



int main(int argc, char * const argv[]) {
	int opt_t;
	int opt_i;
	int opt_l;
	bool opt_c;
	bool opt_h;
	int option;

	while ((option = getopt(argc, argv, "+:t:i:l:ch")) != -1) {
		switch (option) {
			case 't':
				opt_t = safe_atoi(optarg);
				printf("t=%d\n", opt_t);
				break;
			case 'i':
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

	char *const args[] = {"ls", "-l", NULL};
	char buf[BUFF_SIZE];
	int bytes_read;

	int fd = callProgram("ls", args);
	
	while ( (bytes_read = read(fd, &buf, BUFF_SIZE)) > 0)
		assert(write(1, buf, bytes_read), "callProgram father write");

	assert(close(fd), "callProgram father close tube[0]");

	return EXIT_SUCCESS;
}
