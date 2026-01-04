#ifndef CANON_C_UTIL_TIME_H
#define CANON_C_UTIL_TIME_H

#include <stdint.h>

/*
    time.h — Simple high-resolution stopwatch

    Platform-agnostic where possible.
    Uses monotonic clock — safe for intervals.
    No global state, explicit start/stop.
*/

typedef struct {
    uint64_t start;
} Stopwatch;

/* Start the stopwatch (records current time) */
static inline void stopwatch_start(Stopwatch* sw)
{
    if (!sw) return;
#ifdef _WIN32
    LARGE_INTEGER freq, counter;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&counter);
    sw->start = (uint64_t)(counter.QuadPart * 1000000000ULL / freq.QuadPart);
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    sw->start = (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
#endif
}

/* Returns elapsed time in nanoseconds since start */
static inline uint64_t stopwatch_elapsed_ns(const Stopwatch* sw)
{
    if (!sw) return 0;
#ifdef _WIN32
    LARGE_INTEGER freq, counter;
    QueryPerformanceCounter(&counter);
    QueryPerformanceFrequency(&freq);
    uint64_t now = (uint64_t)(counter.QuadPart * 1000000000ULL / freq.QuadPart);
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    uint64_t now = (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
#endif
    return now - sw->start;
}

/* Convenience: elapsed in seconds (double) */
static inline double stopwatch_elapsed_sec(const Stopwatch* sw)
{
    return (double)stopwatch_elapsed_ns(sw) / 1e9;
}

#endif /* CANON_C_UTIL_TIME_H */
