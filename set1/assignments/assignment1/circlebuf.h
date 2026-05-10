
#pragma once

typedef struct s_cbuf
{
    int size, item_size, count;
    void* start, *end, *head, *tail;
}s_cbuf;

s_cbuf* cbuf_init(int size, int item_size);
void cbuf_free(s_cbuf* cbuffer);
int cbuf_add(s_cbuf* cbuffer, const void* item, int item_size);
int cbuf_get(s_cbuf* cbuffer, void* item, int item_size);
void cbuf_print(const s_cbuf* cbuffer);