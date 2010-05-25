/*
 * DAHDI Telephony Interface to the Hardawre Line Echo Canceller (ZARLINK)
 *
 * Written by Dimitar Penev <dpn@switchvoice.com>
 * Copyright (C) 2010 Switchvoice Ltd.
 *
 * All rights reserved.
 *
 * Based on dahdi_echocan_hpec.c, Copyright (C) 2006-2008 Digium, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. 
 */

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/ctype.h>
#include <linux/moduleparam.h>
#include <dahdi/kernel.h>

extern int echocan_tail_length(int channel, int tail_ms);
static int(*ec_tail_length)(int,int) = NULL;

#define module_printk(level, fmt, args...) printk(level "%s: " fmt, THIS_MODULE->name, ## args)

static int echo_can_create(struct dahdi_chan *chan, struct dahdi_echocanparams *ecp,
			   struct dahdi_echocanparam *p, struct dahdi_echocan_state **ec);
static void echo_can_free(struct dahdi_chan *chan, struct dahdi_echocan_state *ec);

static const struct dahdi_echocan_factory my_factory = {
	.name = "ZARLINK",
	.owner = THIS_MODULE,
	.echocan_create = echo_can_create,
};

static const struct dahdi_echocan_ops my_ops = {
	.name = "ZARLINK",
	.echocan_free = echo_can_free,
};

struct dahdi_echocan_state dahdi;

#define dahdi_to_pvt(a) container_of(a, struct ec_pvt, dahdi)
static void echo_can_free(struct dahdi_chan *chan, struct dahdi_echocan_state *ec)
{
	int res;

	//Sets BYPASS (0ms tail e/c) for the freed channel
	if (ec_tail_length) {
		res=ec_tail_length(chan->channo - 1, 0);
		if(res) {
                        printk(KERN_WARNING "ZARLINK echo canceler: unsuported channel\n");
                }
	}
}

static int echo_can_create(struct dahdi_chan *chan, struct dahdi_echocanparams *ecp,
			   struct dahdi_echocanparam *p, struct dahdi_echocan_state **ec)
{
	int res;
	
	if (ecp->param_count > 0) {
		printk(KERN_WARNING "ZARLINK echo canceler does not support parameters; failing request\n");
		return -EINVAL;
	}
	
	//Sets 64ms tail e/c for the requested channel
	if (ec_tail_length) {
		res=ec_tail_length(chan->channo - 1, ecp->tap_length);
		if(res) {
                	printk(KERN_WARNING "ZARLINK echo canceler: unsuported channel or tail length\n");
                	return -EINVAL;
		}
	}

	dahdi.ops = &my_ops;
        *ec = &dahdi;

	return 0;
}

static int __init mod_init(void)
{
        
	ec_tail_length = symbol_get(echocan_tail_length);
        if (!ec_tail_length) {
                module_printk(KERN_ERR, "missing Zarlink echo canceller module!\n");
                return -EPERM;
        }

	if (dahdi_register_echocan_factory(&my_factory)) {
		module_printk(KERN_ERR, "could not register with DAHDI core\n");

		return -EPERM;
	}

	module_printk(KERN_INFO, "Registered echo canceler '%s'\n", my_factory.name);

	return 0;
}

static void __exit mod_exit(void)
{
	dahdi_unregister_echocan_factory(&my_factory);
	symbol_put(ec_tail_length);
}

MODULE_DESCRIPTION("DAHDI ZARLINK wrapper");
MODULE_AUTHOR("Dimitar Penev <dpn@switchvoice.com>");
MODULE_LICENSE("GPL");

module_init(mod_init);
module_exit(mod_exit);
