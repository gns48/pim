#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/kallsyms.h>
#include <linux/mm.h>
#include <linux/time.h>
#include <linux/delay.h>
#include "timing.h"
#include "hashtable.h"
// #include "siphash.h"

static prochash_t* ph_table = NULL;

// pointers to system process frees/thaw functions
// discovered by kallsyms lookup
int (*pim_freeze_processes)(void) = 0;
void (*pim_thaw_processes)(void) =  0;

// remove dead processes
static void cleanup_hashtable(void) {
    pid_t i;
    for(i = 1; i <= PID_MAX; i++) {
        if(ph_table[i].start_code) {
            if(!pid_task(find_vpid(i), PIDTYPE_PID)) { // process finished
                memset(&ph_table[i], 0, sizeof(prochash_t));
            }
        }
    }
}

static void update_hashtable(void) {
    struct task_struct *task;
    for_each_process(task) {
        if(task->mm) { // not a kernel process
            if(ph_table[task->pid].start_code != task->mm->start_code ||
               ph_table[task->pid].length != task->mm->end_code - task->mm->start_code) {
                // no pid or new process
                ph_table[task->pid].start_code = task->mm->start_code;
                ph_table[task->pid].length = task->mm->end_code - task->mm->start_code;
                ph_table[task->pid].count = 0;
                ph_table[task->pid].siphash = 0;
            }
        }
    }
}

static int pim_init(void) {
    int rv = 0;
    
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

    ph_table = (prochash_t*)vzalloc((PID_MAX+1) * sizeof(prochash_t));
    if(!ph_table) {
        printk("PIM: Can not allocate memory for hashtable, exiting\n");
        rv = ENOMEM;
        goto init_exit;
    }
    
    // Freeze all non-kernel processes
    printk("PIM: INFO: Stopping all non-kernel processes\n");
    while (pim_freeze_processes())
        schedule();
    
    printk("PIM: INFO: Initializing hash table...\n");
    printk("PIM: INFO: hash table initialized, %lld ms\n", get_timing_ms(update_hashtable));
    
    // Thaw all non-kernel processes
    pim_thaw_processes();

init_exit:
    return rv;
}

static void pim_done(void) {
    if(ph_table) vfree(ph_table);
    printk("PIM: exiting.\n");
}

module_init(pim_init);
module_exit(pim_done);

MODULE_AUTHOR ("Gleb Semenov");
MODULE_DESCRIPTION ("User processes integrity checker");
MODULE_LICENSE("GPL");

