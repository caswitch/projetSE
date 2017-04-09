#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>

#include "buff.h"
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

#define NODE_NULL(alloc, msg, n)                           \
	if (alloc == NULL){                                    \
		if (n == NULL){                                    \
			GRUMBLE(msg)                                   \
		}                                                  \
		else{                                              \
			free(n);                                       \
			GRUMBLE(msg)                                   \
		}                                                  \
	}


void buff_free(Buffer* b){
	if (b == NULL)
		return;

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

	return;
}

node* node_new(){
	node* n;
	NODE_NULL((n = malloc(sizeof(struct s_node))), "malloc of node", NULL);

	NODE_NULL((n->mem = malloc(sizeof(s) * BUFF_SIZE)), "malloc in node", n)

	n->size = BUFF_SIZE;
	n->prec = NULL;
	n->next = NULL;
	n->readAddr = 0;
	n->writeAddr = 0;
	return n;
}

Buffer* buff_new(){
	Buffer* b = malloc(sizeof(struct s_buff));

	b->readNode = node_new();
	b->writeNode = b->readNode;
	b->start = b->readNode;

	return b;
}

int buff_putc(Buffer* b, char c){
	if (b == NULL)
		return EXIT_FAIL;

	if (b->writeNode == NULL)
				return EXIT_FAIL;

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
	if (b == NULL)
		return EXIT_FAIL;

	// Visit each node of the list from the beginning and write it
	node* cur = b->start;
	while (cur != NULL){
		if (write(1, cur->mem, cur->writeAddr) == EXIT_FAIL)
			return EXIT_FAIL;
		cur = cur->next;
	}

	return EXIT_SUCCESS;
}

char buff_getc(Buffer* b){
	if (b == NULL)
		return EOF;
	
	if (b->readNode == NULL)
		return EOF;

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

char buff_unputc(Buffer* b){
	if (b == NULL)
		return EOF;

	if (b->writeNode == NULL)
		return EOF;

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
	if (b == NULL)
		return;

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
