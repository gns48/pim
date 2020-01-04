/**
 * @file   hashtable.h
 * @author Gleb Semenov <gleb.semenov@gmail.com>
 * @date   Sat Jan  4 02:06:20 2020
 * 
 * @brief  Process code segments hash table
 * 
 * 
 */

#ifndef __HASHTABLE_H__
#define __HASHTABLE_H__

#ifndef PID_MAX
#define PID_MAX 32768
#endif

// process code segment integrity data array element
// indexed by pid
typedef struct proc_hash {
    unsigned long start_code;  // start of code segment
    size_t length;             // length of code segment
    unsigned long count;       // hashcount runs counter
    unsigned long siphash;     // process code segment gasg value
} prochash_t;

#endif // #ifndef __HASHTABLE_H__

