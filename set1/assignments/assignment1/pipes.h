
#pragma once
#include "circlebuf.h"
#define MAX_PIPES 1024

typedef struct s_pipe_props
{
    s_cbuf* buf;
    volatile int status, id, read, write, turn;
}s_pipe_props;
volatile extern int pipes_count;
volatile extern s_pipe_props pipes[MAX_PIPES];

int tas(volatile int* v);
void pipe_init();
int pipe_find(int p);
int pipe_destroy(int p);
int pipe_open(int size);
int pipe_write(int p, char c);
int pipe_writeDone(int p);
int pipe_read(int p, char* c);