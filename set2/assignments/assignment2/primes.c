/* University of Thessaly
Dept of Electrical & Computer Engineering
Concurrent Programming 2021-2022
Evan Zampras - ezampras at uth dot gr
Evangelos Balamotis - ebalamotis at uth dot gr

For an explanation of the problems see the pdf file */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include "../assignment1/mysem.h"

typedef struct sWorkerProps
{
    sMysem* mtx, *empty, *full;
    int on, value, working;
}sWorkerProps;

int isPrime(int num)
{
    int flag = 1;
    for(int i = 2; i <= num / 2; i++)
       if(num % i == 0)
       {
          flag = 0;
          break;
       }
    return flag;
}

void* calc(void* arg)
{
    sWorkerProps* t = (sWorkerProps*) arg;
    if(!t) return NULL;
    printf("Thread %lu starting\n", pthread_self());
    int temp = 0;

    while(t->on)
    {
        mysem_down(t->full);
        if(!(t->on)) { mysem_up(t->full); break; }
        mysem_down(t->mtx);
        temp = t->value;
        mysem_up(t->mtx);
        mysem_up(t->empty);
        if(isPrime(temp))
            printf("Thread %lu calculates %d: 1\n", pthread_self(), temp);
        else
            printf("Thread %lu calculates %d: 0\n", pthread_self(), temp);
    }

    mysem_down(t->mtx);
    (t->working)--;
    mysem_up(t->mtx);
    printf("Thread %lu exiting\n", pthread_self());
    return NULL;
}

int main(int argc, char** argv)
{
    if(argc != 2)
    {
        printf("Invalid argument\n");
        return -1;
    }
    int num_workers = atoi(argv[1]);;

    struct timeval t1, t2;  // for time benchmark
    double elapsed;

    sWorkerProps w;     // the interface between main and worker threads
    w.on = 1;
    w.value = 255;
    w.working = num_workers;
    w.mtx = malloc(sizeof(sMysem));
    w.empty = malloc(sizeof(sMysem));
    w.full = malloc(sizeof(sMysem));
    mysem_init(w.mtx, 1);
    mysem_init(w.empty, 1);
    mysem_init(w.full, 0);

    pthread_t* workers = malloc(num_workers*sizeof(pthread_t));     // array containing the actual workers tids
    
    for(int j = 0; j < num_workers; ++j)
        pthread_create(&workers[j], NULL, calc, &w);

    gettimeofday(&t1, NULL);

    int temp = 0;
    while(scanf("%d", &temp) != EOF)
    {
        mysem_down(w.empty);
        mysem_down(w.mtx);
        w.value = temp;
        mysem_up(w.mtx);
        mysem_up(w.full);
    }
    w.on = 0;
    mysem_up(w.mtx);
    mysem_up(w.full);

    while(w.working);           // wait for all working threads to exit normally

    gettimeofday(&t2, NULL);

    elapsed = (t2.tv_sec - t1.tv_sec);
    elapsed += (t2.tv_usec - t1.tv_usec) / 1000000.0;

    printf("%f s elapsed\n", elapsed);
    free(workers);

    mysem_destroy(w.mtx);
    mysem_destroy(w.full);
    mysem_destroy(w.empty);
    free(w.mtx);
    free(w.full);
    free(w.empty);
    sleep(1);                   // we sleep for just 1 sec to let all output be printed
    
    return 0;
}