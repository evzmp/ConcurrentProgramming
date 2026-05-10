#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "crt.h"

#define BUF_SIZE 64

static void writer(void* arg)
{
    s_FunctionArgs* f_args = (s_FunctionArgs*) arg;
    printf("writer: started\n");

    for(int i = 0; i < BUF_SIZE; ++i)
    {
        if(read(f_args->in, f_args->buf+i, 1) != 1)
        {
            f_args->fend = 1;
            f_args->written = i;
            mycoroutines_switchto(f_args->reader);
            break;
        }
    }

    f_args->written = BUF_SIZE;
    mycoroutines_switchto(f_args->reader);
}

static void reader(void* arg)
{
    s_FunctionArgs* f_args = (s_FunctionArgs*) arg;
    printf("reader: started\n");
    for(int i = 0; i < f_args->written; ++i)
    {
        write(f_args->out, f_args->buf+i, 1);
    }
    
    if(f_args->fend)
    {
        mycoroutines_switchto(f_args->main);
    }

    mycoroutines_switchto(f_args->writer);
}

int main(int argc, char *argv[])
{
    s_FunctionArgs co_args;
    co_args.in = open("in", O_RDWR);
    co_args.out = open("out", O_RDWR | O_CREAT | O_TRUNC, 0666);
    co_args.fend = co_args.written = 0;
    co_args.buf = malloc(BUF_SIZE*sizeof(char));
    co_args.main = malloc(sizeof(co_t));
    co_args.reader = malloc(sizeof(co_t));
    co_args.writer = malloc(sizeof(co_t));

    mycoroutines_init(co_args.main);
    printf("Initialized main\n");
    mycoroutines_create(co_args.reader, reader, &co_args);
    mycoroutines_create(co_args.writer, writer, &co_args);
    mycoroutines_switchto(co_args.writer);

    free(co_args.buf);
    mycoroutines_destroy(co_args.main);
    mycoroutines_destroy(co_args.reader);
    mycoroutines_destroy(co_args.writer);
}