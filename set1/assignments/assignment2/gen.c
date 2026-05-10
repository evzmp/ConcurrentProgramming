#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

int main(int argc, char** argv)
{
    if(argc != 2 || atoi(argv[1]) < 1)
        return -1;
    int count = atoi(argv[1]);
    int j = 0;
    for(long i = 0; i < count; ++i, j++)
    {
        
        printf("%d ", rand()%(INT_MAX/count)+1);
        if(j >= 10)
        {
            printf("\n");
            j = 0;
        }
    }

    printf("-1\n");

    return 0;
}