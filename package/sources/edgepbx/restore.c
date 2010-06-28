#include <linux/module.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <asm/gpio.h>
#include <linux/proc_fs.h>
#include <linux/platform_device.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("The Edgepbx FX08 restore and clean function");
MODULE_AUTHOR("LI YUQIAN");

#define GPIO_PF6  6 /* for restore sign, H for restore*/
#define GPIO_PF3  3 /* SYS LED on PF7 */
#define GPIO_PF11 11 /* for clean the PF6 sign, H for clean restore sign */

int proc_read_restore(char *buf, char **start, off_t offset, 
		    int count, int *eof, void *data);


static int proc_write_restore_clean(struct file *file, const char *buffer,
                             unsigned long count, void *data)
{
  int   on;
  char *endbuffer;

  on = simple_strtol (buffer, &endbuffer, 10);
	gpio_direction_output(GPIO_PF11,0);
  
  if (on)
  	gpio_set_value(GPIO_PF11, 1);
  else
  	gpio_set_value(GPIO_PF11, 0);

  return count;
}

static int proc_write_sys_led(struct file *file, const char *buffer,
                              unsigned long count, void *data)
{
  int   on;
  char *endbuffer;

  on = simple_strtol (buffer, &endbuffer, 10);
	gpio_direction_output(GPIO_PF3,0);
  
  if (on)
  	gpio_set_value(GPIO_PF3, 1);
  else
  	gpio_set_value(GPIO_PF3, 0);

  return count;
}

int proc_read_restore(char *buf, char **start, off_t offset, 
		    int count, int *eof, void *data)
{
	int len, val;
  
  gpio_direction_input(GPIO_PF6);
  val = gpio_get_value(GPIO_PF6);
    
	len = sprintf(buf, "s%d\n", val );

	*eof=1;
	return len;
}

static int __init restore_init(void)
{
    struct proc_dir_entry *rsc, *sys_led;
		int ret;
		
    create_proc_read_entry("rs", 0, NULL, proc_read_restore, NULL);
    rsc = create_proc_read_entry("rsc", 0, NULL, NULL, NULL);
    rsc->write_proc = proc_write_restore_clean;
    sys_led = create_proc_read_entry("sys_led", 0, NULL, NULL, NULL);
    sys_led->write_proc = proc_write_sys_led;

    ret = gpio_request(GPIO_PF6, "Restore sign");
    if (ret) {
        printk(KERN_WARNING "Restore: unable to request GPIO_PF6\n");
        return ret;
    }


    ret = gpio_request(GPIO_PF3, "System LED");
    if (ret) {
        printk(KERN_WARNING "Restore: unable to request GPIO_PF3\n");
        return ret;
    }

    ret = gpio_request(GPIO_PF11, "Restore Clean");
    if (ret) {
        printk(KERN_WARNING "Restore: unable to request GPIO_PF11\n");
        return ret;
    }
    return 0;
}

static void __exit restore_cleanup(void)
{
    gpio_free(GPIO_PF6);
    gpio_free(GPIO_PF3);
    gpio_free(GPIO_PF11);
    remove_proc_entry("rs", NULL);
    remove_proc_entry("sys_led", NULL);
    remove_proc_entry("rsc", NULL);
}

module_init(restore_init);
module_exit(restore_cleanup);
