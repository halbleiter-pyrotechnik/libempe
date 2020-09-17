
#include "runtime.h"
#include "time.h"


/**
 * The frequency of the SysTick timer
 * which is used in get_time()
 */
static const float timer_frequency = 90e6;


inline void runtime_set(runtime_t* t, float seconds)
{
    t->duration = seconds;
}


inline void runtime_start(runtime_t* t)
{
    t->is_expired = false;
    t->time_start = get_time();
    // Negative duration => run forever
    if (t->duration <= 0.0f)
        t->time_stop = 0;
    else
        t->time_stop = t->time_start + (uint32_t) (timer_frequency * t->duration);
}


inline bool is_runtime_expired(runtime_t* t)
{
    // Once off, stay off
    if (t->is_expired)
        return true;

    if (t->time_start == 0)
    {
        // First invokation or uninitialized struct
        runtime_start(t);
        return false;
    }

    // Stop time is zero => run forever
    t->is_expired = (t->time_stop != 0) && (get_time() > t->time_stop);
    return t->is_expired;
}
