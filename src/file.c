#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "file.h"

sFile* my_open(int fd){	
	sFile* f = malloc(sizeof(struct s_file));

	if (f == NULL)
		return NULL;

	f->buffer = malloc(BUFFER_SIZE * sizeof(char));
	//f->mode = mode[0];
	f->length = 0;
	f->index = 0;
	f->fd = fd;

	return f;
}

char my_getc(sFile* f){
	if(f->length == 0 || f->index >= f->length){
		f->length = read(f->fd, f->buffer, BUFFER_SIZE);
		f->index = 0;
		if (f->length == -1)
			return -1;
		if (f->length == 0)
			return EOF;
	}
	return f->buffer[f->index++];
}

int my_close(sFile* f){
	if (f == NULL)
		return -1;

	free(f->buffer);
	free(f);
	return 0;
}
