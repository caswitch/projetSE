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
		//printf("\n%p -> %p -> %p", b->readNode->prec, b->readNode, b->readNode->next);
		//free(b->readNode->prec);
	}
	free(b);

	return;
}

node* node_new(){
	node* n = malloc(sizeof(struct s_node));
	n->mem = malloc(sizeof(s) * BUFF_SIZE);
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

Buffer* buff_putc(Buffer* b, char c){
	if (b == NULL)
		return NULL;

	if (b->writeNode == NULL)
		return b;

	if (b->writeNode->writeAddr >= b->writeNode->size){
		b->writeNode->next = node_new();
		b->writeNode->next->prec = b->writeNode;
		b->writeNode = b->writeNode->next;
	}

	b->writeNode->mem[b->writeNode->writeAddr++] = c;
	b->length++;
	return b;
}

int buff_getSize(Buffer* b){
	if (b == NULL)
		return 0;
	return b->length;
}


int buff_print(Buffer* b){
	if (b == NULL)
		return -1;

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

	if (b->readNode->readAddr >= b->readNode->size){
		b->readNode = b->readNode->next;
		return buff_getc(b);
	}

	return b->readNode->mem[b->readNode->readAddr++];
}

char buff_unputc(Buffer* b){
	if (b == NULL)
		return EOF;

	if (b->writeNode == NULL)
		return EOF;

	if (b->writeNode->writeAddr == 0){
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

	node* cur = b->start;
	while (cur != NULL){
		cur->readAddr = 0;
		cur->writeAddr = 0;
		cur->size = 0;
		cur = cur->next;
	}
}
