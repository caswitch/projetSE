#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "assert.h"

// Grumbles and exits
void grumble(char* msg){
	// Small 'if' to avoid the "Error: Success" problem
	if (errno)
		perror(msg);
	else
		fprintf(stderr, "%s\n", msg);

	exit(1);
}

// Checks for error. If there's one, grumble and exit 
void assert(int return_value, char* msg){
	if (return_value == -1)
		grumble(msg);
}
