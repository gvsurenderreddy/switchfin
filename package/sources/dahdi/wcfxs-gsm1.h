/*
 * Wilcard S100P FXS Interface Driver for Zapata Telephony interface
 *
 * Written by Mark Spencer <markster@linux-support.net>
 *
 * Copyright (C) 2001, Linux Support Services, Inc.
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

#include <linux/ioctl.h>

#define NUM_REGS	  109
#define NUM_INDIRECT_REGS 105

struct wcfxs_stats {
	int tipvolt;	/* TIP voltage (mV) */
	int ringvolt;	/* RING voltage (mV) */
	int batvolt;	/* VBAT voltage (mV) */
};

struct wcfxs_regs {
	unsigned char direct[NUM_REGS];
	unsigned short indirect[NUM_INDIRECT_REGS];
};

struct wcfxs_regop {
	int indirect;
	unsigned char reg;
	unsigned short val;
};

//* Define state values
typedef enum {
        WCGSM_NA = -1,
        WCGSM_FREE,
        WCGSM_BUSY
} WCGSM_STAT;

#define SMS_MAX_LOCAL_SMS   	10
#define SMS_MAX_SIM_SMS    	32
#define SMS_NUM_MAX_LEN 	20
#define SMS_MAX_LEN 		250  
#define SMS_TIMEOUT 		20 

struct SMS_tag {
	int count;		//Total number of available messages
				//still not read from the user space	  
	char *sms[SMS_MAX_LOCAL_SMS];//SMS messages 
} SMS;

int sms_transaction=0;		//This flag is set to 1 during getting/sending SMS to the GSM1 module

#define WCFXS_GET_STATS	_IOR (DAHDI_CODE, 60, struct wcfxs_stats)
#define WCFXS_GET_REGS	_IOR (DAHDI_CODE, 61, struct wcfxs_regs)
#define WCFXS_SET_REG	_IOW (DAHDI_CODE, 62, struct wcfxs_regop)

#define WCGSM_GET_CALLERID    10
#define WCGSM_GET_SMS_COUNT   11
#define WCGSM_GET_SMS         12
#define WCGSM_SEND_SMS        13
