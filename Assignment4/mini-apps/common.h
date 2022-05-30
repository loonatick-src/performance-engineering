#ifndef PE_COMMON_H
#define PE_COMMON_H

#include "papi.h"

void handle_error (int retval)
{
     printf("PAPI error %d: %s\n", retval, PAPI_strerror(retval));
     exit(1);
}
#endif
