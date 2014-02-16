#ifndef GIT_FILTER_H
#define GIT_FILTER_H

#include <stdlib.h>
#include <stdio.h>

#define log(...) fprintf(stderr, __VA_ARGS__)

#define die(...) do { \
        log(__VA_ARGS__); \
        log("\n"); \
        exit(1); \
    } while(0)

#define A(cond, ...) \
    do { \
        if (cond) { \
            log("error at %d\n", __LINE__); \
            die(__VA_ARGS__); \
        } \
    } while(0)

#endif
