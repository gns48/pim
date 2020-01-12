/**
 * @file   hashtable.c
 * @author Gleb Semenov <gleb.semenov@gmail.com>
 * @date   Sun Jan  5 17:13:16 2020
 * 
 * @brief  Hastable manipulation routines
 * 
 */

#include "pim.h"

/** 
 * Initialize hastable entry
 * 
 * @param ph  - ptr to hastable entry
 * @param task ptr
 * 
 * @return 0 if ok
 */
int init_ph(prochash_t *ph, struct task_struct *task) {
    ph->start_code = (uint8_t*)task->mm->start_code;
    ph->length     = (size_t)task->mm->end_code - (size_t)task->mm->start_code;
    ph->text       = (uint8_t*)vzalloc(ph->length);
    if(!ph->text) {
        kerror("can not allocate code segment for %d[%s]\n", task->pid, task->comm);
        return ENOMEM;
    }
    ph->count = 0ul;
    ph->siphash = 0ull;
    strcpy(ph->comm, task->comm);
    return 0;
}

/** 
 * Remove dead processes and dump hashtable
 * 
 * @param ph_table - ptr to hash table
 */
void cleanup_and_dump_hashtable(prochash_t* ph_table) {
    pid_t i;
    for(i = 1; i <= PID_MAX; i++) {
        if(ph_table[i].start_code) {
            if(!pid_task(find_vpid(i), PIDTYPE_PID)) { // process finished
                vfree(ph_table[i].text);
                memset(ph_table + i, 0, sizeof(prochash_t));
                kinfo("%6d Process is dead, removed\n", i);
            }
            else {
                kinfo("%6d %10ld %10ld %08lX %08lX %s", i,
                       ph_table[i].length,
                       ph_table[i].count,
                       ph_table[i].siphash,
                       ph_table[i].siphash_old,
                       ph_table[i].comm);
            }
        }
    }
}

/** 
 * Calculate hash ror hashtable element
 * Called from timer interrupt in user context
 * 
 * @param ph - pointer to hastable element
 * 
 * @return 0 is ok
 */
int calculate_hash(prochash_t *ph,  const uint128_t *k) {
    size_t n;

    if(access_ok(VERIFY_READ, ph->start_code, ph->length)) {
        n = copy_from_user(ph->text, ph->start_code, ph->length);
        if(n == 0) {
            pim_siphash64(ph->text, ph->length, k, (uint8_t*)&ph->siphash);
            if(!ph->count) ph->siphash_old = ph->siphash;
            ph->count++;
            return 0;
        }
    }
    return EACCES;
}

