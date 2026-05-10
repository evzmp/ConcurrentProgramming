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

#define DUR_FLIGHT 5        // train ride duration
#define MAX_SIZE 30         // max train capacity

typedef struct ITrainInterface
{
    sMysem* trainGo, *passBoard, *nMutex, *mainExit;
    int on, num, cap, queue;
}ITrainInterface;

void* trainFunc(void* arg)
{
    ITrainInterface* t = (ITrainInterface*) arg;
    if(!t) return NULL;
    while(1)
    {
        printf("%lu: Train waiting to start\n", pthread_self());
        mysem_down(t->trainGo);
        if(!(t->on) && t->queue < t->cap) { mysem_up(t->passBoard); break; }

        printf("%lu: Train ride start\n", pthread_self());
        sleep(DUR_FLIGHT);
        printf("%lu: Train ride end\n", pthread_self());

        mysem_down(t->nMutex);
        t->num = 0;
        mysem_up(t->nMutex);
        mysem_up(t->passBoard);     // signal to blocked passengers that they can board
    }
    printf("%lu: Main has signaled to exit, AND there are no more pending rides -> train exiting\n", pthread_self());
    mysem_up(t->mainExit);          // signal main that train is done
    return NULL;
}

void* passengerFunc(void* arg)
{
    ITrainInterface* t = (ITrainInterface*) arg;
    if(!t) return NULL;

    mysem_down(t->nMutex);
    (t->queue)++;
    printf("Passenger %lu waiting to board..., queue now %d\n", pthread_self(), t->queue);
    mysem_up(t->nMutex);
    mysem_down(t->passBoard);   // wait for boarding
    if(!(t->on) && (t->queue) < t->cap - t->num)
    {
        mysem_down(t->nMutex);
        (t->queue)--;
        mysem_up(t->nMutex);
        mysem_up(t->passBoard);
        return NULL;
    }
    
    mysem_down(t->nMutex);
    (t->num)++;
    (t->queue)--;
    printf("Passenger %lu boarded, now train has %d passengers\n", pthread_self(), t->num);
    mysem_up(t->nMutex);

    if(t->num == t->cap) mysem_up(t->trainGo);      // if train full, last passenger invokes train ride
    else mysem_up(t->passBoard);                    // else allow next passenger to board

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
    ta.nMutex = malloc(sizeof(sMysem));
    ta.passBoard = malloc(sizeof(sMysem));
    ta.trainGo = malloc(sizeof(sMysem));
    ta.mainExit = malloc(sizeof(sMysem));
    mysem_init(ta.nMutex, 1);
    mysem_init(ta.passBoard, 1);
    mysem_init(ta.trainGo, 0);
    mysem_init(ta.mainExit, 0);

    pthread_t pass;
    pthread_t train;
    pthread_create(&train, 0, trainFunc, &ta);

    int temp = 0;
    while(1)
    {
        if(scanf("%d", &temp) == EOF) break;
        if(temp < 0 || temp > cap)
        {
            printf("Passengers boarding can be > 0 and < train capacity\n");
            continue;
        }
        else    // create passenger
            for(int i = 0; i < temp; ++i)
                pthread_create(&pass, 0, passengerFunc, &ta);
    }
    printf("Main is now signaling train (and remaining passengers) to exit, as all pending full rides have been completed\n");
    ta.on = 0;
    mysem_up(ta.trainGo);       // unstuck train to force it to check if its done
    mysem_down(ta.mainExit);    // main gets stuck until train says its done
    while(ta.queue);            // wait until all passenger threads have exited normally

    mysem_destroy(ta.nMutex);
    mysem_destroy(ta.passBoard);
    mysem_destroy(ta.trainGo);
    mysem_destroy(ta.mainExit);
    free(ta.nMutex);
    free(ta.passBoard);
    free(ta.trainGo);
    free(ta.mainExit);

    return 0;
}