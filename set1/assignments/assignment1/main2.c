/* University of Thessaly
Dept of Electrical & Computer Engineering
Concurrent Programming 2021-2022
Evan Zampras - ezampras at uth dot gr
Evangelos Balamotis - ebalamotis at uth dot gr

For an explanation of the problems see the pdf file */

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "pipes.h"

typedef struct s_thread_arg
{
    int pipe, thread_done;
    FILE* input, *copy;
}s_thread_arg;

void* t1(void* arg)     // read threads
{
    s_thread_arg* t = (s_thread_arg*) arg;
    if(!t || !(t->input))
        return NULL;
    printf( "Thread %lu starting (file -> pipe) (pipe %d)\n", pthread_self(), t->pipe);
    char c;
    int out = 255, r = 0;
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

    t->thread_done = 1;
    printf( "Thread %lu exiting\n", pthread_self());

    return NULL;
}

void* t2(void* arg)     // write threads
{
    s_thread_arg* t = (s_thread_arg*) arg;
    if(!t || !(t->copy))
        return NULL;
    printf( "Thread %lu starting (pipe -> file) (pipe %d)\n", pthread_self(), t->pipe);
    char c;
    int out = 255;
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
    printf( "Thread %lu finished writing pipe (%d) into output file\n", pthread_self(), t->pipe);
    t->thread_done = 1;
    printf( "Thread %lu exiting\n", pthread_self());

    return NULL;
}

int main(int argc, char** argv)
{

    FILE* f1 = fopen("input1", "r");
    FILE* f2 = fopen("input2", "r");
    FILE* f3 = fopen("input3", "r");
    FILE* f4 = fopen("input4", "r");
    FILE* f5 = fopen("input5", "r");
    FILE* f6 = fopen("output1", "w");
    FILE* f7 = fopen("output2", "w");
    FILE* f8 = fopen("output3", "w");
    FILE* f9 = fopen("output4", "w");
    FILE* f10 = fopen("output5", "w");

    pthread_t threads[10];
    s_thread_arg thread_args[10];

    pipe_init();
    // create 5 pipes of varying sizes
    int p1 = pipe_open(1025);
    int p2 = pipe_open(19);
    int p3 = pipe_open(167);
    int p4 = pipe_open(60);
    int p5 = pipe_open(333);

    // read 1
    thread_args[0].input = f1;
    thread_args[0].pipe = p1;
    thread_args[0].thread_done = 0;
    // write 1
    thread_args[1].copy = f6;
    thread_args[1].pipe = p1;
    thread_args[1].thread_done = 0;
    // read 2
    thread_args[2].input = f2;
    thread_args[2].pipe = p2;
    thread_args[2].thread_done = 0;
    // write 2
    thread_args[3].copy = f7;
    thread_args[3].pipe = p2;
    thread_args[3].thread_done = 0;
    // read 3
    thread_args[4].input = f3;
    thread_args[4].pipe = p3;
    thread_args[4].thread_done = 0;
    // write 3
    thread_args[5].copy = f8;
    thread_args[5].pipe = p3;
    thread_args[5].thread_done = 0;
    // read 4
    thread_args[6].input = f4;
    thread_args[6].pipe = p4;
    thread_args[6].thread_done = 0;
    // write 4
    thread_args[7].copy = f9;
    thread_args[7].pipe = p4;
    thread_args[7].thread_done = 0;
    // read 5
    thread_args[8].input = f5;
    thread_args[8].pipe = p5;
    thread_args[8].thread_done = 0;
    // write 5
    thread_args[9].copy = f10;
    thread_args[9].pipe = p5;
    thread_args[9].thread_done = 0;

    // 5 threads read, 5 threads write
    pthread_create(&threads[0], NULL, t1, &thread_args[0]);
    pthread_create(&threads[1], NULL, t2, &thread_args[1]);
    pthread_create(&threads[2], NULL, t1, &thread_args[2]);
    pthread_create(&threads[3], NULL, t2, &thread_args[3]);
    pthread_create(&threads[4], NULL, t1, &thread_args[4]);
    pthread_create(&threads[5], NULL, t2, &thread_args[5]);
    pthread_create(&threads[6], NULL, t1, &thread_args[6]);
    pthread_create(&threads[7], NULL, t2, &thread_args[7]);
    pthread_create(&threads[8], NULL, t1, &thread_args[8]);
    pthread_create(&threads[9], NULL, t2, &thread_args[9]);

    while(1)
    {
        if(thread_args[0].thread_done && thread_args[1].thread_done 
            && thread_args[2].thread_done && thread_args[3].thread_done
            && thread_args[4].thread_done && thread_args[5].thread_done
            && thread_args[6].thread_done && thread_args[7].thread_done
            && thread_args[8].thread_done && thread_args[9].thread_done)   // wait for all threads to finish
            break;
    }

    fclose(f1);
    fclose(f2);
    fclose(f3);
    fclose(f4);
    fclose(f5);
    fclose(f6);
    fclose(f7);
    fclose(f8);
    fclose(f9);
    fclose(f10);
    return 0;
}