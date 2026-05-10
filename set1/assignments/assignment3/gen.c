#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    if(argc != 2)
    {
        printf("Invalid args\n");
        return -1;
    }
    FILE* f = fopen("in", "w+b");

    srand(576712);
    
    char bytes[4] = {'\0'};
    for(int i = 0; i < atoi(argv[1]); ++i)
    {
        int n = rand()%255;
        bytes[3] = (n >> 24) & 0xFF;
        bytes[2] = (n >> 16) & 0xFF;
        bytes[1] = (n >> 8) & 0xFF;
        bytes[0] = n & 0xFF;
        fwrite(&bytes[0], 1, 1, f);
        fwrite(&bytes[1], 1, 1, f);
        fwrite(&bytes[2], 1, 1, f);
        fwrite(&bytes[3], 1, 1, f);
    }
    fclose(f);
    return 0;
}