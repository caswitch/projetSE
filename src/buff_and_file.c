#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>

#include "buff_and_file.h"
#include "assert.h"

#define EXIT_FAIL -1

// Small 'if' to avoid the "Error: Success" problem
#define GRUMBLE(msg)                                       \
	if (errno){                                            \
		perror(msg);                                       \
	}                                                      \
	else{                                                  \
		fprintf(stderr, "%s\n", msg);                      \
	}                                                      \
	exit(EXIT_FAILURE);

#define ALLOC_NULL(alloc, msg, n, OPERATION)               \
	if (alloc == NULL){                                    \
		if (n == NULL){                                    \
			GRUMBLE(msg)                                   \
		}                                                  \
		else{                                              \
			OPERATION;                                     \
			GRUMBLE(msg)                                   \
		}                                                  \
	}

#define PTR_NULL(ptr, val)                                 \
	if (ptr == NULL){                                      \
		return val;                                        \
	}

#define CHECK_VAL(a, val, ret)                                 \
	if (a == val){                                         \
		return ret;                                        \
	}


void buff_free(Buffer* b){
	if (b != NULL){
		node* cur = b->start;
		while (cur != NULL){
			free(cur->mem);
			cur = cur->next;
			//printf("\n%p -> %p -> %p", cur->prec, cur, cur->next);
			if (cur)
				free(cur->prec);
		}
		free(cur);
		free(b);
	}

	return;
}

node* node_new(){
	node* n;
	ALLOC_NULL((n = malloc(sizeof(struct s_node))),
			"malloc of node", NULL, free(NULL));

	ALLOC_NULL((n->mem = malloc(sizeof(s) * BUFF_SIZE)),
			"malloc in node", n, free(n))

	n->size = BUFF_SIZE;
	n->prec = NULL;
	n->next = NULL;
	n->readAddr = 0;
	n->writeAddr = 0;

	return n;
}

Buffer* buff_new(){
	Buffer* b;
	ALLOC_NULL((b = malloc(sizeof(struct s_buff))),
			"malloc of buffer", NULL, buff_free(NULL))

	b->readNode = node_new();
	b->writeNode = b->readNode;
	b->start = b->readNode;

	return b;
}

int buff_putc(Buffer* b, char c){
	PTR_NULL(b, EXIT_FAIL)
	PTR_NULL(b->writeNode, EXIT_FAIL)

	// If there's no more room in the node,
	if (b->writeNode->writeAddr >= b->writeNode->size){
		// Create a new node, link this node to it
		if (b->writeNode->next == NULL){
			b->writeNode->next = node_new();
			b->writeNode->next->prec = b->writeNode;
		}
		// Link the new node to the current
		// Hop to the next node
		b->writeNode = b->writeNode->next;
	}

	b->writeNode->mem[b->writeNode->writeAddr++] = c;
	return EXIT_SUCCESS;
}

int buff_print(Buffer* b){
	PTR_NULL(b, EXIT_FAIL) 

	// Visit each node of the list from the beginning and write it
	node* cur = b->start;
	while (cur != NULL){
		CHECK_VAL(write(1, cur->mem, cur->writeAddr),
				EXIT_FAIL, EXIT_FAIL)
		cur = cur->next;
	}

	return EXIT_SUCCESS;
}

s buff_getc(Buffer* b){
	PTR_NULL(b, EOF)
	PTR_NULL(b->readNode, EOF)

	// If we're at the end of our node
	if (b->readNode->readAddr >= b->readNode->size){
		// And there is more
		if (b->readNode->next){
			// We pick the next node and call ourselves again
			b->readNode = b->readNode->next;
			return buff_getc(b);
		}
		else {
			return EOF;
		}
	}

	return b->readNode->mem[b->readNode->readAddr++];
}

s buff_unputc(Buffer* b){
	PTR_NULL(b, EOF)
	PTR_NULL(b->writeNode, EOF)

	// If we're at the start of our node
	if (b->writeNode->writeAddr == 0){
		// We pick the last one and call ourselves again
		b->writeNode = b->writeNode->prec;
		return buff_unputc(b);
	}

	//printf("%d/%d\n", b->writeNode->writeAddr, b->writeNode->size);
	return b->writeNode->mem[b->writeNode->writeAddr--];
}

void buff_reset(Buffer* b){
	if (b != NULL){
		b->readNode = b->start;
		b->writeNode = b->start;

		// Go through each node and reset it
		node* cur = b->start;
		while (cur != NULL){
			cur->readAddr = 0;
			cur->writeAddr = 0;
			cur = cur->next;
		}
	}
}

sFile* my_open(int fd){	
	sFile* f;
	ALLOC_NULL((f = malloc(sizeof(struct s_file))),
			"malloc of file", NULL, my_close(NULL))

	ALLOC_NULL((f->buffer = malloc(BUFFER_SIZE * sizeof(char))),
			"malloc in file", f, my_close(NULL))
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
		CHECK_VAL(f->length, EXIT_FAIL, EXIT_FAIL);
		CHECK_VAL(f->length, 0, EOF);
	}
	return f->buffer[f->index++];
}

int my_close(sFile* f){
	PTR_NULL(f, EXIT_FAIL)

	free(f->buffer);
	free(f);
	return 0;
}

