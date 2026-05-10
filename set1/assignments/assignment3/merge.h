
#pragma once

#define INT_SIZE sizeof(int)
#define INTERNAL_SIZE 64*INT_SIZE
#define MAX_FILENAME_LEN 128

typedef struct s_arg
{
    char input[MAX_FILENAME_LEN], output[MAX_FILENAME_LEN];
    int size, pos, done;
}s_arg;

int fsize(FILE* f);
void make_name(char* name, int l, int r);
void* recursive_external_merge(void* arg);
void merge(int* arr, int l, int m, int r);
void mergesort(int* arr, int l, int r);
void merge_files(char* part1, char* part2, char* output);
void read_int(int fd, int* i);
void write_int(int fd, int* i);