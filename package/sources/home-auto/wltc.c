/*
 * WireLess Remote Controller (WLTC) driver
 * WLTC is a module accepted by IP0x. 
 * It allows the PBX to control various wireless devices 
 * in your smart home like WLRR. 
 * For more information about WLTC and WLTC products please check 
 * www.switchvoice.com
 * 
 * Written by Dimitar Penev <dpn@switchfin.org>
 *
 * Copyright (C) 2011, Switchfin Org.
 *
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. 
 *
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/delay.h>
//#include <linux/proc_fs.h>
#include "sport_interface.h"		//SPI over SPORT

#define WLTC_TIME_OUT 2 		//Time out in seconds

/* ------------------------ Blackfin -------------------------*/
#define SPI_BAUDS   60  // 13.4 MHz for 133MHz system clock    
#define SPI_NCSA    8  //PF8 for nCSA 
#define SPI_NCSB    9  //PF9 for nCSB

#define write_byte(X) sport_tx_byte(SPI_NCSA, X)
#define read_byte() sport_rx_byte(SPI_NCSA)

#ifdef SF_IP01
	#define NUM_CARDS 1
#else
	#define NUM_CARDS 8
#endif

#define WLTC_ID		0xD5

//ioctl commands 
#define WLTC_GET	10
#define WLTC_SET   	11


static int debug = 0;
int card=-1; //Port index of the WLTC in the IP0x 


//* Define state values
typedef enum {
        WLTC_NA = -1,
        WLTC_FREE,
        WLTC_BUSY
} WLTC_STAT;
int  wltc_state;
const int wltc_major = 242;
int wltc_ioctl( struct inode *inodep, struct file *filep, unsigned int cmd, unsigned long arg);
int wltc_open(struct inode *inode, struct file *filp);
int wltc_release(struct inode *inode, struct file *filp);
struct file_operations wltc_fops = { // Structure that declares the usual file access functions
        .open           = wltc_open,
        .release        = wltc_release,
        .ioctl          = wltc_ioctl
};

static inline void wltc_setcard(int);

//SPI over sport is shared by wltc and wcfxs.-----------------------------------------
//We save the sport parameters and then restore it
//params[0] is baud
//params[1] is the CS mask
void sport_restore_params(int *params){
	if(params[0]>0){//If no one has initialized sport we will get params[0] == -1 
		sport_interface_init(params[0], (u16)params[1]);
	}

}

//Wait some 'seconds' ---------------------------------------------------------------
static void wait(int seconds)
{
        set_current_state(TASK_INTERRUPTIBLE);
        schedule_timeout(seconds);
}


// Char device ioctl -----------------------------------------------------------------
int wltc_ioctl( struct inode *inodep, struct file *filep, unsigned int cmd, unsigned long arg)
{
	unsigned char data;
	int i, sport_params[2];
	spinlock_t lock = SPIN_LOCK_UNLOCKED;
	unsigned long flags;

        switch(cmd) {

                case WLTC_SET:
			// Write byte to the WLTC register. 
			// The LSB is used to control the appliance

                        if (card == -1) {
                                printk("No WLTC in the system detected\n");
                                return -EFAULT;
                        }
			spin_lock_irqsave(&lock, flags);
			//get the sport parametsr (baud and mask)
			sport_get_params(sport_params);
        		sport_interface_init(SPI_BAUDS, (1<<SPI_NCSA) | (1<<SPI_NCSB));	
			wltc_setcard(card);
                	write_byte((*(char *)arg) > 0);		//1 Switch on the Relay
								//0 Switch off the Relay
			sport_restore_params(sport_params);	
			spin_unlock_irqrestore(&lock, flags);
			for (i=0;i<(int)(WLTC_TIME_OUT*10); i++){
				wait(HZ/10);
				spin_lock_irqsave(&lock, flags);
				sport_get_params(sport_params);
				sport_interface_init(SPI_BAUDS, (1<<SPI_NCSA) | (1<<SPI_NCSB));
				wltc_setcard(card);
				data=read_byte();
				sport_restore_params(sport_params);
				spin_unlock_irqrestore(&lock, flags);
				if(data == (WLTC_ID & 0xfe)) break;	//The acknolwadge is WLTC ID with LSB cleared
			}				
			
			if(i==(int)(WLTC_TIME_OUT*10)) return -EFAULT;  //Time out 

                        break;

                case WLTC_GET:
                        // Read byte from the WLTC register. 
                        // The LSB is the state of the appliance
			// Currently this command is not supported.

                        if (card == -1) {
                                printk("No WLTC in the system detected\n");
                                return -EFAULT;
                        }

                        //get the sport parametsr (baud and mask)
                        sport_get_params(sport_params);
                        sport_interface_init(SPI_BAUDS, (1<<SPI_NCSA) | (1<<SPI_NCSB));                        
                        wltc_setcard(card);
                        data=read_byte();
			sport_restore_params(sport_params);

			if (copy_to_user((char *)arg, &data, 1)) {
                                printk("Error copying the data\n");
                                return -EFAULT;
                        }
                        
			break;

                default:
                        printk("invalid wltc_ioctl command 0x%X\n", cmd);
                        return -EFAULT;
        }

        return 0;
}


