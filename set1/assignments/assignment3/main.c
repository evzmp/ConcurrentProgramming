/* University of Thessaly
Dept of Electrical & Computer Engineering
Concurrent Programming 2021-2022
Evan Zampras - ezampras at uth dot gr
Evangelos Balamotis - ebalamotis at uth dot gr

For an explanation of the problems see the pdf file */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "merge.h"

int main(void)
{
    FILE* f = fopen("in", "r");
    int sz = fsize(f);
    printf("File is %d bytes.\n", sz);
    if(sz % INT_SIZE)
    {
        printf("File isn't a continuous array of 4-byte ints, exiting\n");
        return 1;
    }
    fclose(f);
    pthread_t t;
    s_arg targs;
    targs.size = sz;
    targs.pos = 0;
    strcpy(targs.input, "in");
    strcpy(targs.output, "out");
    targs.done = 0;
    printf("Sorting...\n");
    pthread_create(&t, NULL, recursive_external_merge, &targs);     // thread handles the rest;
    while(targs.done != 1);     // wait for primary thread to finish

    return 0;
}