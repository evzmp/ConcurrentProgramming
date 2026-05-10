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

typedef struct sWorkerProps
{
    pthread_cond_t cond, leave;
    pthread_mutex_t mutex, lmutex;
    int on, value, working, empty;
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
        pthread_mutex_lock(&t->mutex);
        while(t->empty)
            pthread_cond_wait(&t->cond, &t->mutex);
        if(!(t->on)) break;
        temp = t->value;
        t->empty = 1;
        pthread_cond_broadcast(&t->cond);
        pthread_mutex_unlock(&t->mutex);
        if(isPrime(temp))
            printf("Thread %lu calculates %d: 1\n", pthread_self(), temp);
        else
            printf("Thread %lu calculates %d: 0\n", pthread_self(), temp);
    }
    
    (t->working)--;
    pthread_cond_signal(&t->cond);
    pthread_mutex_unlock(&t->mutex);
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
    w.empty = 1;
    pthread_cond_init(&w.cond, NULL);
    pthread_mutex_init(&w.mutex, NULL);

    pthread_t* workers = malloc(num_workers*sizeof(pthread_t));     // array containing the actual workers tids
    for(int j = 0; j < num_workers; ++j)
        pthread_create(&workers[j], NULL, calc, &w);

    gettimeofday(&t1, NULL);

    int temp = 0;
    while(scanf("%d", &temp) != EOF)
    {
        pthread_mutex_lock(&w.mutex);
        while(!w.empty)
            pthread_cond_wait(&w.cond, &w.mutex);
        w.value = temp;
        w.empty = 0;
        pthread_cond_signal(&w.cond);
        pthread_mutex_unlock(&w.mutex);
    }
    w.empty = 0;
    w.on = 0;
    pthread_cond_signal(&w.cond);

    pthread_mutex_lock(&w.mutex);
    while(w.working)
        pthread_cond_wait(&w.cond, &w.mutex);
    pthread_mutex_unlock(&w.mutex);

    gettimeofday(&t2, NULL);

    elapsed = (t2.tv_sec - t1.tv_sec);
    elapsed += (t2.tv_usec - t1.tv_usec) / 1000000.0;

    printf("%f s elapsed\n", elapsed);
    free(workers);

    sleep(1);   // we sleep for just 1 sec to let all output be printed
    
    return 0;
}