// Char device open/release ----------------------------------------------------------
int wltc_open(struct inode *inode, struct file *filp)
{

        // Check first if device is not busy.
        if (wltc_state == WLTC_BUSY) {
                printk("wltc char device is busy !!\n");
                return -EFAULT;
        }
        if (wltc_state == WLTC_NA) {
                printk("Wcfxs char device is not available !!\n");
                return -EFAULT;
        }
        wltc_state = WLTC_BUSY;

        return 0;
}

int wltc_release(struct inode *inode, struct file *filp)
{
        wltc_state = WLTC_FREE;

        return 0;
}


// WLTC hardware interface ------------------------------------------------------------

// Card - Sets the active SPI device at teh sertain IP0x slot.
// Card is in the range 1..8
// Read the CPLD code to understand  the logic
static inline void wltc_setcard(int card){
        if (card <= 4)
                sport_tx_byte(SPI_NCSB, card);
        else
                sport_tx_byte(SPI_NCSB, 0x40 + (card-4));
}

// sets the card index of teh WLTC in the 1..8 range
// returns 0 in case WLTC is found in the system
//         -1 otherwise
static int wltc_detect(void){
	int i;
	unsigned char data;
	int sport_params[2];
	spinlock_t lock = SPIN_LOCK_UNLOCKED;
	unsigned long flags;
	
	for(i=0;i<NUM_CARDS; i++){
		udelay(1000);
		spin_lock_irqsave(&lock, flags);
		sport_get_params(sport_params);
        	sport_interface_init(SPI_BAUDS, (1<<SPI_NCSA) | (1<<SPI_NCSB));
		wltc_setcard(i+1);
		data=read_byte();
		sport_restore_params(sport_params);
		spin_unlock_irqrestore(&lock, flags);
		if((data & 0xfe) == (WLTC_ID & 0xfe)) { //LSB is an acknowledge bit
			card=i+1;
			break;
		}
		else
			read_byte();       //Slicks gets confused if only one read 
	}
	return -(i == NUM_CARDS);

}
// Module init/exit  ------------------------------------------------------------------
static int __init wltc_init(void){
	
	int res;

//	sport_interface_init(SPI_BAUDS, (1<<SPI_NCSA) | (1<<SPI_NCSB));

	res=wltc_detect();
	if(!res){
		printk(KERN_ALERT "wltc_init: WLTC module found at port %d\n", card);
	}
	else{
		printk(KERN_ALERT "wltc_init: No WLTC module found\n");
		return res;
	}	

        // Registering char device 
        res = register_chrdev(wltc_major, "wltc", &wltc_fops);
        if (res < 0) {
                printk("cannot obtain wltc major number %d\n", wltc_major);
                return res;
        }

	return 0;
}

static void __exit wltc_exit(void)
{
	unregister_chrdev(wltc_major, "wltc");

	printk(KERN_ALERT "wltc_exit: WLTC module exits\n");
}


module_param(debug, int, 0600);

MODULE_DESCRIPTION("WireLess Remote Controller (WLTC) Driver");
MODULE_AUTHOR("Dimitar Penev <dpn@switchfin.org>");
MODULE_LICENSE("GPL");

module_init(wltc_init);
module_exit(wltc_exit);

