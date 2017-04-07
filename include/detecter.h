#ifndef __DETECTER__H_
#define __DETECTER__H_

int safe_atoi(char const* str);

void usage(char * const command);

Buffer* output_delta(int fd, Buffer* cache);

void print_time(char *format);

int callProgram(char const *prog, char *const args[]);

void exit_code(int i);

void interval(char const *prog, char *const args[], int opt_i, 
			  int opt_l, bool opt_c, bool opt_t, char* format);

#endif
