
#ifndef RUNTIME_H
#define RUNTIME_H


#include "stdint.h"
#include "stdbool.h"


typedef struct
{
    /** Runtime in seconds */
    float duration;

    /** Timer value when starting to count */
    uint32_t time_start;

    /** Overflow value */
    uint32_t time_stop;

    /** Once off, stay off */
    bool is_expired;
} runtime_t;


/**
 * Configure the number of seconds after which to expire
 */
void runtime_set(runtime_t*, float seconds);

/**
 * Start counting i.e. remember the start time
 */
void runtime_start(runtime_t*);

/**
 * Check whether the runtime has expired
 */
bool is_runtime_expired(runtime_t*);


#endif
