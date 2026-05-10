
#pragma once
#include <ucontext.h>
#define STACKSIZE 16384

// coroutines library

typedef struct co_t
{
    ucontext_t* ctx;
    char stack[STACKSIZE];
}co_t;

typedef struct s_FunctionArgs
{
    char* buf;
    co_t* reader, *writer, *main;
    int in, out, fend, written;
}s_FunctionArgs;


int mycoroutines_init(co_t *main);
int mycoroutines_create(co_t *co, void (body)(void*), void* arg);
int mycoroutines_switchto(co_t *co); 
int mycoroutines_destroy(co_t *co);

void PrintError(const char* caller, const char* msg);