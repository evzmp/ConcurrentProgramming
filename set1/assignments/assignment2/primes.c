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

typedef struct worker_props
{
    int value, on, waiting;
}worker_props; 

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
    worker_props* t = (worker_props*) arg;
    if(!t) return NULL;
    while(t->on)
    {   
        if(t->waiting) continue;
        if(isPrime(t->value))
            printf("%d: 1\n", t->value);
        else
            printf("%d: 0\n", t->value);
        t->waiting = 1;
    }
    fflush(stdout);
    return NULL;
}

int main(int argc, char** argv)
{
    if(argc != 2) return -1;
    int num_workers = atoi(argv[1]);;

    struct timeval t1, t2;  // for time benchmark
    double elapsed;

    worker_props* worker_jobs = malloc(num_workers*sizeof(worker_props));   // array containing each worker's personal properties and state variables
    for(int i = 0; i < num_workers; ++i)
    {
        worker_jobs[i].value = 255;
        worker_jobs[i].on = 1;
        worker_jobs[i].waiting = 1;
    }
    pthread_t* workers = malloc(num_workers*sizeof(pthread_t));     // array containing the actual workers tids
    

    for(int j = 0; j < num_workers; ++j)
        pthread_create(&workers[j], NULL, calc, &worker_jobs[j]);

    gettimeofday(&t1, NULL);

    int temp = 0, scannext = 0;
    while(scanf(" %d", &temp) != EOF)
    {
        scannext = 0;
        while(1)
        {
            for(int j = 0; j < num_workers; ++j)
            {
                if(worker_jobs[j].waiting)
                {
                    worker_jobs[j].value = temp;
                    worker_jobs[j].waiting = 0;
                    scannext = 1;
                    break;
                }
            }
            if(scannext) break;
        }
    }

    int finished = 0;
    while(finished < num_workers)               // wait for all workers to finish their tasks and then make them exit their routine
        for(int j = 0; j < num_workers; ++j)
            if(worker_jobs[j].waiting && worker_jobs[j].on)     // waiting beyond this point is useless because there is 
            {                                                   // no more work to be given to any worker
                worker_jobs[j].on = 0;
                finished++;
            }
    gettimeofday(&t2, NULL);

    elapsed = (t2.tv_sec - t1.tv_sec);
    elapsed += (t2.tv_usec - t1.tv_usec) / 1000000.0;

    printf("%f s elapsed\n", elapsed);
    free(workers);
    free(worker_jobs);
    sleep(1);                   // we sleep for just 1 sec to let all output be printed
    
    return 0;
}