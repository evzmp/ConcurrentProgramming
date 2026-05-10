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

#define MAX_INPUT_TIME 60       // max delay allowed before next car arrives
#define TIME_TO_CROSS 5         // time in seconds for cars to cross bridge

typedef struct sBridgeProps
{
    int lq, rq, usl, usr, cap, turn, on, fair, cars;
    pthread_mutex_t mutex;
    pthread_cond_t leftCanPass, rightCanPass, leave;
}sBridgeProps;

void invoke(pthread_cond_t* cond)
{
    if(!cond) return;
    pthread_cond_signal(cond);
}

typedef struct sCarProps
{
    int dir, time;
    sBridgeProps* bridgeRef;
}sCarProps;

void* carFunc(void* arg)
{
    sCarProps* t = (sCarProps*) arg;
    if(!t) return NULL;
    sBridgeProps* b = t->bridgeRef;
    if(t->time) sleep(t->time);

    pthread_mutex_lock(&b->mutex);
    (b->cars)++;
    if(t->dir == 0)
    {
        printf("Car %lu entering left\n", pthread_self());
        (b->lq)++;
        while(1)
        {
            while(b->usr || (b->usl && b->usl == b->cap) || (b->turn == 1))
            {
                pthread_cond_wait(&b->leftCanPass, &b->mutex);
            }
            if(((b->usl + b->usr) == 0 && b->turn == 0) || (b->usl && b->usl < b->cap && b->turn == 0))
            {
                (b->usl)++;
                if(b->lq && b->usl < b->cap)
                    invoke(&b->leftCanPass);
                pthread_mutex_unlock(&b->mutex);
                printf("Car %lu crossing >>>>>>>>\n", pthread_self());
                sleep(TIME_TO_CROSS);
                printf("Car %lu exiting ---->>>>\n", pthread_self());
                pthread_mutex_lock(&b->mutex);
                (b->fair)++;
                (b->lq)--;
                (b->usl)--;
                if(b->fair == b->cap && b->rq != 0 && b->lq == 0) {  b->fair = 0; b->turn = 1; invoke(&b->rightCanPass); }
                if(b->fair == b->cap && b->rq == 0 && b->lq != 0) {  b->fair = 0; b->turn = 0; invoke(&b->leftCanPass); }
                if(b->fair == b->cap && b->rq && b->lq) { b->fair = 0; b->turn = 1; invoke(&b->rightCanPass); }
                if(b->fair != b->cap && b->rq == 0 && b->lq != 0) {  b->fair = 0; b->turn = 0; invoke(&b->leftCanPass); }
                if(b->fair != b->cap && b->rq != 0 && b->lq == 0) { ; b->fair = 0; b->turn = 1; invoke(&b->rightCanPass); }
                if(b->fair != b->cap && b->rq && b->lq && b->turn == 1) {  invoke(&b->rightCanPass); }
                (b->cars)--;
                pthread_mutex_unlock(&b->mutex);
                break;
            }
        }
    }
    else
    {
        printf("Car %lu entering right\n", pthread_self());
        (b->rq)++;
        while(1)
        {
            while(b->usl || (b->usr && b->usr == b->cap) || (b->turn == 0))
            {
                pthread_cond_wait(&b->rightCanPass, &b->mutex);
            }
            if(((b->usl + b->usr) == 0 && b->turn == 1) || (b->usr && b->usr < b->cap && b->turn == 1))
            {
                (b->usr)++;
                if(b->rq && b->usr < b->cap)
                    invoke(&b->rightCanPass);
                pthread_mutex_unlock(&b->mutex);
                printf("Car %lu crossing <<<<<<<<\n", pthread_self());
                sleep(TIME_TO_CROSS);
                printf("Car %lu exiting <<<<----\n", pthread_self());
                pthread_mutex_lock(&b->mutex);
                (b->fair)++;
                (b->rq)--;
                (b->usr)--;
                if(b->fair == b->cap && b->lq != 0 && b->rq == 0) { b->fair = 0; b->turn = 0; invoke(&b->leftCanPass);  }
                if(b->fair == b->cap && b->lq == 0 && b->rq != 0) { b->fair = 0; b->turn = 1; invoke(&b->rightCanPass);}
                if(b->fair == b->cap && b->lq && b->rq ) { b->fair = 0; b->turn = 0; invoke(&b->leftCanPass); }
                if(b->fair != b->cap && b->rq == 0 && b->lq != 0) { b->fair = 0; b->turn = 0; invoke(&b->leftCanPass);  }
                if(b->fair != b->cap && b->rq != 0 && b->lq == 0) { b->fair = 0; b->turn = 1; invoke(&b->rightCanPass); }
                if(b->fair != b->cap && b->rq && b->lq && b->turn == 0) { invoke(&b->leftCanPass); }
                (b->cars)--;
                pthread_mutex_unlock(&b->mutex);
                break;
            }
        }
    }
    if(!b->cars)
        pthread_cond_signal(&b->leave);
    free(t);
    return NULL;
}

int main(int argc, char** argv)
{
    if(argc != 2 || atoi(argv[0]) < 0)
    {
        printf("Invalid arguments\n");
        return -1;
    }
    
    int bridgeCapacity = atoi(argv[1]);
    pthread_t car;

    sBridgeProps bridge;
    bridge.lq = bridge.rq = bridge.usl = bridge.usr = bridge.fair = bridge.turn = bridge.cars = 0;
    bridge.on = 1;
    bridge.cap = bridgeCapacity;
    pthread_cond_init(&bridge.leftCanPass, NULL);
    pthread_cond_init(&bridge.rightCanPass, NULL);
    pthread_cond_init(&bridge.leave, NULL);
    pthread_mutex_init(&bridge.mutex, NULL);

    int dir, eta;
    while(1)
    {
        if(scanf(" %d %d", &dir, &eta) == EOF) break;
        if((dir != 0 && dir != 1))
        {
            printf("Invalid car info, format: <direction (0: left, 1: right)> <ETA (seconds)>\n");
            continue;
        }
        sCarProps* myCar = malloc(sizeof(sCarProps));
        myCar->bridgeRef = &bridge;
        myCar->dir = dir;
        myCar->time = eta;
        pthread_create(&car, 0, carFunc, myCar);
    }
    bridge.on = 0;
    if(bridge.cars)
        pthread_cond_wait(&bridge.leave, &bridge.mutex);
    printf("Left\n");

    return 0;
}