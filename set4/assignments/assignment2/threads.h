
#pragma once
#include <signal.h>
#include "../assignment1/crt.h"

typedef struct thr_t
{
    co_t* cot;
    int status; // 0 = ready, 1 = blocked, 2 = finished
}thr_t;

typedef struct s_Sched
{
    void* sig_handler;
    thr_t** threads;
    thr_t* main_thread;
    int index, thread_count;
}s_Sched;

extern s_Sched* g_sched;
//extern co_t* g_main_cot;
void HandleSignal(int signum);

void SchedSwitch(void);

int mythreads_init();
int mythreads_create(thr_t *thr, void (body)(void *), void *arg);
int mythreads_yield();
int mythreads_join(thr_t *thr);
int mythreads_destroy(thr_t *thr);
// int mythreads_tuple_out(char *fmt, ...);
// int mythreads_tuple_in(char *fmt, ...);
