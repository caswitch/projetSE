#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "file.h"

#define BUFFER_SIZE 512

FICHIER my_open(int fd){	
	FICHIER f = malloc(sizeof(struct FICHIER));

	if (f == NULL)
		return NULL;

	f->buffer = malloc(BUFFER_SIZE * sizeof(char));
	//f->mode = mode[0];
	f->length = 0;
	f->index = 0;
	f->fd = fd;
	f->mode = 0;

	return f;
}

char my_getc(FICHIER f){
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

int my_close(FICHIER f){
	short ret=1;
	if (f == NULL)
		return -1;
	if (f->mode == 1 && f->index > 0){
		//écrit le reste du buffer d'écriture
		ret = write(f->fd, f->buffer, f->index);
	}
	ret = ret || (close(f->fd) == 0);
	free(f->buffer);
	free(f);
	return ret;
}
