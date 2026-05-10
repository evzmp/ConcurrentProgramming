
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include "merge.h"

int fsize(FILE* f)
{
    if(!f) return -1;
    fseek(f, 0L, SEEK_END);
    int size = ftell(f);
    fseek(f, -size, SEEK_CUR);
    return size;
}

void make_name(char* name, int l, int r)
{
    if(!name) return;
    strcpy(name, "temp");
    char lstr[MAX_FILENAME_LEN] = {'\0'};
    char rstr[MAX_FILENAME_LEN] = {'\0'};
    sprintf(lstr, "%d", l);
    sprintf(rstr, "%d", r);
    strcat(name, lstr);
    strcat(name, rstr);
}

void read_int(int fd, int* i)
{
    u_int8_t c[4];
    for(int i = 0; i < 4; ++i)
        read(fd, &c[i], 1);
    *i = c[0] + (c[1] << 8) + (c[2] << 16) + (c[3] << 24);
}

void write_int(int fd, int* i)
{
    u_int8_t c[4];
    c[3] = (*i >> 24) & 0xFF;
    c[2] = (*i >> 16) & 0xFF;
    c[1] = (*i >> 8) & 0xFF;
    c[0] = *i & 0xFF;
    for(int i = 0; i < 4; ++i)
        write(fd, &c[i], 1);
}

void merge(int* arr, int l, int m, int r)
{
    int i, j, k, n1 = m - l + 1, n2 = r - m, L[n1], R[n2];
    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];
  
    i = 0; j = 0; k = l;

    while (i < n1 && j < n2)
    {
        if (L[i] <= R[j])
        {
            arr[k] = L[i];
            i++;
        }
        else
        {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
  
    while (i < n1)
    {
        arr[k] = L[i];
        i++;
        k++;
    }
  
    while (j < n2)
    {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void mergesort(int* arr, int l, int r)
{
    if (l < r)
    {
        int m = l + (r - l) / 2;
        mergesort(arr, l, m);
        mergesort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

void merge_files(char* part1, char* part2, char* output)
{
    if(!part1 || !part2 || !output) return;

    int f1 = open(part1, O_RDWR, 0666);
    int f2 = open(part2, O_RDWR, 0666);
    int fout = open(output, O_RDWR | O_CREAT | O_TRUNC, 0666);

    int n1 = lseek(f1, 0, SEEK_END)/INT_SIZE, n2 =  lseek(f2, 0, SEEK_END)/INT_SIZE;
    int i = 0, j = 0, k = 0, t1 = -255, t2 = -255;

    while (i < n1 && j < n2)
    {
        lseek(f1, i*INT_SIZE, SEEK_SET);
        read_int(f1, &t1);
        lseek(f2, j*INT_SIZE, SEEK_SET);
        read_int(f2, &t2);
        
        if (t1 <= t2)
        {
            lseek(fout, k*INT_SIZE, SEEK_SET);
            write_int(fout, &t1);
            i++;
        }
        else
        {
            lseek(fout, k*INT_SIZE, SEEK_SET);
            write_int(fout, &t2);
            j++;
        }
        k++;
    }
  
    while (i < n1)
    {
        lseek(f1, i*INT_SIZE, SEEK_SET);
        read_int(f1, &t1);
        lseek(fout, k*INT_SIZE, SEEK_SET);
        write_int(fout, &t1);
        i++;
        k++;
    }
  
    while (j < n2)
    {
        lseek(f2, j*INT_SIZE, SEEK_SET);
        read_int(f2, &t2);
        lseek(fout, k*INT_SIZE, SEEK_SET);
        write_int(fout, &t2);
        j++;
        k++;
    }
    close(f1); close(f2); close(fout);
}

void* recursive_external_merge(void* arg)
{
    s_arg* t = (s_arg*) arg;
    if(!t || !(t->input)) return NULL;
    if(t->size <= INTERNAL_SIZE)            // below INTERNAL_SIZE, we sort in memory
    {
        FILE* fin = fopen(t->input, "r+");     // input file must exist!
        FILE* fout = fopen(t->output, "w+");
        char* buf = malloc(t->size);
        fseek(fin, t->pos, SEEK_SET);
        fread(buf, 1, t->size, fin);
        mergesort((int*) buf, 0, ((t->size)/sizeof(int))-1);
        fwrite(buf, 1, t->size, fout);
        free(buf);
        fclose(fin);
        fclose(fout);
    }
    
    else    // we break down into 2 threads
    {
        pthread_t worker1, worker2;
        s_arg w1args, w2args;
        w1args.pos = t->pos;
        w1args.size = (t->size)/2 % INT_SIZE == 0 ? (t->size)/2 : (((t->size)/INT_SIZE)/2)*INT_SIZE;
        w2args.pos = t->pos + w1args.size;
        w2args.size = (t->size) - w1args.size;
        w1args.done = w2args.done = 0;
        strcpy(w1args.input, t->input);
        stpcpy(w2args.input, t->input);
        make_name(w1args.output, w1args.pos, w1args.pos+w1args.size);
        make_name(w2args.output, w2args.pos, w2args.pos+w2args.size);
        pthread_create(&worker1, NULL, recursive_external_merge, &w1args);
        pthread_create(&worker2, NULL, recursive_external_merge, &w2args);
        while(w1args.done == 0 || w2args.done == 0);
        merge_files(w1args.output, w2args.output, t->output);
        remove(w1args.output);
        remove(w2args.output);
    }
    t->done = 1;
    return NULL;
}
