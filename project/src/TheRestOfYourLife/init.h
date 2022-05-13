#ifndef INIT_H
#define INIT_H
#include <stdlib.h>

void init_seeds (unsigned int **seeds_ptr, int thread_count) {
    /* if (*seeds_ptr != NULL) { */
    /*     free(*seeds_ptr); */
    /* } */
    *seeds_ptr = (unsigned int *) malloc(sizeof(unsigned int) * thread_count);
    for (int i = 0; i < thread_count; i++) {
        (*seeds_ptr)[i] = i+1;
    }
}
#endif
