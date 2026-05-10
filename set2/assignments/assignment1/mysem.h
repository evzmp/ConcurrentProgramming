
#pragma once

typedef struct sMysem
{
    int num, active;
}sMysem;

int mysem_init(sMysem* s, int n);
int mysem_down(sMysem* s);
int mysem_up(sMysem* s);
int mysem_destroy(sMysem* s);
int mysem_print(sMysem* s);
