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
#include "../assignment1/mysem.h"

#define MAX_INPUT_TIME 60       // max delay allowed before next car arrives
#define TIME_TO_CROSS 5         // time in seconds for cars to cross bridge

typedef struct sBridgeProps
{
    int lq, rq, usl, usr, cap, turn, on, fair, cars;
    sMysem* mtx, *gl, *gr, *end;
}sBridgeProps;

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

    mysem_down(b->mtx);
    (b->cars)++;
    mysem_up(b->mtx);
    mysem_down(b->mtx);
    if(t->dir == 0)
    {
        printf("Car %lu entering left\n", pthread_self());
        (b->lq)++;
        while(1)
        {
            if(b->usr || (b->usl && b->usl == b->cap) || (b->turn == 1))
            {
                mysem_up(b->mtx);
                mysem_down(b->gl);
                continue;
            }
            if(((b->usl + b->usr) == 0 && b->turn == 0) || (b->usl && b->usl < b->cap && b->turn == 0))
            {
                (b->usl)++;
                if(b->lq && b->usl < b->cap) mysem_up(b->gl);
                mysem_up(b->mtx);
                printf("Car %lu crossing >>>>>>>>\n", pthread_self());
                sleep(TIME_TO_CROSS);
                printf("Car %lu exiting ---->>>>\n", pthread_self());
                mysem_down(b->mtx);
                (b->fair)++;
                (b->lq)--;
                (b->usl)--;
                if(b->fair == b->cap && b->rq != 0 && b->lq == 0) {  b->fair = 0; b->turn = 1; mysem_up(b->gr); }
                if(b->fair == b->cap && b->rq == 0 && b->lq != 0) {  b->fair = 0; b->turn = 0; mysem_up(b->gl); }
                if(b->fair == b->cap && b->rq && b->lq) { b->fair = 0; b->turn = 1; mysem_up(b->gr); }
                if(b->fair != b->cap && b->rq == 0 && b->lq != 0) {  b->fair = 0; b->turn = 0; mysem_up(b->gl); }
                if(b->fair != b->cap && b->rq != 0 && b->lq == 0) { ; b->fair = 0; b->turn = 1; mysem_up(b->gr); }
                if(b->fair != b->cap && b->rq && b->lq && b->turn == 1) {  mysem_up(b->gr); }
                (b->cars)--;
                mysem_up(b->mtx);
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
            if(b->usl || (b->usr && b->usr == b->cap) || (b->turn == 0))
            {
                mysem_up(b->mtx);
                mysem_down(b->gr);
                continue;
            }
            if(((b->usl + b->usr) == 0 && b->turn == 1) || (b->usr && b->usr < b->cap && b->turn == 1))
            {
                (b->usr)++;
                if(b->rq && b->usr < b->cap) mysem_up(b->gr);
                mysem_up(b->mtx);
                printf("Car %lu crossing <<<<<<<<\n", pthread_self());
                sleep(TIME_TO_CROSS);
                printf("Car %lu exiting <<<<----\n", pthread_self());
                mysem_down(b->mtx);
                (b->fair)++;
                (b->rq)--;
                (b->usr)--;
                if(b->fair == b->cap && b->lq != 0 && b->rq == 0) { b->fair = 0; b->turn = 0; mysem_up(b->gl); }
                if(b->fair == b->cap && b->lq == 0 && b->rq != 0) { b->fair = 0; b->turn = 1; mysem_up(b->gr); }
                if(b->fair == b->cap && b->lq && b->rq ) { b->fair = 0; b->turn = 0; mysem_up(b->gl); }
                if(b->fair != b->cap && b->rq == 0 && b->lq != 0) { b->fair = 0; b->turn = 0; mysem_up(b->gl); }
                if(b->fair != b->cap && b->rq != 0 && b->lq == 0) { b->fair = 0; b->turn = 1; mysem_up(b->gr); }
                if(b->fair != b->cap && b->rq && b->lq && b->turn == 0) { mysem_up(b->gl); }
                (b->cars)--;
                mysem_up(b->mtx);
                break;
            }
        }
    }
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
    bridge.mtx = malloc(sizeof(sMysem));
    bridge.gr = malloc(sizeof(sMysem));
    bridge.gl = malloc(sizeof(sMysem));
    bridge.end = malloc(sizeof(sMysem));
    mysem_init(bridge.mtx, 1);
    mysem_init(bridge.gl, 0);
    mysem_init(bridge.gr, 0);
    mysem_init(bridge.end, 0);

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
    printf("Left\n");
    while(bridge.cars);

    mysem_destroy(bridge.mtx);
    mysem_destroy(bridge.gl);
    mysem_destroy(bridge.gr);
    mysem_destroy(bridge.end);
    free(bridge.mtx);
    free(bridge.gl);
    free(bridge.gr);
    free(bridge.end);
    return 0;
}