
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "circlebuf.h"


s_cbuf* cbuf_init(int size, int item_size)
{
    if(size < 1 || item_size < 1) return NULL;
    s_cbuf* t = (s_cbuf*) malloc(sizeof(s_cbuf));
    t->start = malloc(size*item_size);
    if(!t->start) return NULL;
    t->size = size;
    t->item_size = item_size;
    t->end = t->start + t->size;
    t->head = t->tail = t->start;
    t->count = 0;
    return t;
}

void cbuf_free(s_cbuf* cbuffer)
{
    if(!cbuffer) return;
    if(cbuffer->start) free(cbuffer->start);
    cbuffer->head = cbuffer->tail = cbuffer->start = cbuffer->end = NULL;
    cbuffer->size = cbuffer->item_size = cbuffer->count = 0;
    free(cbuffer);
    cbuffer = NULL;
}

int cbuf_add(s_cbuf* cbuffer, const void* item, int item_size)
{
    if(!cbuffer || !item || item_size < 1) return -2;
    if(cbuffer->count == cbuffer->size) return -1;
    if(cbuffer->head == cbuffer->end)
        cbuffer->head = cbuffer->start;
    memcpy(cbuffer->head, item, item_size);
    cbuffer->head++;
    cbuffer->count++;
    return 0;
}

int cbuf_get(s_cbuf* cbuffer, void* item, int item_size)
{
    if(!cbuffer || !item || item_size < 1) return -2;
    if(cbuffer->count == 0) return -1;
    if(cbuffer->tail == cbuffer->end)
        cbuffer->tail = cbuffer->start;
    memcpy(item, cbuffer->tail, item_size);
    cbuffer->tail++;
    cbuffer->count--;
    return 0;
}

void cbuf_print(const s_cbuf* cbuffer)
{
    if(!cbuffer) return;
    for(char* i = cbuffer->tail; i < (char*) cbuffer->head; ++i)
        printf("%c", *i);
    putchar('\n');
}

