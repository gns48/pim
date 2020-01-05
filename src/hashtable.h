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
    uint8_t* start_code;   // start of code segment
    uint8_t* text;         // code segment in kernel space
    size_t length;         // length of code segment
    unsigned long count;   // hashcount runs counter
    unsigned long siphash; // process code segment hash value
    unsigned long siphash_old; // process code segment old hash value
    char comm[TASK_COMM_LEN]; // task name
} prochash_t;

int init_ph(prochash_t *ph, struct task_struct *task);
void cleanup_and_dump_hashtable(prochash_t* ph_table);

#endif // #ifndef __HASHTABLE_H__
