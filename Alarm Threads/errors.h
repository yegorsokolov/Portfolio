#ifndef __ERRORS_H
#define __ERRORS_H

#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Define a macro for debugging output. 
 * When compiled with -DDEBUG, it prints debugging messages.
 */
#ifdef DEBUG
    #define DPRINTF(arg) printf arg
#else
    #define DPRINTF(arg)
#endif

/*
 * Error handling macros for cleaner error reporting.
 * They print the error message, file, and line number, then abort the program.
 */
#define err_abort(code, text) do { \
    fprintf(stderr, "ERROR: %s at %s:%d: %s\n", \
        text, __FILE__, __LINE__, strerror(code)); \
    abort(); \
} while (0)

#define errno_abort(text) do { \
    fprintf(stderr, "ERROR: %s at %s:%d: %s\n", \
        text, __FILE__, __LINE__, strerror(errno)); \
    abort(); \
} while (0)

/*
 * A safe memory allocation macro that automatically checks for failures.
 */
#define safe_malloc(size) ({ \
    void *ptr = malloc(size); \
    if (!ptr) errno_abort("Memory allocation failed"); \
    ptr; \
})

/*
 * A macro to check return values of pthread functions and handle errors.
 */
#define check_pthread(call) do { \
    int status = (call); \
    if (status != 0) err_abort(status, #call " failed"); \
} while (0)

#endif /* __ERRORS_H */

