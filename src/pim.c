/**
 * @file   pim.c
 * @author Gleb Semenov <gleb.semenov@gmail.com>
 * @date   Sun Jan  5 21:07:54 2020
 * 
 * @brief  User process integrity monitor
 * 
 */

#include "pim.h"

/*
 * local data definitions
 */

/* pointers to system process frees/thaw functions
   discovered by kallsyms lookup */
static int (*pim_freeze_processes)(void) = 0;
static void (*pim_thaw_processes)(void) =  0;

/* main dada structure: Hash table */
static prochash_t* ph_table = NULL;

/* SipHash key */
static uint128_t siphash_key;

/* init success */
static int init_success = 0;
/* We are getting done */
static int done_flag = 0;


/** 
 * Module initializer.
 * Called by kernel module loading routine
 * 
 * @return 0 is ok
 */
static int pim_init(void) {
    int rv = 0;
    ktime_t start, end;
    int total_counter, user_counter;
    struct task_struct *task;
    
    kinfo("Process integrity monitor started\n");

    pim_freeze_processes = (int (*)(void))kallsyms_lookup_name("freeze_processes");
    if(!pim_freeze_processes) {
        kerror("Can't find 'freeze_processes' function. Exiting...\n");
        rv = ENOENT;
        goto init_exit;
    }

    pim_thaw_processes = (void (*)(void))kallsyms_lookup_name("thaw_processes");
    if(!pim_thaw_processes) {
        kerror("Can't find 'thaw_processes' function. Exiting...\n");
        rv = ENOENT;
        goto init_exit;
    }

    // allocating hash table
    ph_table = (prochash_t*)vzalloc((PID_MAX+1) * sizeof(prochash_t));
    if(!ph_table) {
        kerror("Can not allocate memory for hashtable, exiting\n");
        rv = ENOMEM;
        goto init_exit;
    }

    siphash_key.p_low  = (uint64_t)get_random_long();
    siphash_key.p_high = (uint64_t)get_random_long();
    
    // Freeze all non-kernel processes
    kinfo("Stopping all non-kernel processes\n");
    while (pim_freeze_processes())
        schedule();
    
    kinfo("Initializing hash table...\n");
    total_counter =  user_counter = 0;
    start = ktime_get();
    for_each_process(task) {
        total_counter++;
        if(task->mm) { // not a kernel process
            user_counter++;
            rv = init_ph(ph_table + task->pid, task);
            if(rv) break;
        }
    }
    end = ktime_get();
    kinfo("hash table initialized %d out of %d processes, status: %d, elapsed time %lld ms\n",
          user_counter, total_counter, rv, ktime_to_ms(ktime_sub(end, start)));
    
    // Thaw all non-kernel processes
    pim_thaw_processes();

init_exit:
    init_success = (rv == 0);
    return rv;
}

/** 
 * Module destructor
 * Called by module unloading routine
 *
 */
static void pim_done(void) {
    int i;
    done_flag = 1;
    
    for(i = 1; i <= PID_MAX; i++) 
        if(ph_table[i].text) vfree(ph_table[i].text);

    if(ph_table) vfree(ph_table);
    kinfo("Process integrity monitor exiting.\n");
}

/*
 * Module loading stuff
 */

module_init(pim_init);
module_exit(pim_done);

MODULE_AUTHOR ("Gleb Semenov, gleb.semenov@gmail.com");
MODULE_DESCRIPTION ("User processes integrity monitor");
MODULE_VERSION("0.1alpha");
MODULE_LICENSE("GPL");










