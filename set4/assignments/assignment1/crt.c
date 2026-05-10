
#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>
#include "crt.h"

void PrintError(const char* caller, const char* msg)
{
    if(caller && msg)
        printf("Error: %s(): %s\n", caller, msg);
}

int mycoroutines_init(co_t *main)
{
    main->ctx = (ucontext_t *) malloc(sizeof(ucontext_t));
    if(!main->ctx)
    {
        PrintError(__func__, "Malloc error");
        return -1;
    }

    getcontext(main->ctx);
    main->ctx->uc_stack.ss_sp = main->stack;
    main->ctx->uc_stack.ss_size = sizeof(main->stack);
    return 0;
}

int mycoroutines_create(co_t *co, void (body)(void*), void* arg)
{
    co->ctx = (ucontext_t *) malloc(sizeof(ucontext_t));
    if(!co->ctx)
    {
        PrintError(__func__, "Malloc error");
        return -1;
    }

    if(getcontext(co->ctx) == -1)
    {
        PrintError(__func__, "getcontext() returned -1");
        return -1;
    }
    co->ctx->uc_stack.ss_sp = co->stack;
    co->ctx->uc_stack.ss_size = sizeof(co->stack);
    makecontext(co->ctx, (void(*)(void))body, 1, arg);
    return 0;
}

int mycoroutines_switchto(co_t *co)
{
    if(!co)
    {
        PrintError(__func__, "Invalid coroutine");
        return -1;
    }
    if( setcontext(co->ctx) == -1)
    {
        PrintError(__func__, "setcontext() returned -1");
        return -1;
    }
    return 0;
}

int mycoroutines_destroy(co_t *co)
{
    if(co->ctx) free(co->ctx);
    if(co) free(co);
    printf("Destroyed coroutine\n");
    return 0;
}