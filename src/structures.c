#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>

#include "structures.h"

#define EXIT_FAIL -1

/**
 * @def GRUMBLE(msg)
 *
 * @brief Grumbles and exits
 * @details Checks for a system call error via errno.
 * If there was one, it uses perror. 
 * Otherwise, it prints to stderr and exits.
 * 
 * This is to make sure that if you choose to grumble after 
 * something that didn't create an errno, 
 * you don't end up writing "Error: Success !" in your output.
 *
 * @param msg Error message
 */
#define GRUMBLE(msg)                                       \
	if (errno){                                            \
		perror(msg);                                       \
	}                                                      \
	else{                                                  \
		fprintf(stderr, "%s\n", msg);                      \
	}                                                      \
	exit(EXIT_FAILURE);

/**
 * @def ALLOC_NULL(alloc, msg, ptr, OPERATION)
 *
 * @brief Checks if the malloc function didi not fail.
 * @brief If so, free, grumble and exits.
 * 
 * @param alloc the return pointer of a malloc
 * @param msg Error pessage
 * @param ptr a pointer to check if is NULL or not
 * @param OPERATION can be an instruction, a fonction or a macro
 */
#define ALLOC_NULL(alloc, msg, ptr, OPERATION)             \
	if (alloc == NULL){                                    \
		if (ptr == NULL){                                  \
			GRUMBLE(msg)                                   \
		}                                                  \
		else{                                              \
			OPERATION;                                     \
			GRUMBLE(msg)                                   \
		}                                                  \
	}

/**
 * @def ASSERT(val, OPERATION)
 *
 * @brief Checks if val is -1. If so, call OPERATION.
 * 
 * @param val int
 * @param OPERATION can be an instruction, a fonction or a macro
 */
#define ASSERT(val, OPERATION)                             \
	if (val == -1){                                        \
		OPERATION;                                         \
	}

/**
 * @def CHECK_NULL(ptr, OPERATION)
 *
 * @brief Checks if ptr is NULL. If so, call OPERATION.
 * 
 * @param ptr a pointer
 * @param OPERATION can be an instruction, a fonction or a macro
 */
#define CHECK_NULL(ptr, OPERATION)                         \
	if (ptr == NULL){                                      \
		OPERATION;                                         \
	}

/**
 * @def CHECK_ZERO(val, OPERATION)
 *
 * @brief Checks if val is 0. If so, call OPERATION.
 * 
 * @param val int
 * @param OPERATION can be an instruction, a fonction or a macro
 */
#define CHECK_ZERO(val, OPERATION)                         \
	if (val == 0){                                         \
		OPERATION;                                         \
	}


void buff_free(Buffer* b){
	if (b != NULL && b->start != NULL){
		
		node* cur = b->start;
		while (cur->next != NULL){
			if (cur->prec)
				free(cur->prec);

			free(cur->mem);
			cur = cur->next;
		}
		free(cur->prec);
		free(cur->mem);
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
	CHECK_NULL(b, return EXIT_FAIL)
	CHECK_NULL(b->writeNode, return EXIT_FAIL)

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
	CHECK_NULL(b, return EXIT_FAIL) 

	// Visit each node of the list from the beginning and write it
	node* cur = b->start;
	while (cur != NULL){
		ASSERT(write(1, cur->mem, cur->writeAddr), return EXIT_FAIL)
		cur = cur->next;
	}

	return EXIT_SUCCESS;
}

s buff_getc(Buffer* b){
	CHECK_NULL(b, return EOF)
	CHECK_NULL(b->readNode, return EOF)

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
	CHECK_NULL(b, return EOF)
	CHECK_NULL(b->writeNode, return EOF)

	// If we're at the start of our node
	CHECK_ZERO(b->writeNode->writeAddr, return EOF)

	/*
		// This is correct because of buff_putc's behaviour
		// 
		// Indeed, writeAddr will never be 0 unless we're at the absolute
		// start of the buffer. It skips 0. It goes from 255 to 256, which
		// is out of bounds, detects that 256 is out of bounds, changes node,
		// then writes at 0 in the next and sets writeAddr to 1.
		// So, a mere --writeAddr is always enough !

		if (b->writeNode->prec){
			b->writeNode = b->writeNode->prec;
			return buff_unputc(b);
		}
	//*/

	return b->writeNode->mem[--b->writeNode->writeAddr];
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
			"malloc in file", f, my_close(f))
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
		ASSERT(f->length, return EXIT_FAIL);
		CHECK_ZERO(f->length, return EOF);
	}
	return f->buffer[f->index++];
}

int my_close(sFile* f){
	CHECK_NULL(f, return EXIT_FAIL)

	free(f->buffer);
	free(f);
	return 0;
}

