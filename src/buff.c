#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

#include "buff.h"
#include "assert.h"

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
	node* n = malloc(sizeof(struct s_node));

	if (n == NULL)
		grumble ("malloc node");

	n->mem = malloc(sizeof(s) * BUFF_SIZE);

	if (n == NULL)
		grumble ("malloc array in node");

	n->size = BUFF_SIZE;
	n->prec = NULL;
	n->next = NULL;
	n->readAddr = 0;
	n->writeAddr = 0;
	return n;
}

Buffer* buff_new(){
	Buffer* b = malloc(sizeof(struct s_buff));

	b->length = 0;
	b->readNode = node_new();
	b->writeNode = b->readNode;
	b->start = b->readNode;

	return b;
}

int buff_putc(Buffer* b, char c){
	if (b == NULL)
		return -1;

	if (b->writeNode == NULL)
		return -1;

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
	b->length++;
	return 0;
}

/*
int buff_getSize(Buffer* b){
	if (b == NULL)
		return 0;
	return b->length;
}
*/

int buff_print(Buffer* b){
	if (b == NULL)
		return -1;

	// Visit each node of the list from the beginning and write it
	node* cur = b->start;
	while (cur != NULL){
		if (write(1, cur->mem, cur->writeAddr) == -1)
			return -1;
		cur = cur->next;
	}

	return 0;
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

	b->length = 0;
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
