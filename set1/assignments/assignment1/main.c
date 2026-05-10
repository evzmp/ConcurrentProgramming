/* University of Thessaly
Dept of Electrical & Computer Engineering
Concurrent Programming 2021-2022
Evan Zampras - ezampras at uth dot gr
Evangelos Balamotis - ebalamotis at uth dot gr

For an explanation of the problems see the pdf file */

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "pipes.h"

typedef struct s_thread_arg
{
    int pipe, pipe2, done;
    FILE* input, *copy;
}s_thread_arg;

void* t1(void* arg)     // thread 1
{
    s_thread_arg* t = (s_thread_arg*) arg;
    if(!t)
        return NULL;
    printf( "Thread %lu starting\n", pthread_self());
    char c;
    int out = 255, r = 1;

    // first we read from input (f1) and put into pipe p1
    while(1)
    {
        r = read(fileno(t->input), &c, 1);
        if(r == 0)
        {
            printf( "Thread %lu finished reading input file into pipe (%d)\n", pthread_self(), t->pipe);
            pipe_writeDone(t->pipe);
            break;
        }
        
        out = pipe_write(t->pipe, c);
        if(out == -1) break;
    }

    // then we read from pipe p2 and put into second copy (f3)
    while(1)
    {
        out = pipe_read(t->pipe2, &c);
        if(out == -1) break;
        else if(out == 0)
        {
            pipe_destroy(t->pipe2);
            break;
        }
        write(fileno(t->copy), &c, 1);
    }

    printf( "Thread %lu finished writing pipe (%d) into second copy file\n", pthread_self(), t->pipe2);
    t->done = 1;
    printf( "Thread %lu exiting\n", pthread_self());

    return NULL;
}

void* t2(void* arg)     // thread 2
{
    s_thread_arg* t = (s_thread_arg*) arg;
    if(!t)
        return NULL;
    printf( "Thread %lu starting\n", pthread_self());
    char c;
    int out = 255, r = 1;

    // first we read from pipe p1 and put into copy (f2)
    while(1)
    {
        out = pipe_read(t->pipe, &c);
        if(out == -1) break;
        else if(out == 0)
        {
            pipe_destroy(t->pipe);
            break;
        }
        write(fileno(t->copy), &c, 1);
    }
    printf( "Thread %lu finished writing pipe (%d) into copy file\n", pthread_self(), t->pipe);

    rewind(t->copy);
    // then we read from copy (f2) and put into pipe p2
    while(1)
    {
        r = read(fileno(t->input), &c, 1);
        if(r == 0)
        {
            printf( "Thread %lu finished reading copy file into pipe (%d)\n", pthread_self(), t->pipe2);
            pipe_writeDone(t->pipe2);
            break;
        }
        out = pipe_write(t->pipe2, c);
        if(out == -1) break;
    }
    t->done = 1;
    printf( "Thread %lu exiting\n", pthread_self());

    return NULL;
}

int main(int argc, char** argv)
{
    FILE* f1 = fopen("input1", "r");
    if(f1 == NULL)
    {
        printf("Error: %s\n", strerror(errno));
        return 0;
    }
    FILE* f2 = fopen("output1", "w+");
    if(f2 == NULL)
    {
        printf("Error: %s\n", strerror(errno));
        return 0;
    }
    FILE* f3 = fopen("output2", "w");
    if(f3 == NULL)
    {
        printf("Error: %s\n", strerror(errno));
        return 0;
    }

    pthread_t threads[2];
    s_thread_arg thread_args[2];

    pipe_init();
    int p1 = pipe_open(64);
    int p2 = pipe_open(64);

    // 1
    thread_args[0].input = f1;
    thread_args[0].copy = f3;
    thread_args[0].pipe = p1;
    thread_args[0].pipe2 = p2;
    thread_args[0].done = 0;
    // 2
    thread_args[1].input = f2;
    thread_args[1].copy = f2;
    thread_args[1].pipe = p1;
    thread_args[1].pipe2 = p2;
    thread_args[1].done = 0;

    pthread_create(&threads[0], NULL, t1, &thread_args[0]);
    pthread_create(&threads[1], NULL, t2, &thread_args[1]);

    while(!thread_args[0].done && !thread_args[1].done);

    fclose(f1);
    fclose(f2);
    fclose(f3);
    return 0;
}