/**
 * @file   pimlog.h
 * @author Gleb Semenov <gleb.semenov@gmail.com>
 * @date   Thu Jan  9 15:05:52 2020
 * 
 *
 * @brief  printk wrappers
 * 
 * 
 */

#ifndef __PIMLOG_H__
#define __PIMLOG_H__

#define MOD_SIGNATURE "PIM: "

#define kinfo(fmt, args...)    printk(KERN_INFO MOD_SIGNATURE "Info: " fmt, ## args)
#define kwarning(fmt, args...) printk(KERN_WARNING MOD_SIGNATURE "Warning: " fmt, ## args)
#define kerror(fmt, args...)   printk(KERN_ERR MOD_SIGNATURE "Error: " fmt, ## args)
#define kcrit(fmt, args...)    printk(KERN_CRIT MOD_SIGNATURE "Crit: " fmt, ## args)

#endif // #ifndef __PIMLOG_H__
