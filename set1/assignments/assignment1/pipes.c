
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "circlebuf.h"
#include "pipes.h"

volatile int pipes_count = 0;
volatile s_pipe_props pipes[MAX_PIPES];

int tas(volatile int* v)
{
    int old = *v;
    *v = 1;
    return old;
}

void pipe_init()
{
    for(int i = 0; i < MAX_PIPES; ++i)
    {
        pipes[i].status = 0;
        pipes[i].read = 0;
        pipes[i].write = 0;
        pipes[i].turn = 0;
        pipes[i].id = -1;
        pipes[i].buf = NULL;
    }
}

int pipe_find(int p)
{
    for(int i = 0; i < MAX_PIPES; ++i)
        if(pipes[i].id == p)
            return i;
    return -1;
}

int pipe_destroy(int p)
{
    int k = -1;
    if((k = pipe_find(p)) == -1)    // pipe not found
        return -1;
    pipes[k].id = -1;
    cbuf_free(pipes[k].buf);
    pipes[k].status = 0;
    pipes_count--;
    return 0;
}

int pipe_open(int size)
{
    if(size < 0 || pipes_count == MAX_PIPES) return -1;  // invalid input or maximum allowed pipes reached
    int i = 0;
    for(i = 0; i < MAX_PIPES; ++i)
        if(pipes[i].id == -1)    // we found a position to open a pipe
            break;
    
    pipes[i].buf = cbuf_init(size, 1);
    pipes[i].id = i;
    pipes[i].status = 1;
    pipes_count++;
    return i;
}

int pipe_writeDone(int p)
{
    int k = pipe_find(p);
    if(k == -1)                 // if pipe is not found
        return -1;
    while(pipes[k].buf->count)
    {
       // cbuf_print(pipes[k].buf);
        continue;
    }
    pipes[k].status = 0;
    return 1;
}

int pipe_read(int p, char* c)
{
    int k = pipe_find(p);   
    if(k == -1)                 // if pipe is not found
        return -1;

    while(1)
    {
        if(pipes[k].status == 0)
            return 0;

        pipes[k].read = 1;
        pipes[k].turn = 1;
        while(pipes[k].write) { if(pipes[k].turn == 0) break; }
        if(cbuf_get(pipes[k].buf, (char*) c, 1) == -1)
        {
            pipes[k].read = 0;
            continue;
        }
        else break;
    }
    return 1;
}

int pipe_write(int p, char c)
{
    int k = pipe_find(p);
    if(k == -1)
        return -1;
    while(1)
    {
        if(pipes[k].status == 0)
            return -1;
        
        pipes[k].write = 1;
        pipes[k].turn = 0;
        while(pipes[k].read) { if(pipes[k].turn == 1) break; }
        if(cbuf_add(pipes[k].buf, (char*) &c, 1) == -1)
        {
            pipes[k].write = 0;
            continue;
        }
        else break;
    }
    pipes[k].write = 0;
    return 1;
}
