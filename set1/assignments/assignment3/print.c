#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv)
{
    if(argc != 2)
    {
        printf("Invalid args\n");
        return -1;
    }
    char filename[128] = {'\0'};
    strcpy(filename, argv[1]);
    FILE* f = fopen(filename, "rb");
    fseek(f, 0L, SEEK_END);
    int bytes = ftell(f);
    printf("Size: %d bytes\n", bytes);
    rewind(f);
    for(int i = 0; i < bytes/4; ++i)
    {
        int m;
        fread(&m, 4, 1, f);
        printf("%d ", m);
    }
    printf("\n");
    fclose(f);
    return 0;
}