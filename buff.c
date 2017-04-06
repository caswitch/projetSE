typedef struct s_buff{
	unsigned int size;
	unsigned int readAddr;
	unsigned int writeAddr;
	char* mem;
} Buffer;

void buff_free(Buffer* b){
	if (b == NULL)
		return;
	free(b->mem);
	free(b);
	return;
}

Buffer* buff_putc(Buffer* b, char c){
	if (b == NULL){
		b = malloc(sizeof(struct s_buff));
		b->mem = malloc(BUFF_SIZE);
		b->size = BUFF_SIZE;
		b->readAddr = 0;
		b->writeAddr = 0;
	}

	if (b->writeAddr >= b->size){
		b->size += BUFF_SIZE;
		b->mem = realloc(b->mem, b->size);
	}

	b->mem[b->writeAddr] = c;
	b->writeAddr++;
	return b;
}

int buff_getSize(Buffer* b){
	if (b == NULL)
		return 0;
	return b->writeAddr;
}

char* buff_toString(Buffer* b){
	if (b == NULL)
		return "";
	return b->mem;
}

int buff_getc(Buffer* b){
	if (b == NULL)
		return EOF;

	if (b->readAddr >= b->size)
		return EOF;

	return b->mem[b->readAddr++];
}

void buff_unputc(Buffer* b){
	b->writeAddr--;
}

bool buff_setpos(Buffer* b, bool mode, unsigned int pos){
	if (b == NULL)
		return false;

	if (mode == WRITE)
		b->writeAddr = pos;
	else
		b->readAddr = pos;

	return true;
}

bool buff_reset(Buffer* b){
	return buff_setpos(b, 0, 0) && buff_setpos(b, 1, 0);
}
