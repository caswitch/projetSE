#ifndef __DETECTER__H_
#define __DETECTER__H_

void grumble(char * msg); 

void assert(int return_value, char * msg) ;

int safe_atoi(char const* str);

void usage(char * const command); 

Buffer buff_putc(Buffer b, char c);

int buff_getc(Buffer b);

bool buff_setpos(Buffer b, bool mode, unsigned int pos);

bool buff_reset(Buffer b);

Buffer output_delta(int fd);

void print_time(char *format);

int callProgram(char const *prog, char *const args[]);

void exit_code (int i);

void interval (char const *prog, char *const args[], int opt_i, int opt_l, bool opt_c, bool opt_t, char * format);



#endif
