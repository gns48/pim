/**
 * @file   timing.h
 * @author Gleb Semenov <gleb.semenov@gmail.com>
 * @date   Sat Jan  4 05:05:34 2020
 * 
 * @brief  execution time calculators
 * 
 * 
 */

#ifndef __TIMING_H__
#define __TIMING_H__

// return execution time of a function, milliseconds
static inline s64 get_timing_ms(void (*f)(void)) {
    ktime_t start, end;
    start = ktime_get();
    f();
    end = ktime_get();
    return ktime_to_ms(ktime_sub(end, start));
}

// return execution time of a function, microseconds
static inline s64 get_timing_us(void (*f)(void)) {
    ktime_t start, end;
    start = ktime_get();
    f();
    end = ktime_get();
    return ktime_to_us(ktime_sub(end, start));
}


// return execution time of a function, nanoseconds
static inline s64 get_timing_ns(void (*f)(void)) {
    ktime_t start, end;
    start = ktime_get();
    f();
    end = ktime_get();
    return ktime_to_ns(ktime_sub(end, start));
}

#endif // #ifndef __TIMING_H__

