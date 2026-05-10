#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    FILE* f = fopen("input1", "w");
    char c;
    for(int i = 0, j = 0; i < 100000; ++i, ++j)
    {
        if(j == 48)
        {
            c = '\n';
            fwrite(&c, 1, 1, f);
            j = 0;
            continue;
        }
        c = (char) rand()%(90+1-70)+70;
        fwrite(&c, 1, 1, f);
    }
    fclose(f);

    return 0;
}