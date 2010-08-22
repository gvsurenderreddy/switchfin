/* #########################################################################
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# The Free Software Foundation; version 3 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# Primary Authors: LI YUQIAN
#
# Modified for Switchfin Aug 2010: Dimitar Penev dpn@switchfin.org
#
######################################################################### */

#include <linux/module.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <asm/gpio.h>
#include <linux/proc_fs.h>
#include <linux/platform_device.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("The Edgepbx FX08 restore and clean function");
MODULE_AUTHOR("LI YUQIAN");

#define GPIO_PF13 13 	/* for restore sign, H for restore*/
#define GPIO_PF7  7 	/* SYS LED on PF7 */

int proc_read_restore(char *buf, char **start, off_t offset, 
		      int count, int *eof, void *data);


static int proc_write_sys_led(struct file *file, const char *buffer,
                              unsigned long count, void *data)
{
    int   on;
    char *endbuffer;

    on = simple_strtol (buffer, &endbuffer, 10);
    gpio_direction_output(GPIO_PF7,0);
  
    if (on)
  	gpio_set_value(GPIO_PF7, 1);
    else
  	gpio_set_value(GPIO_PF7, 0);

    return count;
}

int proc_read_restore(char *buf, char **start, off_t offset, 
		    int count, int *eof, void *data)
{
    int len, val;
  
    gpio_direction_input(GPIO_PF13);
    val = gpio_get_value(GPIO_PF13);
    
    len = sprintf(buf, "s%d\n", val );

    *eof=1;
    return len;
}

static int __init restore_init(void)
{
    struct proc_dir_entry *rsc, *sys_led;
		int ret;
		
    create_proc_read_entry("rs", 0, NULL, proc_read_restore, NULL);
    sys_led = create_proc_read_entry("sys_led", 0, NULL, NULL, NULL);
    sys_led->write_proc = proc_write_sys_led;

    ret = gpio_request(GPIO_PF13, "Restore sign");
    if (ret) {
        printk(KERN_WARNING "Restore: unable to request GPIO_PF13\n");
        return ret;
    }

    ret = gpio_request(GPIO_PF7, "System LED");
    if (ret) {
        printk(KERN_WARNING "Restore: unable to request GPIO_PF7\n");
        return ret;
    }

    return 0;
}

static void __exit restore_cleanup(void)
{
    gpio_free(GPIO_PF13);
    gpio_free(GPIO_PF7);
    remove_proc_entry("rs", NULL);
    remove_proc_entry("sys_led", NULL);
    remove_proc_entry("rsc", NULL);
}

module_init(restore_init);
module_exit(restore_cleanup);
