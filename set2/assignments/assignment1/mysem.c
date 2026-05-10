
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <errno.h>
#include "mysem.h"

void per(const char* func, const char* msg)
{
    if(!func || !msg) return;
    fprintf(stderr, "%s(): ", func);
    perror(msg);
    exit(-1);
}

int mysem_init(sMysem* s, int n)
{
    if(n < 0)
        return 0;
    if(s && s->active == 1)
    {
        printf("Semaphore %d is already initialized\n", s->num);
        return -1;      // already initialized
    }
    int get = semget(IPC_PRIVATE, 1, S_IRWXU);
    if(get == -1)
        per(__func__, "semget");
    int ret = semctl(get, 0, SETVAL, n);
    if(ret == -1)
        per(__func__, "semctl");
    s->num = get;
    s->active = 1;
    return 1;
}

int mysem_down(sMysem* s)
{
    if(!s)
    {
        printf("%s(): Null semaphore provided!\n", __func__);
        return -2;
    }
    if(s->active == 0)
    {
        printf("Semaphore %d is not initialized\n", s->num);
        return -1;
    }
    struct sembuf op;
    op.sem_num = 0;
    op.sem_op = -1;
    op.sem_flg = 0;
    int get = semop(s->num, &op, 1);
    if(get == -1)
        per(__func__, "semop");
    return 1;
}

int mysem_up(sMysem* s)
{
    if(!s)
    {
        printf("%s(): Null semaphore provided!\n", __func__);
        return -2;
    }
    if(s->active == 0)
    {
        printf("Semaphore %d is not initialized\n", s->num);
        return -1;
    }
    int get = semctl(s->num, 0, GETVAL);
    if(get == -1)
        per(__func__, "semctl");
    else if(get == 1)
        return 0;
    struct sembuf op;
    op.sem_num = 0;
    op.sem_op = 1;
    op.sem_flg = 0;
    get = semop(s->num, &op, 1);
    if(get == -1)
        per(__func__, "semop");
    return 1;
}

int mysem_destroy(sMysem* s)
{
    if(!s)
    {
        printf("%s(): Null semaphore provided!\n", __func__);
        return -2;
    }
    if(s->active == 0)
    {
        printf("Semaphore %d is not initialized\n", s->num);
        return -1;
    }
    int get = semctl(s->num, 0, IPC_RMID);
    if(get == -1)
        per(__func__, "semctl");
    s->active = 0;
    return 1;
}

int mysem_print(sMysem* s)
{
    if(!s)
    {
        printf("%s(): Null semaphore provided!\n", __func__);
        return -2;
    }
    int get = semctl(s->num, 0, GETVAL);
    if(get == -1)
        per(__func__, "semctl");
    printf("Semaphore (group %d) = %d\n", s->num, get);
    return 1;
}