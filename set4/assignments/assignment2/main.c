#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "threads.h"

void f1(void* arg)
{
    printf("Thread 1\n");
    sleep(2);
    return;
}

void f2(void* arg)
{
    printf("Thread 2\n");
    sleep(3);
    return;
}

int main(void)
{
    thr_t thread1, thread2;
    mythreads_init();
    mythreads_create(&thread1, (void (*)(void*)) f1, NULL);
    mythreads_create(&thread2, (void (*)(void*)) f2, NULL);

    mythreads_yield();
    mythreads_destroy(&thread1);
    mythreads_destroy(&thread2);


    return 0;
}