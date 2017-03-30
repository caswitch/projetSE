#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>


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
int tonumber(char const* str) {
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
				opt_t = tonumber(optarg);
				printf("t=%d\n", opt_t);
				break;
			case 'i':
				opt_i = tonumber(optarg);
				printf("i=%d\n", opt_i);
				break;
			case 'l':
				opt_l = tonumber(optarg);
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

	return EXIT_SUCCESS;
}
