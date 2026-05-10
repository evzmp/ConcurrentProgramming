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

#define DUR_FLIGHT 5        // train ride duration
#define MAX_SIZE 30         // max train capacity

void invoke(pthread_cond_t* cond)
{
    if(!cond) return;
    pthread_cond_signal(cond);
}

void wait(pthread_cond_t* cond, pthread_mutex_t* mutex)
{
    if(!cond || !mutex) return;
    pthread_cond_wait(cond, mutex);
}

void lock(pthread_mutex_t* mutex)
{
    if(!mutex) return;
    pthread_mutex_lock(mutex);
}

void unlock(pthread_mutex_t* mutex)
{
    if(!mutex) return;
    pthread_mutex_unlock(mutex);
}

typedef struct ITrainInterface
{
    pthread_cond_t train, pass, mainDone;
    pthread_mutex_t mtx;
    int on, num, cap, queue;
}ITrainInterface;

void* trainFunc(void* arg)
{
    ITrainInterface* t = (ITrainInterface*) arg;
    if(!t) return NULL;
    while(1)
    {
        lock(&t->mtx);
        while(t->on && t->num < t->cap)
        {
            wait(&t->train, &t->mtx);
            if(!t->on) break;
        }
        if(!t->on)
        {
            if(t->queue)
                invoke(&t->pass);
            else
                invoke(&t->mainDone);
            printf("Train exiting\n");
            unlock(&t->mtx);
            break;
        }
        unlock(&t->mtx);
        printf("### Train ride start...\n");
        sleep(DUR_FLIGHT);
        printf("### Train ride end.\n");
        (t->num = 0);
        invoke(&t->pass);
    }

    return NULL;
}

void* passengerFunc(void* arg)
{
    ITrainInterface* t = (ITrainInterface*) arg;
    if(!t) return NULL;
    lock(&t->mtx);
    (t->queue)++;
    printf("Passenger %lu entering queue\n", pthread_self());
    while(t->num == t->cap)
    {
        wait(&t->pass, &t->mtx);
        if(!t->on) break;
    }
    if(!t->on)
    {
        (t->queue)--;
        if(t->queue <= 0)
            invoke(&t->mainDone);
        pthread_cond_broadcast(&t->pass);
        unlock(&t->mtx);
        printf("Passenger %lu will leave\n", pthread_self());
        return NULL;
    }
    (t->queue)--;
    (t->num)++;
    if(t->num == t->cap)
    {
        invoke(&t->train);
    }
    else
    {
        invoke(&t->pass);
    }
    printf("Passenger %lu will leave\n", pthread_self());
    unlock(&t->mtx);
    return NULL;
    
}

int main(int argc, char** argv)
{
    if(argc != 2 || atoi(argv[1]) < 0)
    {
        printf("Invalid arguments\n");
        return -1;
    }
    if(atoi(argv[1]) > MAX_SIZE)
    {
        printf("In this world we live, no train can fit more than %d passengers\n", MAX_SIZE);
        return -1;
    }

    int cap = atoi(argv[1]);
    printf("Train capacity: %d\n", cap);

    ITrainInterface ta;
    ta.cap = cap;
    ta.on = 1;
    ta.num = 0;
    ta.queue = 0;
    pthread_cond_init(&ta.train, NULL);
    pthread_cond_init(&ta.pass, NULL);
    pthread_cond_init(&ta.mainDone, NULL);

    pthread_mutex_init(&ta.mtx, NULL);

    pthread_t pass;
    pthread_t train;
    pthread_create(&train, 0, trainFunc, &ta);

    int temp = 0;
    while(1)
    {
        if(scanf("%d", &temp) == EOF) break;
        if(temp < 0)
        {
            printf("Passengers boarding can only be a positive number !\n");
            continue;
        }
        else    // create passenger
            for(int i = 0; i < temp; ++i)
                pthread_create(&pass, 0, passengerFunc, &ta);
    }
    printf("Main signaling train to finish\n");
    lock(&ta.mtx);
    ta.on = 0;
    invoke(&ta.train);
    wait(&ta.mainDone, &ta.mtx);
    unlock(&ta.mtx);
    sleep(1);       // we sleep for just 1 sec to let all output be printed
    printf("Main exiting...\n");
    return 0;
}