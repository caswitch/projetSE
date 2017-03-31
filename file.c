#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct FICHIER {
	int fd; //int car vaut parfois -1
	char* buffer;
	int length; //int car vaut parfois -1
	int index; //juste int à cause des warnings (comparaisons signed/unsigned)
	unsigned int mode : 1; //un seul bit
} *FICHIER;

#define BUFFER_SIZE 512

FICHIER my_open(const char* path, const char* mode){
	if (strlen(mode) == 0)
		return NULL;
	FICHIER f = malloc(sizeof(struct FICHIER));
	
	if (f == NULL)
		return NULL;

	int flag;
	switch(mode[0]){
		case 'w':
			flag = O_WRONLY | O_TRUNC | O_CREAT;
			f->mode = 1; 
			// indique qu'on est en mode écriture
			break;
		case 'r':
			flag = O_RDONLY;
			f->mode = 0;
			break;
		default:
			free(f);
			return NULL;
	}
	f->buffer = malloc(BUFFER_SIZE * sizeof(char));
	f->fd = open(path, flag, 0666);
	//f->mode = mode[0];
	f->length = 0;
	f->index = 0;
	return f;
}

int my_getc(FICHIER f){
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

int my_ungetc(FICHIER f){
	f->index -= 2;
	if(f->index < 0){
		f->index = BUFFER_SIZE-1;
		if(lseek(f->fd, BUFFER_SIZE * (-1), SEEK_CUR) == -1)
			return EOF;
		f->length = read(f->fd, f->buffer, BUFFER_SIZE);
	}
	return f->buffer[f->index++];
}

int my_putc(int c, FICHIER f){
	//putchar bufferisé
	if (f->index >= BUFFER_SIZE){
		int s = write(f->fd, f->buffer, f->index);
		if (s <= 0)
			return s;
		f->index = 0;
	}
	f->buffer[f->index] = (char) c;
	f->index++;
	return 1;
}

int my_close(FICHIER f){
	short ret=1;
	if (f == NULL)
		return -1;
	if (f->mode == 1 && f->index > 0){
		//écrit le reste du buffer d'écriture
		ret = write(f->fd, f->buffer, f->index);
	}
	ret = ret && (close(f->fd) == 0);
	free(f->buffer);
	free(f);
	return ret;
}
