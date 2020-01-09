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

#define kinfo(fmt, args...)    printk(MOD_SIGNATURE "Info: " fmt, ## args)
#define kwarning(fmt, args...) printk(MOD_SIGNATURE "Warning: " fmt, ## args)
#define kerror(fmt, args...)   printk(MOD_SIGNATURE "Error: " fmt, ## args)
#define kcrit(fmt, args...)    printk(MOD_SIGNATURE "Critical: " fmt, ## args)

#endif // #ifndef __PIMLOG_H__
