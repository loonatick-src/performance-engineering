#ifndef __loonatick_dbg_h__
#define __loonatick_dbg_h__

/* Debugging macros
 * Small additions to the debug macros in
 * Zed A. Shaw's ``Learc C The Hard Way"
 * */

#include <stdio.h>
#include <errno.h>
#include <string.h>

#ifdef NDEBUG
#define debug(M, ...)
#define debug_bytes(O, ...)
#define debug_conditional(C, M, ...)

#else


#define debug(M, ...) fprintf(stderr, "DEBUG %s:%d in function %s: " M "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#endif

#define clean_errno() (errno == 0 ? "None" : strerror(errno))

#define log_err(M, ...) fprintf(stderr, "[ERROR] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

#define log_warn(M, ...) fprintf(stderr, "[WARN] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

#define log_info(M, ...) fprintf(stderr, "[INFO] (%s:%d) " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define check(A, M, ...) if(!(A)) { log_err(M, ##__VA_ARGS__); errno=0; goto error;}

#define sentinel(M, ...) { log_err(M, ##__VA_ARGS__); errno=0; goto error; }

#define check_mem(A) check((A), "Out of memory.")

#define check_debug(A, M, ...) if (!(A)) { debug(M, ##__VA_ARGS__); errno=0; goto error; }

#define debug_bytes(O)\
    unsigned char *debug_bytes_ptr = (unsigned char *)(O);\
    for (int debug_bytes_index = 0; debug_bytes_index < sizeof(*O); debug_bytes_index++, debug_bytes_ptr++)\
    fprintf(stderr, "%x ", *debug_bytes_ptr);\
    putchar('\n'); 

#define debug_conditional(C, M, ...) if (C) { debug(M, ##__VA_ARGS__); }
#endif
