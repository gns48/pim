#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/kallsyms.h>
#include <linux/mm.h>
#include <linux/time.h>
#include <linux/delay.h>
#include <linux/random.h>
#include <linux/uaccess.h>

#include "timing.h"
#include "hashtable.h"
#include "siphash.h"

static prochash_t* ph_table = NULL;

// pointers to system process frees/thaw functions
// discovered by kallsyms lookup
static int (*pim_freeze_processes)(void) = 0;
static void (*pim_thaw_processes)(void) =  0;

// SipHash key
static uint128_t siphash_key;

// init success
static int init_success = 0;

static int calculate_hash(prochash_t *ph) {
    size_t n = ph->length;

    if(!ph->text) return ENOMEM;

    n = copy_from_user(ph->text, ph->start_code, ph->length);
    if(n < ph->length) {
        pim_siphash64(ph->text, ph->length-n, &siphash_key, (uint8_t*)&ph->siphash);
        ph->count++;
        return 0;
    }
    else return EACCES;
}

static int pim_init(void) {
    int rv = 0;
    ktime_t start, end;
    struct task_struct *task;
    
    printk("Process integrity checking module started\n");

    pim_freeze_processes = (int (*)(void))kallsyms_lookup_name("freeze_processes");
    if(!pim_freeze_processes) {
        printk("PIM: ERROR: Can't find 'freeze_processes' function. Exiting...\n");
        rv = ENOENT;
        goto init_exit;
    }

    pim_thaw_processes = (void (*)(void))kallsyms_lookup_name("thaw_processes");
    if(!pim_thaw_processes) {
        printk("PIM: ERROR: Can't find 'thaw_processes' function. Exiting...\n");
        rv = ENOENT;
        goto init_exit;
    }

    // allocating hash table
    ph_table = (prochash_t*)vzalloc((PID_MAX+1) * sizeof(prochash_t));
    if(!ph_table) {
        printk("PIM: Can not allocate memory for hashtable, exiting\n");
        rv = ENOMEM;
        goto init_exit;
    }

    siphash_key.p_low  = (uint64_t)get_random_long();
    siphash_key.p_high = (uint64_t)get_random_long();
    
    // Freeze all non-kernel processes
    printk("PIM: INFO: Stopping all non-kernel processes\n");
    while (pim_freeze_processes())
        schedule();
    
    printk("PIM: INFO: Initializing hash table...\n");
    start = ktime_get();
    for_each_process(task) {
        rv = init_ph(ph_table + task->pid, task);
        if(rv) break;
    }
    end = ktime_get();
    printk("PIM: INFO: hash table initialized, status: %d, elapsed time %lld ms\n",
           rv, ktime_to_ms(ktime_sub(end, start)));
    
    // Thaw all non-kernel processes
    pim_thaw_processes();

init_exit:
    init_success = (rv == 0);
    return rv;
}

static void pim_done(void) {
    int i;
    for(i = 1; i <= PID_MAX; i++) {
        if(ph_table[i].text)
            vfree(ph_table[i].text);
    }
    if(ph_table) vfree(ph_table);
    printk("PIM: exiting.\n");
}

module_init(pim_init);
module_exit(pim_done);

MODULE_AUTHOR ("Gleb Semenov");
MODULE_DESCRIPTION ("User processes integrity checker");
MODULE_LICENSE("GPL");
