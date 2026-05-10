
#include <stdio.h>
#include <stdlib.h>
#include "threads.h"
#include "../assignment1/crt.h"

s_Sched* g_sched;

void SchedSwitch(void)
{
    if(g_Scheduler->index < (g_Scheduler->thread_count)-1)
        g_Scheduler->index += 1;
    else
        g_Scheduler->index = 0;
    mycoroutines_switchto(g_Scheduler->threads[g_Scheduler->index]->cot);
}

void HandleSignal(int signum)
{
    if(signum == 2)
        printf("Got SIGINT\n");
}

int mythreads_init()
{
    g_sched = malloc(sizeof(s_Sched));
    if(!g_sched)
    {
        PrintError(__func__, "Malloc error");
        return -1;
    }
    g_sched->sig_handler = HandleSignal;
    g_sched->main_thread = malloc(sizeof(thr_t));
    g_sched->main_thread->status = 0;
    g_sched->main_thread->cot = malloc(sizeof(co_t));
    g_sched->main_thread->cot->ctx = (ucontext_t *) malloc(sizeof(ucontext_t));
    if(!g_sched->main_thread->cot->ctx)
    {
        PrintError(__func__, "Malloc error");
        return -1;
    }

    getcontext(g_sched->main_thread->cot->ctx);
    g_sched->main_thread->cot->ctx->uc_stack.ss_sp = g_sched->main_thread->cot->stack;
    g_sched->main_thread->cot->ctx->uc_stack.ss_size = sizeof(g_sched->main_thread->cot->stack);
    return 0;
}

int mythreads_create(thr_t *thr, void (body)(void *), void *arg)
{
    thr = malloc(sizeof(thr_t));
    if(!thr)
    {
        PrintError(__func__, "Malloc error");
        return -1;
    }
    thr->cot = malloc(sizeof(co_t));
    if(!thr->cot)
    {
        PrintError(__func__, "Malloc error");
        return -1;
    }
    mycoroutines_create(thr->cot, body, arg);
    thr->status = 0;
    if(g_sched->thread_count == 0)
    {
        g_sched->threads = malloc(sizeof(thr_t*));
        if(!(g_sched->threads))
        {
            PrintError(__func__, "Malloc error");
            return -1;
        }
        g_sched->threads[0] = thr;
        g_sched->thread_count += 1;
    }
    else
        g_sched->threads = realloc(g_sched->threads, (g_sched->thread_count + 1)*sizeof(thr_t*));
        g_sched->threads[g_sched->thread_count] = thr;
        g_sched->thread_count += 1;
    }
    return 0;
}

int mythreads_yield()
{
    SchedSwitch();
    return 0;
}

int mythreads_join(thr_t *thr)
{
    while(thr->status != 2);
    return 0;
}

int mythreads_destroy(thr_t *thr)
{
    mycoroutines_destroy(thr->cot);
    thr->status = 2;
    free(thr);
    return 0;
}

// int mythreads_tuple_out(char *fmt, ...);
// int mythreads_tuple_in(char *fmt, ...);