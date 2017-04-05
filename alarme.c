#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int opt_i = 4;

void interval () {
	 int i = 0;

	 while (i <= opt_i) {
		usleep (opt_i * 10000000);
		printf ("hi\n");
		i++;
	 }
}

int main (void) {
	interval ();
}
