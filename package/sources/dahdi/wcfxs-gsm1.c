/*
 * Wildcard TDM400P TDM FXS/FXO Interface Driver for Zapata Telephony interface
 *
 * Written by Mark Spencer <markster@linux-support.net>
 *            Matthew Fredrickson <creslin@linux-support.net>
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

/*
 * Adapted to the Blackfin by David Rowe 2005.....
 *
 * BLACKFIN DEBUGGING NOTES:
 *
 * insmod wcfxs.ko debug=1 will enable debugging, but printks inside
 * the ISR will not be enabled.
 *
 * insmod wcfxs.ko debug=2 will enable debugging including printks
 * inside the ISR.
 * 
 * debug=2 increases the latency of the ISR, for example the ISR may
 * take longer than one interrupt to execute, which will upset timing
 * measurements in the /proc/bfsi.
 *
 * The SPI can be driven by the hardware SPI or by the SPORT
 * Hardware SPI is used if macro CONFIG_4FX_SPI_INTERFACE is defined, 
 * otherwise SPORT is used 
 * MULTI_SPI_FRAMEWORK is not defined for IP0x
 *
 *  5 Mar. 2010 Dimitar Penev Modified for DAHDI
 * 18 Mar. 2010 Dimitar Penev included IP01 support 
 * for use in Switchfin, based on the previous work done by Atcom
 * 01 Sep. 2010 Dimitar Penev added GSM1 support 	
 * 
 * Copyright @ 2010 Switchfin <dpn@switchfin.org>
 */

#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/pci.h>
#include <linux/interrupt.h>
#include <linux/proc_fs.h>
#include "proslic.h"
#include "wcfxs.h"
#include "bfsi.h"

#ifndef SF_IP01
#include "sport_interface.h"		//Added by Alex Tao
#endif

/*
 *  Define for audio vs. register based ring detection
 *  
 */
/* #define AUDIO_RINGCHECK  */

/*
  Experimental max loop current limit for the proslic
  Loop current limit is from 20 mA to 41 mA in steps of 3
  (according to datasheet)
  So set the value below to:
  0x00 : 20mA (default)
  0x01 : 23mA
  0x02 : 26mA
  0x03 : 29mA
  0x04 : 32mA
  0x05 : 35mA
  0x06 : 37mA
  0x07 : 41mA
*/
static int loopcurrent = 20;

static alpha  indirect_regs[] =
{
{0,255,"DTMF_ROW_0_PEAK",0x55C2},
{1,255,"DTMF_ROW_1_PEAK",0x51E6},
{2,255,"DTMF_ROW2_PEAK",0x4B85},
{3,255,"DTMF_ROW3_PEAK",0x4937},
{4,255,"DTMF_COL1_PEAK",0x3333},
{5,255,"DTMF_FWD_TWIST",0x0202},
{6,255,"DTMF_RVS_TWIST",0x0202},
{7,255,"DTMF_ROW_RATIO_TRES",0x0198},
{8,255,"DTMF_COL_RATIO_TRES",0x0198},
{9,255,"DTMF_ROW_2ND_ARM",0x0611},
{10,255,"DTMF_COL_2ND_ARM",0x0202},
{11,255,"DTMF_PWR_MIN_TRES",0x00E5},
{12,255,"DTMF_OT_LIM_TRES",0x0A1C},
{13,0,"OSC1_COEF",0x7B30},
{14,1,"OSC1X",0x0063},
{15,2,"OSC1Y",0x0000},
{16,3,"OSC2_COEF",0x7870},
{17,4,"OSC2X",0x007D},
{18,5,"OSC2Y",0x0000},
{19,6,"RING_V_OFF",0x0000},
{20,7,"RING_OSC",0x7EF0},
{21,8,"RING_X",0x0160},
{22,9,"RING_Y",0x0000},
{23,255,"PULSE_ENVEL",0x2000},
{24,255,"PULSE_X",0x2000},
{25,255,"PULSE_Y",0x0000},
//{26,13,"RECV_DIGITAL_GAIN",0x4000},	// playback volume set lower
{26,13,"RECV_DIGITAL_GAIN",0x2000},	// playback volume set lower
{27,14,"XMIT_DIGITAL_GAIN",0x4000},
//{27,14,"XMIT_DIGITAL_GAIN",0x2000},
{28,15,"LOOP_CLOSE_TRES",0x1000},
{29,16,"RING_TRIP_TRES",0x3600},
{30,17,"COMMON_MIN_TRES",0x1000},
{31,18,"COMMON_MAX_TRES",0x0200},
{32,19,"PWR_ALARM_Q1Q2",0x07C0},
{33,20,"PWR_ALARM_Q3Q4",0x2600},
{34,21,"PWR_ALARM_Q5Q6",0x1B80},
{35,22,"LOOP_CLOSURE_FILTER",0x8000},
{36,23,"RING_TRIP_FILTER",0x0320},
{37,24,"TERM_LP_POLE_Q1Q2",0x008C},
{38,25,"TERM_LP_POLE_Q3Q4",0x0100},
{39,26,"TERM_LP_POLE_Q5Q6",0x0010},
{40,27,"CM_BIAS_RINGING",0x0C00},
{41,64,"DCDC_MIN_V",0x0C00},
{42,255,"DCDC_XTRA",0x1000},
{43,66,"LOOP_CLOSE_TRES_LOW",0x1000},
};

static struct fxo_mode {
	char *name;
	/* FXO */
	int ohs;
	int ohs2;
	int rz;
	int rt;
	int ilim;
	int dcv;
	int mini;
	int acim;
	int ring_osc;
	int ring_x;
} fxo_modes[] =
{
	{ "FCC", 0, 0, 0, 1, 0, 0x3, 0, 0 }, 	/* US, Canada */
	{ "TBR21", 0, 0, 0, 0, 1, 0x3, 0, 0x2, 0x7e6c, 0x023a },
										/* Austria, Belgium, Denmark, Finland, France, Germany, 
										   Greece, Iceland, Ireland, Italy, Luxembourg, Netherlands,
										   Norway, Portugal, Spain, Sweden, Switzerland, and UK */
	{ "ARGENTINA", 0, 0, 0, 0, 0, 0x3, 0, 0 },
	{ "AUSTRALIA", 1, 0, 0, 0, 0, 0, 0x3, 0x3 },
	{ "AUSTRIA", 0, 1, 0, 0, 1, 0x3, 0, 0x3 },
	{ "BAHRAIN", 0, 0, 0, 0, 1, 0x3, 0, 0x2 },
	{ "BELGIUM", 0, 1, 0, 0, 1, 0x3, 0, 0x2 },
	{ "BRAZIL", 0, 0, 0, 0, 0, 0, 0x3, 0 },
	{ "BULGARIA", 0, 0, 0, 0, 1, 0x3, 0x0, 0x3 },
	{ "CANADA", 0, 0, 0, 0, 0, 0x3, 0, 0 },
	{ "CHILE", 0, 0, 0, 0, 0, 0x3, 0, 0 },
	{ "CHINA", 0, 0, 0, 0, 0, 0, 0x3, 0xf },
	{ "COLUMBIA", 0, 0, 0, 0, 0, 0x3, 0, 0 },
	{ "CROATIA", 0, 0, 0, 0, 1, 0x3, 0, 0x2 },
	{ "CYRPUS", 0, 0, 0, 0, 1, 0x3, 0, 0x2 },
	{ "CZECH", 0, 0, 0, 0, 1, 0x3, 0, 0x2 },
	{ "DENMARK", 0, 1, 0, 0, 1, 0x3, 0, 0x2 },
	{ "ECUADOR", 0, 0, 0, 0, 0, 0x3, 0, 0 },
	{ "EGYPT", 0, 0, 0, 0, 0, 0, 0x3, 0 },
	{ "ELSALVADOR", 0, 0, 0, 0, 0, 0x3, 0, 0 },
	{ "FINLAND", 0, 1, 0, 0, 1, 0x3, 0, 0x2 },
	{ "FRANCE", 0, 1, 0, 0, 1, 0x3, 0, 0x2 },
	{ "GERMANY", 0, 1, 0, 0, 1, 0x3, 0, 0x3 },
	{ "GREECE", 0, 1, 0, 0, 1, 0x3, 0, 0x2 },
	{ "GUAM", 0, 0, 0, 0, 0, 0x3, 0, 0 },
	{ "HONGKONG", 0, 0, 0, 0, 0, 0x3, 0, 0 },
	{ "HUNGARY", 0, 0, 0, 0, 0, 0x3, 0, 0 },
	{ "ICELAND", 0, 1, 0, 0, 1, 0x3, 0, 0x2 },
	{ "INDIA", 0, 0, 0, 0, 0, 0x3, 0, 0x4 },
	{ "INDONESIA", 0, 0, 0, 0, 0, 0x3, 0, 0 },
	{ "IRELAND", 0, 1, 0, 0, 1, 0x3, 0, 0x2 },
	{ "ISRAEL", 0, 0, 0, 0, 1, 0x3, 0, 0x2 },
	{ "ITALY", 0, 1, 0, 0, 1, 0x3, 0, 0x2 },
	{ "JAPAN", 0, 0, 0, 0, 0, 0, 0x3, 0 },
	{ "JORDAN", 0, 0, 0, 0, 0, 0, 0x3, 0 },
	{ "KAZAKHSTAN", 0, 0, 0, 0, 0, 0x3, 0 },
	{ "KUWAIT", 0, 0, 0, 0, 0, 0x3, 0, 0 },
	{ "LATVIA", 0, 0, 0, 0, 1, 0x3, 0, 0x2 },
	{ "LEBANON", 0, 0, 0, 0, 1, 0x3, 0, 0x2 },
	{ "LUXEMBOURG", 0, 1, 0, 0, 1, 0x3, 0, 0x2 },
	{ "MACAO", 0, 0, 0, 0, 0, 0x3, 0, 0 },
	{ "MALAYSIA", 0, 0, 0, 0, 0, 0, 0x3, 0 },	/* Current loop >= 20ma */
	{ "MALTA", 0, 0, 0, 0, 1, 0x3, 0, 0x2 },
	{ "MEXICO", 0, 0, 0, 0, 0, 0x3, 0, 0 },
	{ "MOROCCO", 0, 0, 0, 0, 1, 0x3, 0, 0x2 },
	{ "NETHERLANDS", 0, 1, 0, 0, 1, 0x3, 0, 0x2 },
	{ "NEWZEALAND", 0, 0, 0, 0, 0, 0x3, 0, 0x4 },
	{ "NIGERIA", 0, 0, 0, 0, 0x1, 0x3, 0, 0x2 },
	{ "NORWAY", 0, 1, 0, 0, 1, 0x3, 0, 0x2 },
	{ "OMAN", 0, 0, 0, 0, 0, 0, 0x3, 0 },
	{ "PAKISTAN", 0, 0, 0, 0, 0, 0, 0x3, 0 },
	{ "PERU", 0, 0, 0, 0, 0, 0x3, 0, 0 },
	{ "PHILIPPINES", 0, 0, 0, 0, 0, 0, 0x3, 0 },
	{ "POLAND", 0, 0, 1, 1, 0, 0x3, 0, 0 },
	{ "PORTUGAL", 0, 1, 0, 0, 1, 0x3, 0, 0x2 },
	{ "ROMANIA", 0, 0, 0, 0, 0, 3, 0, 0 },
	{ "RUSSIA", 0, 0, 0, 0, 0, 0, 0x3, 0 },
	{ "SAUDIARABIA", 0, 0, 0, 0, 0, 0x3, 0, 0 },
	{ "SINGAPORE", 0, 0, 0, 0, 0, 0x3, 0, 0 },
	{ "SLOVAKIA", 0, 0, 0, 0, 0, 0x3, 0, 0x3 },
	{ "SLOVENIA", 0, 0, 0, 0, 0, 0x3, 0, 0x2 },
	{ "SOUTHAFRICA", 1, 0, 1, 0, 0, 0x3, 0, 0x3 },
	{ "SOUTHKOREA", 0, 0, 0, 0, 0, 0x3, 0, 0 },
	{ "SPAIN", 0, 1, 0, 0, 1, 0x3, 0, 0x2 },
	{ "SWEDEN", 0, 1, 0, 0, 1, 0x3, 0, 0x2 },
	{ "SWITZERLAND", 0, 1, 0, 0, 1, 0x3, 0, 0x2 },
	{ "SYRIA", 0, 0, 0, 0, 0, 0, 0x3, 0 },
	{ "TAIWAN", 0, 0, 0, 0, 0, 0, 0x3, 0 },
	{ "THAILAND", 0, 0, 0, 0, 0, 0, 0x3, 0 },
	{ "UAE", 0, 0, 0, 0, 0, 0x3, 0, 0 },
	{ "UK", 0, 1, 0, 0, 1, 0x3, 0, 0x5 },
	{ "USA", 0, 0, 0, 0, 0, 0x3, 0, 0 },
	{ "YEMEN", 0, 0, 0, 0, 0, 0x3, 0, 0 },
};

#include <dahdi/kernel.h>

#ifdef LINUX26
#include <linux/moduleparam.h>
#endif

#define NUM_FXO_REGS 60

#define WC_MAX_IFACES 128

#define WC_CNTL    	0x00
#define WC_OPER		0x01
#define WC_AUXC    	0x02
#define WC_AUXD    	0x03
#define WC_MASK0   	0x04
#define WC_MASK1   	0x05
#define WC_INTSTAT 	0x06
#define WC_AUXR		0x07

#define WC_DMAWS	0x08
#define WC_DMAWI	0x0c
#define WC_DMAWE	0x10
#define WC_DMARS	0x18
#define WC_DMARI	0x1c
#define WC_DMARE	0x20

#define WC_AUXFUNC	0x2b
#define WC_SERCTL	0x2d
#define WC_FSCDELAY	0x2f

#define WC_REGBASE	0xc0

#define WC_SYNC		0x0
#define WC_TEST		0x1
#define WC_CS		0x2
#define WC_VER		0x3

/* ------------------------ Blackfin -------------------------*/

#include "GSM_module_SPI.h"//yn

#define RESET_BIT   4  /* GPIO bit tied to nRESET on Si chips */

#ifndef MULTI_SPI_FRAMEWORK
#include "fx.c"
#endif

#ifdef MULTI_SPI_FRAMEWORK
	#define FX_LED_RED     1
	#define FX_LED_GREEN   2
#endif

#ifdef CONFIG_4FX_SPI_INTERFACE
	#ifdef MULTI_SPI_FRAMEWORK
		#define __write_8bits(X, Y) multi_spi_write_8_bits(X->curcard, Y)
		#define __read_8bits(X) multi_spi_read_8_bits(X->curcard)
	#else
		#define __write_8bits(X, Y) bfsi_spi_write_8_bits(SPI_NCSA, Y)
		#define __read_8bits(X) bfsi_spi_read_8_bits(SPI_NCSA)
	#endif
#else
#define __write_8bits(X, Y) sport_tx_byte(SPI_NCSA, Y)
#define __read_8bits(X) sport_rx_byte(SPI_NCSA)
#endif

#define __reset_spi(X) do {} while(0)

/* ------------------------ Blackfin -------------------------*/


#define FLAG_EMPTY	0
#define FLAG_WRITE	1
#define FLAG_READ	2

#define RING_DEBOUNCE	64		/* Ringer Debounce (in ms) */
#define BATT_DEBOUNCE	64		/* Battery debounce (in ms) */
#define POLARITY_DEBOUNCE 64           /* Polarity debounce (in ms) */
#define BATT_THRESH	3		/* Anything under this is "no battery" */

#define OHT_TIMER		6000	/* How long after RING to retain OHT */

#define FLAG_3215	(1 << 0)

#ifdef SF_IP01
	#define NUM_CARDS 1
#else
	#define NUM_CARDS 8
#endif

#define MAX_ALARMS 10

#define MOD_TYPE_FXS	0
#define MOD_TYPE_FXO	1
#define MOD_TYPE_GSM    2 //added YN

#define MINPEGTIME	10 * 8		/* 30 ms peak to peak gets us no more than 100 Hz */
#define PEGTIME		50 * 8		/* 50ms peak to peak gets us rings of 10 Hz or more */
#define PEGCOUNT	5		/* 5 cycles of pegging means RING */

#define NUM_CAL_REGS 12

int outgoing_call_state=0;// YN
int incomming_call_state=0;// YN

int gsm_port=-1; //port at wich GSM1 is detected 
const int wcgsm_major = 241;
int  wcgsm_state;
int wcgsm_ioctl( struct inode *inodep, struct file *filep, unsigned int cmd, unsigned long arg);
int wcgsm_open(struct inode *inode, struct file *filp);
int wcgsm_release(struct inode *inode, struct file *filp);

/* Structure that declares the usual file access functions */
struct file_operations wcgsm_fops = {
        .open           = wcgsm_open,
        .release        = wcgsm_release,
        .ioctl          = wcgsm_ioctl
};

//Used for resheduling the TDM 8-bit slots
static int card2TDMslot_mapping[NUM_CARDS];// YN

struct calregs {
	unsigned char vals[NUM_CAL_REGS];
};

struct wcfxs {
	int   irq;
	char *variety;
	struct dahdi_span span;
	unsigned char ios;
	int usecount;
	unsigned int intcount;
	int dead;
	int pos;
	int flags[NUM_CARDS];
	int freeregion;
	int alt;
	int curcard;
	int cards;
	int cardflag;		/* Bit-map of present cards */
	spinlock_t lock;

	/* FXO Stuff */
	union {
		struct {
#ifdef AUDIO_RINGCHECK
			unsigned int pegtimer[NUM_CARDS];
			int pegcount[NUM_CARDS];
			int peg[NUM_CARDS];
			int ring[NUM_CARDS];
#else			
			int wasringing[NUM_CARDS];
#endif			
			int ringdebounce[NUM_CARDS];
			int offhook[NUM_CARDS];
			int battdebounce[NUM_CARDS];
			int nobatttimer[NUM_CARDS];
			int battery[NUM_CARDS];
		        int lastpol[NUM_CARDS];
		        int polarity[NUM_CARDS];
		        int polaritydebounce[NUM_CARDS];
		} fxo;
		struct {
			int oldrxhook[NUM_CARDS];
			int debouncehook[NUM_CARDS];
			int lastrxhook[NUM_CARDS];
			int debounce[NUM_CARDS];
			int ohttimer[NUM_CARDS];
			int idletxhookstate[NUM_CARDS];		/* IDLE changing hook state */
			int lasttxhook[NUM_CARDS];
			int palarms[NUM_CARDS];
			struct calregs calregs[NUM_CARDS];
		} fxs;
	} mod;

	/* Receive hook state and debouncing */
	int modtype[NUM_CARDS];

	unsigned long ioaddr;
	dma_addr_t 	readdma;
	dma_addr_t	writedma;
	volatile int *writechunk;					/* Double-word aligned write memory */
	volatile int *readchunk;					/* Double-word aligned read memory */
	struct dahdi_chan chans[NUM_CARDS];
	struct dahdi_chan *_chans[NUM_CARDS];
};

//YN =>from here
inline void __wcfxs_setcard(struct wcfxs *wc, int card);

static char *SIM_pin="0000";
static char SIM_Enter_string[20]="at+cpin=\"";

static int debug = 0;
static void wait_just_a_bit(int foo);

//Must be included after the wcfxs structure definition
#include "gsm_module.c"

//<=to here

struct wcfxs_desc {
	char *name;
	int flags;
};

static struct wcfxs_desc wcfxs_bf = { "Blackfin IP0x", 0 };
static int acim2tiss[16] = { 0x0, 0x1, 0x4, 0x5, 0x7, 0x0, 0x0, 0x6, 0x0, 0x0, 0x0, 0x2, 0x0, 0x3 };

static struct wcfxs *ifaces[WC_MAX_IFACES];

static void wcfxs_release(struct wcfxs *wc);

#ifdef OLD_DR
static void init_sport0(void);
static void init_dma_wc(void);
static int init_sport_interrupts(void);
static void enable_dma_sport0(void);
static void disable_sport0(void);
int wcfxs_proc_read(char *buf, char **start, off_t offset, 
		    int count, int *eof, void *data);/

#endif

static int robust = 0;
static int timingonly = 0;
static int lowpower = 0;
static int boostringer = 0;
static int _opermode = 0;
static char *opermode = "FCC";
static int fxshonormode = 0;
static struct wcfxs *devs;

/* added for uCasterisk/Blackfin */

static int loopback = 0;

static int wcfxs_init_ok = 0;

static int wcfxs_init_proslic(struct wcfxs *wc, int card,int TDM_channel_offset, int fast , int manual, int sane);

#ifdef AUDIO_RINGCHECK
static inline void ring_check(struct wcfxs *wc, int card)
{
	int x;
	short sample;
	if (wc->modtype[card] != MOD_TYPE_FXO)
		return;
	wc->mod.fxo.pegtimer[card] += DAHDI_CHUNKSIZE;
	for (x=0;x<DAHDI_CHUNKSIZE;x++) {
		/* Look for pegging to indicate ringing */
		sample = DAHDI_XLAW(wc->chans[card].readchunk[x], (&(wc->chans[card])));
		if ((sample > 10000) && (wc->mod.fxo.peg[card] != 1)) {
			if (debug > 1) printk("High peg!\n");
			if ((wc->mod.fxo.pegtimer[card] < PEGTIME) && (wc->mod.fxo.pegtimer[card] > MINPEGTIME))
				wc->mod.fxo.pegcount[card]++;
			wc->mod.fxo.pegtimer[card] = 0;
			wc->mod.fxo.peg[card] = 1;
		} else if ((sample < -10000) && (wc->mod.fxo.peg[card] != -1)) {
			if (debug > 1) printk("Low peg!\n");
			if ((wc->mod.fxo.pegtimer[card] < (PEGTIME >> 2)) && (wc->mod.fxo.pegtimer[card] > (MINPEGTIME >> 2)))
				wc->mod.fxo.pegcount[card]++;
			wc->mod.fxo.pegtimer[card] = 0;
			wc->mod.fxo.peg[card] = -1;
		}
	}
	if (wc->mod.fxo.pegtimer[card] > PEGTIME) {
		/* Reset pegcount if our timer expires */
		wc->mod.fxo.pegcount[card] = 0;
	}
	/* Decrement debouncer if appropriate */
	if (wc->mod.fxo.ringdebounce[card])
		wc->mod.fxo.ringdebounce[card]--;
	if (!wc->mod.fxo.offhook[card] && !wc->mod.fxo.ringdebounce[card]) {
		if (!wc->mod.fxo.ring[card] && (wc->mod.fxo.pegcount[card] > PEGCOUNT)) {
			/* It's ringing */
			if (debug)
				printk("RING on %d/%d!\n", wc->span.spanno, card + 1);
			if (!wc->mod.fxo.offhook[card])
				dahdi_hooksig(&wc->chans[card], DAHDI_RXSIG_RING);
			wc->mod.fxo.ring[card] = 1;
		}
		if (wc->mod.fxo.ring[card] && !wc->mod.fxo.pegcount[card]) {
			/* No more ring */
			if (debug)
				printk("NO RING on %d/%d!\n", wc->span.spanno, card + 1);
			dahdi_hooksig(&wc->chans[card], DAHDI_RXSIG_OFFHOOK);
			wc->mod.fxo.ring[card] = 0;
		}
	}
}
#endif
//static char digital_milliwatt[] = {0x1e,0x0b,0x0b,0x1e,0x9e,0x8b,0x8b,0x9e};
//int swi = 0;
//static short sw[] = {2048, 1023, -1024, -2048, -1023, 1024};

#define LOG_LEN 64
#define LOG_START 0
int logdma1[LOG_LEN];
int logdma2[20];
int logdma3[20];
int ilogdma = 0;
int serialnum = 0;
int notzero=0;

//Modified YN
//TDM slots are resheduled according to card2TDMslot_mapping
static inline void wcfxs_transmitprep(struct wcfxs *wc, u8 *writechunk)
{
	int x;
	//static u8 temp[8]={255,137,128,137,255,9,0,9};

	/* Calculate Transmission */
	dahdi_transmit(&wc->span);

	for (x=0;x<DAHDI_CHUNKSIZE;x++) {
		if (wc->cardflag & (1 << 7))
			writechunk[8*x+card2TDMslot_mapping[7]] = wc->chans[7].writechunk[x];//YN
		if (wc->cardflag & (1 << 6))
			writechunk[8*x+card2TDMslot_mapping[6]] = wc->chans[6].writechunk[x];//YN
		if (wc->cardflag & (1 << 5))
			writechunk[8*x+card2TDMslot_mapping[5]] = wc->chans[5].writechunk[x];//YN
		if (wc->cardflag & (1 << 4))
			writechunk[8*x+card2TDMslot_mapping[4]] = wc->chans[4].writechunk[x];//YN
		if (wc->cardflag & (1 << 3))
			writechunk[8*x+card2TDMslot_mapping[3]] = wc->chans[3].writechunk[x];//YN
		if (wc->cardflag & (1 << 2))
			writechunk[8*x+card2TDMslot_mapping[2]] = wc->chans[2].writechunk[x];//YN
		if (wc->cardflag & (1 << 1))
			writechunk[8*x+card2TDMslot_mapping[1]] = wc->chans[1].writechunk[x];//YN
		if (wc->cardflag & (1 << 0)) {
			writechunk[8*x+card2TDMslot_mapping[0]] = wc->chans[0].writechunk[x];//YN
		}
	}
}

//Modified YN
//TDM slots are resheduled according to card2TDMslot_mapping
static inline void wcfxs_receiveprep(struct wcfxs *wc, u8 *readchunk)
{
	int x;
	//int echo_before;

	//memset(readchunk, 0, 64);
	for (x=0;x<DAHDI_CHUNKSIZE;x++) {
		if (wc->cardflag & (1 << 7))
			wc->chans[7].readchunk[x] = readchunk[8*x+card2TDMslot_mapping[7]];//YN
		if (wc->cardflag & (1 << 6))
			wc->chans[6].readchunk[x] = readchunk[8*x+card2TDMslot_mapping[6]];//YN
		if (wc->cardflag & (1 << 5))
			wc->chans[5].readchunk[x] = readchunk[8*x+card2TDMslot_mapping[5]];//YN
		if (wc->cardflag & (1 << 4))
			wc->chans[4].readchunk[x] = readchunk[8*x+card2TDMslot_mapping[4]];//YN
		if (wc->cardflag & (1 << 3))
			wc->chans[3].readchunk[x] = readchunk[8*x+card2TDMslot_mapping[3]];//YN
		if (wc->cardflag & (1 << 2))
			wc->chans[2].readchunk[x] = readchunk[8*x+card2TDMslot_mapping[2]];//YN
		if (wc->cardflag & (1 << 1))
			wc->chans[1].readchunk[x] = readchunk[8*x+card2TDMslot_mapping[1]];//YN
		if (wc->cardflag & (1 << 0))
			wc->chans[0].readchunk[x] = readchunk[8*x+card2TDMslot_mapping[0]];//YN
	}
#ifdef AUDIO_RINGCHECK
	for (x=0;x<wc->cards;x++)
		ring_check(wc, x);
#endif		
	/* XXX We're wasting 8 taps.  We should get closer :( */
	//echo_before = cycles();
	for (x=0;x<wc->cards;x++) {
		if (wc->cardflag & (1 << x))
			dahdi_ec_chunk(&wc->chans[x], wc->chans[x].readchunk, wc->chans[x].writechunk);
	}

	//echo_sams = cycles() - echo_before;
	dahdi_receive(&wc->span);
}

/* we have only one card at the moment */
inline void __wcfxs_setcard(struct wcfxs *wc, int card)
{
	if (wc->curcard != card) {
#ifndef SF_IP01
           #ifdef DR_DONT_NEED
	   __wcfxs_setcreg(wc, WC_CS, (1 << card));
           #endif
	   if (card < 4) {
	     #ifdef CONFIG_4FX_SPI_INTERFACE
		 	#ifndef MULTI_SPI_FRAMEWORK
			     bfsi_spi_write_8_bits(SPI_NCSB, card+1);
			#endif
	     #else
	     sport_tx_byte(SPI_NCSB, card+1);
	     #endif
	   }
	   else {
	     #ifdef CONFIG_4FX_SPI_INTERFACE
		 	#ifndef MULTI_SPI_FRAMEWORK
		    	bfsi_spi_write_8_bits(SPI_NCSB, 0x40 + (card-4) + 1);
			#endif
	     #else
	     sport_tx_byte(SPI_NCSB, 0x40 + (card-4) + 1);
	     #endif
	   }
#endif
	   wc->curcard = card;
	}
}

static void __wcfxs_setreg(struct wcfxs *wc, int card, unsigned char reg, unsigned char value)
{
	__wcfxs_setcard(wc, card);
	if (wc->modtype[card] == MOD_TYPE_FXO) {
		__write_8bits(wc, 0x20);
		__write_8bits(wc, reg & 0x7f);
	} else {
	  //#define DAISY
	        #ifdef DAISY
	        __write_8bits(wc, 0x01); // 3210 daisy chain mode
		#endif
		__write_8bits(wc, reg & 0x7f);
	}
	__write_8bits(wc, value);
}

static void wcfxs_setreg(struct wcfxs *wc, int card, unsigned char reg, unsigned char value)
{
	unsigned long flags;
	spin_lock_irqsave(&wc->lock, flags);
	__wcfxs_setreg(wc, card, reg, value);
	spin_unlock_irqrestore(&wc->lock, flags);
}

static inline void wcfxs_set_led(struct wcfxs *wc, int port, int colour)
{
	unsigned long flags;
	spin_lock_irqsave(&wc->lock, flags);
#ifndef MULTI_SPI_FRAMEWORK
	fx_set_led(port, colour);
#ifndef SF_IP01
	//IP01 need not set wc->curcard to -1, using directly PFx
	wc->curcard = -1;                         /* leds mess up current card setting */	
#endif
#endif
	spin_unlock_irqrestore(&wc->lock, flags);
}

static unsigned char __wcfxs_getreg(struct wcfxs *wc, int card, unsigned char reg)
{
	__wcfxs_setcard(wc, card);
	if (wc->modtype[card] == MOD_TYPE_FXO) {
		__write_8bits(wc, 0x60);
		__write_8bits(wc, reg & 0x7f);
	} else {
	        #ifdef DAISY
	        __write_8bits(wc, 0x01); // 3210 daisy chain mode
		#endif
		__write_8bits(wc, reg | 0x80);
	}
	return __read_8bits(wc);
}

static inline void reset_spi(struct wcfxs *wc, int card)
{
	unsigned long flags;
	spin_lock_irqsave(&wc->lock, flags);
	__wcfxs_setcard(wc, card);
	__reset_spi(wc);
	__reset_spi(wc);
	spin_unlock_irqrestore(&wc->lock, flags);
}

static unsigned char wcfxs_getreg(struct wcfxs *wc, int card, unsigned char reg)
{
	unsigned long flags;
	unsigned char res;
	spin_lock_irqsave(&wc->lock, flags); 
	res = __wcfxs_getreg(wc, card, reg);
	spin_unlock_irqrestore(&wc->lock, flags);
	return res;
}

static int __wait_access(struct wcfxs *wc, int card)
{
    unsigned char data;
    long origjiffies;
    int count = 0;

    #define MAX 6000 /* attempts */


    origjiffies = jiffies;
    /* Wait for indirect access */
    while (count++ < MAX)
	 {
		data = __wcfxs_getreg(wc, card, I_STATUS);

		if (!data)
			return 0;

	 }

    if(count > (MAX-1)) printk(" ##### Loop error (%02x) #####\n", data);

	return 0;
}

static unsigned char translate_3215(unsigned char address)
{
	int x;
	for (x=0;x<sizeof(indirect_regs)/sizeof(indirect_regs[0]);x++) {
		if (indirect_regs[x].address == address) {
			address = indirect_regs[x].altaddr;
			break;
		}
	}
	return address;
}

static int wcfxs_proslic_setreg_indirect(struct wcfxs *wc, int card, unsigned char address, unsigned short data)
{
	unsigned long flags;
	int res = -1;
	/* Translate 3215 addresses */
	if (wc->flags[card] & FLAG_3215) {
		address = translate_3215(address);
		if (address == 255)
			return 0;
	}
	spin_lock_irqsave(&wc->lock, flags);
	if(!__wait_access(wc, card)) {
		__wcfxs_setreg(wc, card, IDA_LO,(unsigned char)(data & 0xFF));
		__wcfxs_setreg(wc, card, IDA_HI,(unsigned char)((data & 0xFF00)>>8));
		__wcfxs_setreg(wc, card, IAA,address);
		res = 0;
	};
	spin_unlock_irqrestore(&wc->lock, flags);
	return res;
}

static int wcfxs_proslic_getreg_indirect(struct wcfxs *wc, int card, unsigned char address)
{ 
	unsigned long flags;
	int res = -1;
	char *p=NULL;
	/* Translate 3215 addresses */
	if (wc->flags[card] & FLAG_3215) {
		address = translate_3215(address);
		if (address == 255)
			return 0;
	}
	spin_lock_irqsave(&wc->lock, flags);
	if (!__wait_access(wc, card)) {
		__wcfxs_setreg(wc, card, IAA, address);
		if (!__wait_access(wc, card)) {
			unsigned char data1, data2;
			data1 = __wcfxs_getreg(wc, card, IDA_LO);
			data2 = __wcfxs_getreg(wc, card, IDA_HI);
			res = data1 | (data2 << 8);
		} else
			p = "Failed to wait inside\n";
	} else
		p = "failed to wait\n";
	spin_unlock_irqrestore(&wc->lock, flags);
	if (p)
		printk(p);
	return res;
}

static int wcfxs_proslic_init_indirect_regs(struct wcfxs *wc, int card)
{
	unsigned char i;

	for (i=0; i<sizeof(indirect_regs) / sizeof(indirect_regs[0]); i++)
	{
		if(wcfxs_proslic_setreg_indirect(wc, card, indirect_regs[i].address,indirect_regs[i].initial))
			return -1;
	}

	return 0;
}

static int wcfxs_proslic_verify_indirect_regs(struct wcfxs *wc, int card)
{ 
	int passed = 1;
	unsigned short i, initial;
	int j;

	for (i=0; i<sizeof(indirect_regs) / sizeof(indirect_regs[0]); i++) 
	{
		if((j = wcfxs_proslic_getreg_indirect(wc, card, (unsigned char) indirect_regs[i].address)) < 0) {
			printk("Failed to read indirect register %d\n", i);
			return -1;
		}
		initial= indirect_regs[i].initial;

		if ( j != initial && (!(wc->flags[card] & FLAG_3215) || (indirect_regs[i].altaddr != 255)))
		{
			 printk("!!!!!!! %s  iREG %X = %X  should be %X\n",
				indirect_regs[i].name,indirect_regs[i].address,j,initial );
			 passed = 0;
		}	
	}

    if (passed) {
		if (debug)
			printk("Init Indirect Registers completed successfully.\n");
    } else {
		printk(" !!!!! Init Indirect Registers UNSUCCESSFULLY.\n");
		return -1;
    }
    return 0;
}

static inline void wcfxs_voicedaa_check_hook(struct wcfxs *wc, int card)
{
#ifndef AUDIO_RINGCHECK
	unsigned char res;
#endif	
	signed char b;

#ifndef AUDIO_RINGCHECK
	if (!wc->mod.fxo.offhook[card]) {
		res = wcfxs_getreg(wc, card, 5);
		if ((res & 0x60) && wc->mod.fxo.battery[card]) {
			wc->mod.fxo.ringdebounce[card] += (DAHDI_CHUNKSIZE * NUM_CARDS);
			if (wc->mod.fxo.ringdebounce[card] >= DAHDI_CHUNKSIZE * 64) {
				if (!wc->mod.fxo.wasringing[card]) {
					wc->mod.fxo.wasringing[card] = 1;
					dahdi_hooksig(&wc->chans[card], DAHDI_RXSIG_RING);
					wcfxs_set_led(wc, card+1, FX_LED_GREEN);
					if (debug>=2)
						printk("RING on %d/%d!\n", wc->span.spanno, card + 1);
				}
				wc->mod.fxo.ringdebounce[card] = DAHDI_CHUNKSIZE * 64;
			}
		} else {

			wc->mod.fxo.ringdebounce[card] -= DAHDI_CHUNKSIZE;
			if (wc->mod.fxo.ringdebounce[card] <= 0) {
				if (wc->mod.fxo.wasringing[card]) {
					wc->mod.fxo.wasringing[card] =0;
					dahdi_hooksig(&wc->chans[card], DAHDI_RXSIG_OFFHOOK);
					wcfxs_set_led(wc, card+1, FX_LED_RED);
					/* leds mess up current card setting */
					wc->curcard = -1;
					if (debug>=2)
						printk("NO RING on %d/%d!\n", wc->span.spanno, card + 1);
				}
				wc->mod.fxo.ringdebounce[card] = 0;
			}
				
		}
	}
#endif
	b = wcfxs_getreg(wc, card, 29);
#if 0 
	{
		static int count = 0;
		if (!(count++ % 100)) {
			printk("Card %d: Voltage: %d  Debounce %d\n", card + 1, 
			       b, wc->mod.fxo.battdebounce[card]);
		}
	}
#endif	
	if (abs(b) < BATT_THRESH) {
		wc->mod.fxo.nobatttimer[card]++;
#if 0
		if (wc->mod.fxo.battery[card])
			printk("Battery loss: %d (%d debounce)\n", b, wc->mod.fxo.battdebounce[card]);
#endif
		if (wc->mod.fxo.battery[card] && !wc->mod.fxo.battdebounce[card]) {
			if (debug>=2)
				printk("NO BATTERY on %d/%d!\n", wc->span.spanno, card + 1);
			wc->mod.fxo.battery[card] =  0;
#ifdef	JAPAN
			if ((!wc->ohdebounce) && wc->offhook) {
				dahdi_hooksig(&wc->chans[card], DAHDI_RXSIG_ONHOOK);
				if (debug)
					printk("Signalled On Hook\n");
#ifdef	ZERO_BATT_RING
				wc->onhook++;
#endif
			}
#else
			dahdi_hooksig(&wc->chans[card], DAHDI_RXSIG_ONHOOK);
#endif
			wc->mod.fxo.battdebounce[card] = BATT_DEBOUNCE;
		} else if (!wc->mod.fxo.battery[card])
			wc->mod.fxo.battdebounce[card] = BATT_DEBOUNCE;
	} else if (abs(b) > BATT_THRESH) {
		if (!wc->mod.fxo.battery[card] && !wc->mod.fxo.battdebounce[card]) {
			if (debug>=2)
				printk("BATTERY on %d/%d (%s)!\n", wc->span.spanno, card + 1, 
					(b < 0) ? "-" : "+");			    
#ifdef	ZERO_BATT_RING
			if (wc->onhook) {
				wc->onhook = 0;
				dahdi_hooksig(&wc->chans[card], DAHDI_RXSIG_OFFHOOK);
				if (debug)
					printk("Signalled Off Hook\n");
			}
#else
			dahdi_hooksig(&wc->chans[card], DAHDI_RXSIG_OFFHOOK);
#endif
			wc->mod.fxo.battery[card] = 1;
			wc->mod.fxo.nobatttimer[card] = 0;
			wc->mod.fxo.battdebounce[card] = BATT_DEBOUNCE;
		} else if (wc->mod.fxo.battery[card])
			wc->mod.fxo.battdebounce[card] = BATT_DEBOUNCE;

		if (wc->mod.fxo.lastpol[card] >= 0) {
		    if (b < 0) {
			wc->mod.fxo.lastpol[card] = -1;
			wc->mod.fxo.polaritydebounce[card] = POLARITY_DEBOUNCE;
		    }
		} 
		if (wc->mod.fxo.lastpol[card] <= 0) {
		    if (b > 0) {
			wc->mod.fxo.lastpol[card] = 1;
			wc->mod.fxo.polaritydebounce[card] = POLARITY_DEBOUNCE;
		    }
		}
	} else {
		/* It's something else... */
		wc->mod.fxo.battdebounce[card] = BATT_DEBOUNCE;
	}
	if (wc->mod.fxo.battdebounce[card])
		wc->mod.fxo.battdebounce[card] -= (NUM_CARDS/4);
	if (wc->mod.fxo.polaritydebounce[card]) {
	        wc->mod.fxo.polaritydebounce[card]--;
		if (wc->mod.fxo.polaritydebounce[card] < 1) {
		    if (wc->mod.fxo.lastpol[card] != wc->mod.fxo.polarity[card]) {
			if (debug>=2)
				printk("%lu Polarity reversed (%d -> %d)\n", jiffies, 
			       wc->mod.fxo.polarity[card], 
			       wc->mod.fxo.lastpol[card]);
			if (wc->mod.fxo.polarity[card])
			    dahdi_qevent_lock(&wc->chans[card], DAHDI_EVENT_POLARITY);
			wc->mod.fxo.polarity[card] = wc->mod.fxo.lastpol[card];
		    }
		}
	}
}

static inline void wcfxs_proslic_check_hook(struct wcfxs *wc, int card)
{
	char res;
	int hook;

	/* For some reason we have to debounce the
	   hook detector.  */
	
	res = wcfxs_getreg(wc, card, 68);
	hook = (res & 1);
	if (hook != wc->mod.fxs.lastrxhook[card]) {
		/* Reset the debounce (must be multiple of 4ms) */
		wc->mod.fxs.debounce[card] = 8 * (4 * 8);
	} else {
		if (wc->mod.fxs.debounce[card] > 0) {
			wc->mod.fxs.debounce[card]-= 4 * DAHDI_CHUNKSIZE;
			if (!wc->mod.fxs.debounce[card]) {
				wc->mod.fxs.debouncehook[card] = hook;
			}
			if (!wc->mod.fxs.oldrxhook[card] && wc->mod.fxs.debouncehook[card]) {
				/* Off hook */
				if (debug>=2)
					printk("wcfxs: Card %d Going off hook\n", card);
				dahdi_hooksig(&wc->chans[card], DAHDI_RXSIG_OFFHOOK);
				if (robust)
					wcfxs_init_proslic(wc, card,card2TDMslot_mapping[card], 1, 0, 1);//YN
				wc->mod.fxs.oldrxhook[card] = 1;
			
			} else if (wc->mod.fxs.oldrxhook[card] && !wc->mod.fxs.debouncehook[card]) {
				/* On hook */
				if (debug>=2)
					printk("wcfxs: Card %d Going on hook\n", card);
				dahdi_hooksig(&wc->chans[card], DAHDI_RXSIG_ONHOOK);
				wc->mod.fxs.oldrxhook[card] = 0;
			}
		}
	}
	wc->mod.fxs.lastrxhook[card] = hook;

	
}

static inline void wcfxs_proslic_recheck_sanity(struct wcfxs *wc, int card)
{
	int res;
	/* Check loopback */
	res = wcfxs_getreg(wc, card, 8);
	if (res) {
		printk("Ouch, part reset, quickly restoring reality (%d)\n", card);
		wcfxs_init_proslic(wc, card,card2TDMslot_mapping[card],1, 0, 1);
	} else {
		res = wcfxs_getreg(wc, card, 64);
		if (!res && (res != wc->mod.fxs.lasttxhook[card])) {
			if (wc->mod.fxs.palarms[card]++ < MAX_ALARMS) {
				printk("Power alarm on module %d, resetting!\n", card + 1);
				if (wc->mod.fxs.lasttxhook[card] == 4)
					wc->mod.fxs.lasttxhook[card] = 1;
				wcfxs_setreg(wc, card, 64, wc->mod.fxs.lasttxhook[card]);
			} else {
				if (wc->mod.fxs.palarms[card] == MAX_ALARMS)
					printk("Too many power alarms on card %d, NOT resetting!\n", card + 1);
			}
		}
	}
}

static void work_interrupt_processing(struct work_struct *test);
static DECLARE_WORK(work_interrupt, work_interrupt_processing);

/* handles regular interrupt processing, called every time we get a DMA
   interrupt which is every 1ms with DAHDI_CHUNKSIZE == 8 */
void regular_interrupt_processing(u8 *read_samples, u8 *write_samples) {
  struct wcfxs *wc = devs;

        /* handle speech samples */
        wcfxs_transmitprep(wc, write_samples);
        wcfxs_receiveprep(wc, read_samples);

        schedule_work(&work_interrupt);
}

/* The work interrupt is a soft IRQ that can properly communicate with
 * the SPI kernel drivers and sleep without crashing the kernel */
static void work_interrupt_processing(struct work_struct *test) {
  struct wcfxs *wc = devs;

        int x;

        wc->intcount++;
        x = wc->intcount % NUM_CARDS;


        /* as ISR is started before chips initialised we need this test
           to ensure we don't test the hook switch and ring detect
           before chips initialised */

        if (wcfxs_init_ok) {

          /* check hook switch (FXS) and ringing (FXO) */

          if ((x < wc->cards) && (wc->cardflag & (1 << x))) {
            if (wc->modtype[x] == MOD_TYPE_FXS) {
            	wcfxs_proslic_check_hook(wc, x);
//#define DR_DONT_NEED
#ifdef DR_DONT_NEED
              if (!(wc->intcount & 0xfc))
                wcfxs_proslic_recheck_sanity(wc, x);
#endif
	    }
	    else {
			if ((wc->modtype[x] == MOD_TYPE_FXO) && (port_type[x] != 'G')) {
              	/* ring detection, despite name */
            	wcfxs_voicedaa_check_hook(wc, x);
            }
			else{
				if (wc->modtype[x] == MOD_TYPE_FXO && !sms_transaction) 
					wcfxs_gsm_control(wc, x);
			}
		 }
          }
	
	//Check if we have new SMS once each 10 sec
	if (!(wc->intcount % 60000)) {
		if (gsm_port>-1 && gsm_port<FX_MAX_PORTS && SMS.count < SMS_MAX_LOCAL_SMS &&      //SMS buffer not full 
		    !incomming_call_state && !outgoing_call_state &&				  //We are not in a GSM conversation
		    !sms_transaction){				 				  //Don't sending SMS right now
			sms_transaction=1;
			if (!sms_read(wc,gsm_port, &(SMS.sms[SMS.count]))) SMS.count++;
			sms_transaction=0;
		} 	
	}

#ifdef DR_DONT_NEED
          if (!(wc->intcount % 10000)) {
            /* Accept an alarm once per 10 seconds */
            for (x=0;x<4;x++)
              if (wc->modtype[x] == MOD_TYPE_FXS) {
                if (wc->mod.fxs.palarms[x])
                  wc->mod.fxs.palarms[x]--;
              }
          }
#endif
        }
}



static int wcfxs_voicedaa_insane(struct wcfxs *wc, int card)
{
	int blah;
	blah = wcfxs_getreg(wc, card, 2);
	
	if (debug) {
		printk("Testing for DAA...\n");
	}
	if (blah != 0x3) {
		printk("  DAA not found! (blah = 0x%x)\n", blah);
		return -2;
	}
	blah = wcfxs_getreg(wc, card, 11);
	if (debug)
		printk("  VoiceDAA System: %02x\n", blah & 0xf);
	return 0;
}

static int wcfxs_proslic_insane(struct wcfxs *wc, int card)
{
	int blah,insane_report;
	insane_report=0;

	blah = wcfxs_getreg(wc, card, 0);
	if (debug) {
		printk("Testing for ProSLIC card = %d blah = 0x%x\n", card, blah);
	}

#if 0
	if ((blah & 0x30) >> 4) {
		printk("ProSLIC on module %d is not a 3210.\n", card);
		return -1;
	}
#endif
	if (((blah & 0xf) == 0) || ((blah & 0xf) == 0xf)) {
		if (debug) {
			    printk("  ProSLIC not loaded...\n");
		}
		return -1;
	}
	if (debug) {
		printk("ProSLIC module %d, product %d, version %d\n", card, (blah & 0x30) >> 4, (blah & 0xf));
	}
	if ((blah & 0xf) < 2) {
		printk("ProSLIC 3210 version %d is too old\n", blah & 0xf);
		return -1;
	}
	if (wcfxs_getreg(wc, card, 1) & 0x80){
		/* ProSLIC 3215, not a 3210 */
		wc->flags[card] |= FLAG_3215;
              printk("ProSLIC module is Si3215\n");
	}
	blah = wcfxs_getreg(wc, card, 8);
	if (blah != 0x2) {
		printk("ProSLIC on module %d insane (1) %d should be 2\n", card, blah);
		return -1;
	} else if ( insane_report)
		printk("ProSLIC on module %d Reg 8 Reads %d Expected is 0x2\n",card,blah);

	blah = wcfxs_getreg(wc, card, 64);
	if (blah != 0x0) {
		printk("ProSLIC on module %d insane (2)\n", card);
		return -1;
	} else if ( insane_report)
		printk("ProSLIC on module %d Reg 64 Reads %d Expected is 0x0\n",card,blah);

	blah = wcfxs_getreg(wc, card, 11);
	if (blah != 0x33) {
		printk("ProSLIC on module %d insane (3)\n", card);
		return -1;
	} else if ( insane_report)
		printk("ProSLIC on module %d Reg 11 Reads %d Expected is 0x33\n",card,blah);

	/* Just be sure it's setup right. */
	wcfxs_setreg(wc, card, 30, 0);

	if (debug) 
		printk("ProSLIC on module %d seems sane.\n", card);
	return 0;
}

static int wcfxs_proslic_powerleak_test(struct wcfxs *wc, int card)
{
	unsigned long origjiffies;
	unsigned char vbat;

	/* Turn off linefeed */
	wcfxs_setreg(wc, card, 64, 0);

	/* Power down */
	wcfxs_setreg(wc, card, 14, 0x10);

	/* Wait for one second */
	origjiffies = jiffies;

	while((vbat = wcfxs_getreg(wc, card, 82)) > 0x6) {
		if ((jiffies - origjiffies) >= (HZ/2))
			break;;
	}

	if (vbat < 0x06) {
		printk("Excessive leakage detected on module %d: %d volts (%02x) after %d ms\n", card,
		       376 * vbat / 1000, vbat, (int)((jiffies - origjiffies) * 1000 / HZ));
		return -1;
	} else if (debug) {
		printk("Post-leakage voltage: %d volts\n", 376 * vbat / 1000);
	}
	return 0;
}

static int wcfxs_powerup_proslic(struct wcfxs *wc, int card, int fast)
{
	unsigned char vbat;
	unsigned long origjiffies;
	int lim;

	/* Set period of DC-DC converter to 1/64 khz */
	wcfxs_setreg(wc, card, 92, 0xff /* was 0xff */);

	/* Wait for VBat to powerup */
	origjiffies = jiffies;

	/* Disable powerdown */
	wcfxs_setreg(wc, card, 14, 0);

	/* If fast, don't bother checking anymore */
	if (fast)
		return 0;

	while((vbat = wcfxs_getreg(wc, card, 82)) < 0xc0) {
		/* Wait no more than 500ms */
		if ((jiffies - origjiffies) > HZ/2) {
			break;
		}
	}

	if (debug)
	{
		printk("reg 0: 0x%x \n", wcfxs_getreg(wc, card, 0));
		printk("reg 14: 0x%x \n", wcfxs_getreg(wc, card, 14));
		printk("reg 74: 0x%x \n", wcfxs_getreg(wc, card, 74));
		printk("reg 80: 0x%x \n", wcfxs_getreg(wc, card, 80));
		printk("reg 81: 0x%x \n", wcfxs_getreg(wc, card, 81));
		printk("reg 92: 0x%x \n", wcfxs_getreg(wc, card, 92));
		printk("reg 82: 0x%x \n", wcfxs_getreg(wc, card, 82));
		printk("reg 83: 0x%x \n", wcfxs_getreg(wc, card, 83));
	}
        //return -1;

	if (vbat < 0xc0) {
		printk("ProSLIC on module %d failed to powerup within %d ms (%d mV only)\n\n -- DID YOU REMEMBER TO PLUG IN THE HD POWER CABLE TO THE TDM400P??\n",
		       card, (int)(((jiffies - origjiffies) * 1000 / HZ)),
			vbat * 375);
		return -1;
	} else if (debug) {
		printk("ProSLIC on module %d powered up to -%d volts (%02x) in %d ms\n",
		       card, vbat * 376 / 1000, vbat, (int)(((jiffies - origjiffies) * 1000 / HZ)));
	}

        /* Proslic max allowed loop current, reg 71 LOOP_I_LIMIT */
        /* If out of range, just set it to the default value     */
        lim = (loopcurrent - 20) / 3;
        if ( loopcurrent > 41 ) {
                lim = 0;
                if (debug)
                        printk("Loop current out of range! Setting to default 20mA!\n");
        }
        else if (debug)
                        printk("Loop current set to %dmA!\n",(lim*3)+20);
        wcfxs_setreg(wc,card,LOOP_I_LIMIT,lim);

	/* Engage DC-DC converter */
	wcfxs_setreg(wc, card, 93, 0x19 /* was 0x19 */);
#if 0
	origjiffies = jiffies;
	while(0x80 & wcfxs_getreg(wc, card, 93)) {
		if ((jiffies - origjiffies) > 2 * HZ) {
			printk("Timeout waiting for DC-DC calibration on module %d\n", card);
			return -1;
		}
	}

#if 0
	/* Wait a full two seconds */
	while((jiffies - origjiffies) < 2 * HZ);

	/* Just check to be sure */
	vbat = wcfxs_getreg(wc, card, 82);
	printk("ProSLIC on module %d powered up to -%d volts (%02x) in %d ms\n",
		       card, vbat * 376 / 1000, vbat, (int)(((jiffies - origjiffies) * 1000 / HZ)));
#endif
#endif
	return 0;

}

static int wcfxs_proslic_manual_calibrate(struct wcfxs *wc, int card){
	unsigned long origjiffies;
	unsigned char i;

	printk("Start manual calibration\n");

	wcfxs_setreg(wc, card, 21, 0);//(0)  Disable all interupts in DR21
	wcfxs_setreg(wc, card, 22, 0);//(0)Disable all interupts in DR21
	wcfxs_setreg(wc, card, 23, 0);//(0)Disable all interupts in DR21
	wcfxs_setreg(wc, card, 64, 0);//(0)

	wcfxs_setreg(wc, card, 97, 0x18); //(0x18)Calibrations without the ADC and DAC offset and without common mode calibration.
	wcfxs_setreg(wc, card, 96, 0x47); //(0x47)	Calibrate common mode and differential DAC mode DAC + ILIM

	origjiffies=jiffies;
	while( wcfxs_getreg(wc,card,96)!=0 ){
		if((jiffies-origjiffies)>80)
			return -1;
	}
//Initialized DR 98 and 99 to get consistant results.
// 98 and 99 are the results registers and the search should have same intial conditions.

/*******************************The following is the manual gain mismatch calibration****************************/
/*******************************This is also available as a function *******************************************/
	// Delay 10ms
	origjiffies=jiffies; 
	while((jiffies-origjiffies)<1);
	wcfxs_proslic_setreg_indirect(wc, card, 88,0);
	wcfxs_proslic_setreg_indirect(wc,card,89,0);
	wcfxs_proslic_setreg_indirect(wc,card,90,0);
	wcfxs_proslic_setreg_indirect(wc,card,91,0);
	wcfxs_proslic_setreg_indirect(wc,card,92,0);
	wcfxs_proslic_setreg_indirect(wc,card,93,0);

	wcfxs_setreg(wc, card, 98,0x10); // This is necessary if the calibration occurs other than at reset time
	wcfxs_setreg(wc, card, 99,0x10);

	for ( i=0x1f; i>0; i--)
	{
		wcfxs_setreg(wc, card, 98,i);
		origjiffies=jiffies; 
		while((jiffies-origjiffies)<4);
		if((wcfxs_getreg(wc,card,88)) == 0)
			break;
	} // for

	for ( i=0x1f; i>0; i--)
	{
		wcfxs_setreg(wc, card, 99,i);
		origjiffies=jiffies; 
		while((jiffies-origjiffies)<4);
		if((wcfxs_getreg(wc,card,89)) == 0)
			break;
	}//for

/*******************************The preceding is the manual gain mismatch calibration****************************/
/**********************************The following is the longitudinal Balance Cal***********************************/
	wcfxs_setreg(wc,card,64,1);
	while((jiffies-origjiffies)<10); // Sleep 100?

	wcfxs_setreg(wc, card, 64, 0);
	wcfxs_setreg(wc, card, 23, 0x4);  // enable interrupt for the balance Cal
	wcfxs_setreg(wc, card, 97, 0x1); // this is a singular calibration bit for longitudinal calibration
	wcfxs_setreg(wc, card, 96,0x40);

	wcfxs_getreg(wc,card,96); /* Read Reg 96 just cause */

	wcfxs_setreg(wc, card, 21, 0xFF);
	wcfxs_setreg(wc, card, 22, 0xFF);
	wcfxs_setreg(wc, card, 23, 0xFF);

	/**The preceding is the longitudinal Balance Cal***/
	return(0);

}
#if 1
static int wcfxs_proslic_calibrate(struct wcfxs *wc, int card)
{
	unsigned long origjiffies;
	int x;

	printk("Start automatic calibration\n");

	/* Perform all calibrations */
	wcfxs_setreg(wc, card, 97, 0x1f);
	
	/* Begin, no speedup */
	wcfxs_setreg(wc, card, 96, 0x5f);

	/* Wait for it to finish */
	origjiffies = jiffies;
	while(wcfxs_getreg(wc, card, 96)) {
		if ((jiffies - origjiffies) > 2 * HZ) {
			printk("Timeout waiting for calibration of module %d\n", card);
			return -1;
		}
	}
	
	if (debug) {
		/* Print calibration parameters */
		printk("Calibration Vector Regs 98 - 107: \n");
		for (x=98;x<108;x++) {
			printk("%d: %02x\n", x, wcfxs_getreg(wc, card, x));
		}
	}
	return 0;
}
#endif

/* wait 'foo' jiffies then return.  

   DR - modified to use a better delay mechanism that the orginal
   busy-waiting method, which locked the kernel up for some rather
   long times (e.g. seconds), at least on non-preemptable kernels.
*/

static void wait_just_a_bit(int foo)
{
	set_current_state(TASK_INTERRUPTIBLE);
	schedule_timeout(foo);
}

//YN TDM_channel_offset parameter added
static int wcfxs_init_voicedaa(struct wcfxs *wc, int card, int TDM_channel_offset, int fast, int manual, int sane)//YN
{
	unsigned char reg16=0, reg26=0, reg30=0, reg31=0;
	long newjiffies;
	wc->modtype[card] = MOD_TYPE_FXO;
	
	//printk("Entering: wcfxs_init_voicedaa\n");

	/* Sanity check */
	if (!sane && wcfxs_voicedaa_insane(wc, card))
		return -2;

	/* Software reset */
	wcfxs_setreg(wc, card, 1, 0x80);

	/* Wait just a bit */
	wait_just_a_bit(HZ/10);

	/* Enable PCM, ulaw */
	wcfxs_setreg(wc, card, 33, 0x28);

	/* Set On-hook speed, Ringer impedence, and ringer threshold */
	reg16 |= (fxo_modes[_opermode].ohs << 6);
	reg16 |= (fxo_modes[_opermode].rz << 1);
	reg16 |= (fxo_modes[_opermode].rt);
	wcfxs_setreg(wc, card, 16, reg16);
	
	/* Set DC Termination:
	   Tip/Ring voltage adjust, minimum operational current, current limitation */
	reg26 |= (fxo_modes[_opermode].dcv << 6);
	reg26 |= (fxo_modes[_opermode].mini << 4);
	reg26 |= (fxo_modes[_opermode].ilim << 1);
	wcfxs_setreg(wc, card, 26, reg26);

	/* Set AC Impedence */
	reg30 = (fxo_modes[_opermode].acim);
	wcfxs_setreg(wc, card, 30, reg30);

	/* Misc. DAA parameters */
	reg31 = 0xa3;
	reg31 |= (fxo_modes[_opermode].ohs2 << 3);
	wcfxs_setreg(wc, card, 31, reg31);

	/* Set Transmit/Receive timeslot */
	wcfxs_setreg(wc, card, 34, ((TDM_channel_offset) * 8)+1);//YN // YN moved to +1 from FSYNC
	wcfxs_setreg(wc, card, 35, 0x00);
	wcfxs_setreg(wc, card, 36, ((TDM_channel_offset) * 8)+1);//YN // YN moved to +1 from FSYNC
	wcfxs_setreg(wc, card, 37, 0x00);

	/* Enable ISO-Cap */
	wcfxs_setreg(wc, card, 6, 0x00);

	/* Wait 1000ms for ISO-cap to come up */
	newjiffies = jiffies;
	newjiffies += 2 * HZ;
	while((jiffies < newjiffies) && !(wcfxs_getreg(wc, card, 11) & 0xf0))
		wait_just_a_bit(HZ/10);

	if (!(wcfxs_getreg(wc, card, 11) & 0xf0)) {
		printk("VoiceDAA did not bring up ISO link properly!\n");
		return -1;
	}
	if (debug)
		printk("  ISO-Cap is now up, line side: %02x rev %02x\n", 
		       wcfxs_getreg(wc, card, 11) >> 4,
		       (wcfxs_getreg(wc, card, 13) >> 2) & 0xf);

	/* Enable on-hook line monitor */
	wcfxs_setreg(wc, card, 5, 0x08);

	/* DR 6/1105: Debug code used to trap bad reads, 
	   see notes in __write_8bits 
	{
	        int i;
		unsigned char r;

		for(i=0; i<1000; i++) {
			r = wcfxs_getreg(wc, card, 31);
			if (r != 0xa3) {
				printk("bad read! Check if CS is going low\n");
				break;
			}

		}
       		printk("  1000 reads OK!\n");
	}
	*/
	
	/* Optional digital loopback, used for testing Blackfin DMA */

	if (loopback) {
		wcfxs_setreg(wc, card, 10, 0x01);
		printk("loopback enabled\n");
	}

	return 0;
}

//YN TDM_channel_offset parameter added
static int wcfxs_init_proslic(struct wcfxs *wc, int card, int TDM_channel_offset,int fast, int manual, int sane)
{

	unsigned short tmp[5];
	unsigned char r19;
	int x;
	int fxsmode=0;

	manual = 1;

	/* By default, don't send on hook */
	wc->mod.fxs.idletxhookstate [card] = 1;

	/* Sanity check the ProSLIC */
	if (!sane && wcfxs_proslic_insane(wc, card))
		return -2;
	
	if (sane) {
		/* Make sure we turn off the DC->DC converter to prevent anything from blowing up */
		wcfxs_setreg(wc, card, 14, 0x10);
	}

	if (wcfxs_proslic_init_indirect_regs(wc, card)) {
		printk(KERN_INFO "Indirect Registers failed to initialize on module %d.\n", card);
		return -1;
	}

	/* Clear scratch pad area */
	wcfxs_proslic_setreg_indirect(wc, card, 97,0);

	/* Clear digital loopback */
	wcfxs_setreg(wc, card, 8, 0);

	/* Revision C optimization */
	wcfxs_setreg(wc, card, 108, 0xeb);

	/* Disable automatic VBat switching for safety to prevent
	   Q7 from accidently turning on and burning out. */
	wcfxs_setreg(wc, card, 67, 0x17);

	/* Turn off Q7 */
	wcfxs_setreg(wc, card, 66, 1);

	/* Flush ProSLIC digital filters by setting to clear, while
	   saving old values */
	for (x=0;x<5;x++) {
		tmp[x] = wcfxs_proslic_getreg_indirect(wc, card, x + 35);
		wcfxs_proslic_setreg_indirect(wc, card, x + 35, 0x8000);
	}

	/* Power up the DC-DC converter */
	if (wcfxs_powerup_proslic(wc, card, fast)) {
		printk("Unable to do INITIAL ProSLIC powerup on module %d\n", card);
		return -1;
	}

	if (!fast) {

		/* Check for power leaks */
		if (wcfxs_proslic_powerleak_test(wc, card)) {
			printk("ProSLIC module %d failed leakage test.  Check for short circuit\n", card);
		}
		/* Power up again */
		if (wcfxs_powerup_proslic(wc, card, fast)) {
			printk("Unable to do FINAL ProSLIC powerup on module %d\n", card);
			return -1;
		}
#ifndef NO_CALIBRATION
		/* Perform calibration */
		if(manual) {
			if (wcfxs_proslic_manual_calibrate(wc, card)) {
				//printk("Proslic failed on Manual Calibration\n");
				if (wcfxs_proslic_manual_calibrate(wc, card)) {
					printk("Proslic Failed on Second Attempt to Calibrate Manually. (Try -DNO_CALIBRATION in Makefile)\n");
					return -1;
				}
				printk("Proslic Passed Manual Calibration on Second Attempt\n");
			}
		}
		else {
			if(wcfxs_proslic_calibrate(wc, card))  {
				//printk("ProSlic died on Auto Calibration.\n");
				if (wcfxs_proslic_calibrate(wc, card)) {
					printk("Proslic Failed on Second Attempt to Auto Calibrate\n");
					return -1;
				}
				printk("Proslic Passed Auto Calibration on Second Attempt\n");
			}
		}
		/* Perform DC-DC calibration */
		wcfxs_setreg(wc, card, 93, 0x99);
		r19 = wcfxs_getreg(wc, card, 107);
		if ((r19 < 0x2) || (r19 > 0xd)) {
			printk("DC-DC cal has a surprising direct 107 of 0x%02x!\n", r19);
			wcfxs_setreg(wc, card, 107, 0x8);
		}

		/* Save calibration vectors */
		for (x=0;x<NUM_CAL_REGS;x++)
			wc->mod.fxs.calregs[card].vals[x] = wcfxs_getreg(wc, card, 96 + x);
#endif

	} else {
		/* Restore calibration registers */
		for (x=0;x<NUM_CAL_REGS;x++)
			wcfxs_setreg(wc, card, 96 + x, wc->mod.fxs.calregs[card].vals[x]);
	}
	/* Calibration complete, restore original values */
	for (x=0;x<5;x++) {
		wcfxs_proslic_setreg_indirect(wc, card, x + 35, tmp[x]);
	}

	if (wcfxs_proslic_verify_indirect_regs(wc, card)) {
		printk(KERN_INFO "Indirect Registers failed verification.\n");
		return -1;
	}


#if 0
    /* Disable Auto Power Alarm Detect and other "features" */
    wcfxs_setreg(wc, card, 67, 0x0e);
    blah = wcfxs_getreg(wc, card, 67);
#endif

#if 0
    if (wcfxs_proslic_setreg_indirect(wc, card, 97, 0x0)) { // Stanley: for the bad recording fix
		 printk(KERN_INFO "ProSlic IndirectReg Died.\n");
		 return -1;
	}
#endif

    wcfxs_setreg(wc, card, 1, 0x28);
 	// U-Law 8-bit interface
    wcfxs_setreg(wc, card, 2, ((TDM_channel_offset) * 8)+1);    // Tx Start count low byte  0  // YN moved to +1 from FSYNC
    wcfxs_setreg(wc, card, 3, 0);    // Tx Start count high byte 0
    wcfxs_setreg(wc, card, 4, ((TDM_channel_offset) * 8)+1);    // Rx Start count low byte  0  //YN moved to +1 from FSYNC
    wcfxs_setreg(wc, card, 5, 0);    // Rx Start count high byte 0
    wcfxs_setreg(wc, card, 18, 0xff);     // clear all interrupt
    wcfxs_setreg(wc, card, 19, 0xff);
    wcfxs_setreg(wc, card, 20, 0xff);
    wcfxs_setreg(wc, card, 73, 0x04);
	if (fxshonormode) {
		fxsmode = acim2tiss[fxo_modes[_opermode].acim];
		wcfxs_setreg(wc, card, 10, 0x08 | fxsmode);
		if (fxo_modes[_opermode].ring_osc)
			wcfxs_proslic_setreg_indirect(wc, card, 20, fxo_modes[_opermode].ring_osc);
		if (fxo_modes[_opermode].ring_x)
			wcfxs_proslic_setreg_indirect(wc, card, 21, fxo_modes[_opermode].ring_x);
	}
    if (lowpower)
    	wcfxs_setreg(wc, card, 72, 0x10);

#if 0
    wcfxs_setreg(wc, card, 21, 0x00); 	// enable interrupt
    wcfxs_setreg(wc, card, 22, 0x02); 	// Loop detection interrupt
    wcfxs_setreg(wc, card, 23, 0x01); 	// DTMF detection interrupt
#endif

#if 0
    /* Enable loopback */
    wcfxs_setreg(wc, card, 8, 0x2);
    wcfxs_setreg(wc, card, 14, 0x0);
    wcfxs_setreg(wc, card, 64, 0x0);
    wcfxs_setreg(wc, card, 1, 0x08);
#endif

	/* Beef up Ringing voltage to 89V */
	if (boostringer) {
		if (wcfxs_proslic_setreg_indirect(wc, card, 21, 0x1d1)) 
			return -1;
		printk("Boosting ringinger on slot %d (89V peak)\n", card + 1);
	} else if (lowpower) {
		if (wcfxs_proslic_setreg_indirect(wc, card, 21, 0x108)) 
			return -1;
		printk("Reducing ring power on slot %d (50V peak)\n", card + 1);
	}
	wcfxs_setreg(wc, card, 64, 0x01);
	return 0;
}

static int wcfxs_ioctl(struct dahdi_chan *chan, unsigned int cmd, unsigned long data)
{
	struct wcfxs_stats stats;
	struct wcfxs_regs regs;
	struct wcfxs_regop regop;
	struct wcfxs *wc = chan->pvt;
	int x;

	switch (cmd) {
	case DAHDI_ONHOOKTRANSFER:
	        if (debug)
		    printk("wcfxs_ioctl, DAHDI_ONHOOKTRANSFER %d\n", chan->chanpos - 1);
		if (wc->modtype[chan->chanpos - 1] != MOD_TYPE_FXS)
			return -EINVAL;
		if (get_user(x, (int *)data))
			return -EFAULT;
		wc->mod.fxs.ohttimer[chan->chanpos - 1] = x << 3;

		if (lowpower < 2) {
		    /* DR: Set lowpower=2 to disable this code and run
		       FXS ports at minimum power.  Unfortunately, this
		       also disables FXS port Called ID transmit.  I don't
		       quite understand why, more work is required........

		       Note: this ioctl is meant for setting the onhook time for
		       transfers, not sure why we need to modify state of
		       SLIC here?
		    */
		    wc->mod.fxs.idletxhookstate[chan->chanpos - 1] = 0x2; /* OHT mode when idle */
		
		    if (wc->mod.fxs.lasttxhook[chan->chanpos - 1] == 0x1) {
				// Apply the change if appropriate 
				wc->mod.fxs.lasttxhook[chan->chanpos - 1] = 0x2;
				wcfxs_setreg(wc, chan->chanpos - 1, 64, wc->mod.fxs.lasttxhook[chan->chanpos - 1]);
		    }
		}

		break;
	case WCFXS_GET_STATS:
		if (wc->modtype[chan->chanpos - 1] == MOD_TYPE_FXS) {
			stats.tipvolt = wcfxs_getreg(wc, chan->chanpos - 1, 80) * -376;
			stats.ringvolt = wcfxs_getreg(wc, chan->chanpos - 1, 81) * -376;
			stats.batvolt = wcfxs_getreg(wc, chan->chanpos - 1, 82) * -376;
		} else if (wc->modtype[chan->chanpos - 1] == MOD_TYPE_FXO) {
			stats.tipvolt = (signed char)wcfxs_getreg(wc, chan->chanpos - 1, 29) * 1000;
			stats.ringvolt = (signed char)wcfxs_getreg(wc, chan->chanpos - 1, 29) * 1000;
			stats.batvolt = (signed char)wcfxs_getreg(wc, chan->chanpos - 1, 29) * 1000;
		} else 
			return -EINVAL;
		if (copy_to_user((struct wcfxs_stats *)data, &stats, sizeof(stats)))
			return -EFAULT;
		break;
	case WCFXS_GET_REGS:
		if (wc->modtype[chan->chanpos - 1] == MOD_TYPE_FXS) {
			for (x=0;x<NUM_INDIRECT_REGS;x++)
				regs.indirect[x] = wcfxs_proslic_getreg_indirect(wc, chan->chanpos -1, x);
			for (x=0;x<NUM_REGS;x++)
				regs.direct[x] = wcfxs_getreg(wc, chan->chanpos - 1, x);
		} else {
			memset(&regs, 0, sizeof(regs));
			for (x=0;x<NUM_FXO_REGS;x++)
				regs.direct[x] = wcfxs_getreg(wc, chan->chanpos - 1, x);
		}
		if (copy_to_user((struct wcfxs_regs *)data, &regs, sizeof(regs)))
			return -EFAULT;
		break;
	case WCFXS_SET_REG:
	        if (debug)
		    printk("wcfxs_ioctl, WCFXS_SET_REG\n");
		if (copy_from_user(&regop, (struct wcfxs_regop *)data, sizeof(regop)))
			return -EFAULT;
		if (regop.indirect) {
			if (wc->modtype[chan->chanpos - 1] != MOD_TYPE_FXS)
				return -EINVAL;
			if (debug)
			    printk("Setting indirect %d to 0x%04x on %d\n", regop.reg, regop.val, chan->chanpos);
			wcfxs_proslic_setreg_indirect(wc, chan->chanpos - 1, regop.reg, regop.val);
		} else {
			regop.val &= 0xff;
			if (debug)
			    printk("Setting direct %d to %04x on %d\n", regop.reg, regop.val, chan->chanpos);
			wcfxs_setreg(wc, chan->chanpos - 1, regop.reg, regop.val);
		}
		break;
	default:
		return -ENOTTY;
	}
	return 0;

}

int wcgsm_ioctl( struct inode *inodep, struct file *filep, unsigned int cmd, unsigned long arg)
{
	struct wcfxs *wc = devs;
	int res;
	
        switch(cmd) {
                case WCGSM_GET_CALLERID:
			printk("Inside WCGSM_GET_CALLERID, CID=%s\n", CallerID);		
                        if (copy_to_user((char *)arg, CallerID, strlen(CallerID)+1)) return -EFAULT;
                        break;

                case WCGSM_GET_SMS_COUNT:
                        return SMS.count;

                case WCGSM_GET_SMS:
			if(sms_transaction) {
				printk("wcgsm busy\n");
                                return -EFAULT;
			}

			if (!SMS.count) {
				printk("No new messages\n");
				return -EFAULT; 
			}

                        if (copy_to_user((char *)arg, SMS.sms[SMS.count-1], strlen(SMS.sms[SMS.count-1])+1)) {
				printk("Error copying the  message\n");
				return -EFAULT;
			}
                        kfree(SMS.sms[SMS.count-1]);
                        SMS.count--;
                        break;

                case WCGSM_SEND_SMS:
                        if(sms_transaction) {
                                printk("wcgsm busy\n");
                                return -EFAULT;
                        }

			if (gsm_port<0 || gsm_port>=FX_MAX_PORTS){ 
                                printk("No GSM1 detected\n");
                                return -EFAULT; 
                        }
			sms_transaction=1;
			res=sms_send(wc,gsm_port, (char *)arg);
			sms_transaction=0;
			if (res) {
				
				printk("Error sending the message\n"); 
                        	return -EFAULT;
			}
			
                        break;		

                default:
                        printk("invalid wcgsm_ioctl command 0x%X\n", cmd);
                        return -EFAULT;
        }

        return 0;
}


static int wcfxs_open(struct dahdi_chan *chan)
{
	struct wcfxs *wc = chan->pvt;
	if (!(wc->cardflag & (1 << (chan->chanpos - 1))))
		return -ENODEV;
	if (wc->dead)
		return -ENODEV;
	wc->usecount++;
#ifndef LINUX26
//	MOD_INC_USE_COUNT;
#else
	try_module_get(THIS_MODULE);
#endif	
	return 0;
}

static int wcfxs_watchdog(struct dahdi_span *span, int event)
{
	printk("TDM: Restarting DMA\n");
#ifdef NOT_NEEDED_YET
	wcfxs_restart_dma(span->pvt);
#endif
	return 0;
}

static int wcfxs_close(struct dahdi_chan *chan)
{
	struct wcfxs *wc = chan->pvt;
	wc->usecount--;
#ifndef LINUX26
//	MOD_DEC_USE_COUNT;
#else
	module_put(THIS_MODULE);
#endif
	if (wc->modtype[chan->chanpos - 1] == MOD_TYPE_FXS)
		wc->mod.fxs.idletxhookstate[chan->chanpos - 1] = 1;

	/* If we're dead, release us now */
	if (!wc->usecount && wc->dead) 
		wcfxs_release(wc);
	return 0;
}

static int wcfxs_hooksig(struct dahdi_chan *chan, enum dahdi_txsig  txsig)
{
	struct wcfxs *wc = chan->pvt;
	size_t len;// YN
	char GSM_Call_string[30]="atd";// YN

	int reg=0;
	if((wc->modtype[chan->chanpos - 1] == MOD_TYPE_FXO)&& (port_type[chan->chanpos - 1] != 'G')) {
		/* XXX Enable hooksig for FXO XXX */
		switch(txsig) {
		case DAHDI_TXSIG_START:
		case DAHDI_TXSIG_OFFHOOK:
			wc->mod.fxo.offhook[chan->chanpos - 1] = 1;
			wcfxs_setreg(wc, chan->chanpos - 1, 5, 0x9);
			if (debug)
			    printk("DAHDI_TXSIG_OFFHOOK FXO %d\n", chan->chanpos - 1);
			break;
		case DAHDI_TXSIG_ONHOOK:
			wc->mod.fxo.offhook[chan->chanpos - 1] = 0;
			wcfxs_setreg(wc, chan->chanpos - 1, 5, 0x8);
			if (debug)
			    printk("DAHDI_TXSIG_ONHOOK FXO %d\n", chan->chanpos - 1);
			break;
		default:
			printk("wcfxo: Can't set tx state to %d\n", txsig);
		}
	} 
	if (wc->modtype[chan->chanpos - 1] == MOD_TYPE_FXS) {//YN
		switch(txsig) {
		case DAHDI_TXSIG_ONHOOK:
			//incomming_call_state 0, initial state+first ring, 1, ringing, 2 conversastion 				
			wcfxs_set_led(wc, chan->chanpos, FX_LED_GREEN);
			if (debug)
			    printk("DAHDI_TXSIG_ONHOOK FXS %d\n", chan->chanpos - 1);
			switch(chan->sig) {
			case DAHDI_SIG_EM:
				if (debug)
			    		printk("DAHDI_SIG_EM FXS %d\n", chan->chanpos - 1);
			case DAHDI_SIG_FXOKS:
				if (debug)
			    		printk("DAHDI_SIG_FXOKS FXS %d\n", chan->chanpos - 1);
			case DAHDI_SIG_FXOLS:
				if (debug)
			    		printk("DAHDI_SIG_FXOLS FXS %d\n", chan->chanpos - 1);
			        //wc->mod.fxs.lasttxhook[chan->chanpos-1] = 1; /* power off audio paths */
				wc->mod.fxs.lasttxhook[chan->chanpos-1] = wc->mod.fxs.idletxhookstate[chan->chanpos-1];
				break;
			case DAHDI_SIG_FXOGS:
				if (debug)
			    		printk("DAHDI_SIG_FXOGS %d\n", chan->chanpos - 1);
				wc->mod.fxs.lasttxhook[chan->chanpos-1] = 3;
				break;
			}
			break;
		case DAHDI_TXSIG_OFFHOOK:
			wcfxs_set_led(wc, chan->chanpos, FX_LED_GREEN);
			if (debug)
			    printk("DAHDI_TXSIG_OFFHOOK FXS %d\n", chan->chanpos - 1);
			switch(chan->sig) {
			case DAHDI_SIG_EM:
				if (debug)
			    		printk("DAHDI_SIG_EM FXS %d\n", chan->chanpos - 1);

				wc->mod.fxs.lasttxhook[chan->chanpos-1] = 5;
				break;
			default:
				if (debug)
			    		printk("default detected %d\n", chan->chanpos - 1);

				wc->mod.fxs.lasttxhook[chan->chanpos-1] = wc->mod.fxs.idletxhookstate[chan->chanpos-1];
				break;
			}
			break;
		case DAHDI_TXSIG_START:
			wcfxs_set_led(wc, chan->chanpos, FX_LED_RED);
			if (debug)
			    printk("DAHDI_TXSIG_START FXS %d\n", chan->chanpos - 1);
			wc->mod.fxs.lasttxhook[chan->chanpos-1] = 4;
			break;
		case DAHDI_TXSIG_KEWL:
			if (debug)
			    printk("DAHDI_TXSIG_KEWL FXS %d\n", chan->chanpos - 1);
			wc->mod.fxs.lasttxhook[chan->chanpos-1] = 0;
			break;
		default:
			printk("wcfxs: Can't set tx state to %d\n", txsig);
		}

		if (debug)
			printk("Setting FXS hook state to %d (%02x)\n", txsig, reg);

#if 1
		wcfxs_setreg(wc, chan->chanpos - 1, 64, wc->mod.fxs.lasttxhook[chan->chanpos-1]);
#endif
	}
	if ((wc->modtype[chan->chanpos - 1] == MOD_TYPE_FXO) && (port_type[chan->chanpos - 1] == 'G')) {//YN
		/* XXX Enable hooksig for GSM */
		switch(txsig) {
		case DAHDI_TXSIG_START:
			if (debug)
			    printk("DAHDI_TXSIG_START GSM %d\n", chan->chanpos - 1);
			if(strlen(chan->txdialbuf)!=0){
				// The line below is necessary when we call and the other side is BUSY
				// Without this line -> blank signal is received
				chan->rxhooksig = DAHDI_RXSIG_OFFHOOK;// YN
				//Generate the GSM calling number string
				strcpy(GSM_Call_string,"atd");
				if (debug)
					printk("chan->txdialbuf => %s\n",chan->txdialbuf);
				strcat(GSM_Call_string,chan->txdialbuf);
				len=strlen( GSM_Call_string);
				GSM_Call_string[len-1]=0;//There is a "w" char which must be removed
				strcat(GSM_Call_string,";");
				if (debug)
					printk("GSM string => %s\n",GSM_Call_string);
				
				if((outgoing_call_state==0)&&(incomming_call_state==0)){
					GSM_send(wc,chan->chanpos - 1,GSM_Call_string);//"atd0899359529;");
					outgoing_call_state=1;
				}
				break;
			}
		case DAHDI_TXSIG_OFFHOOK:
			
			if(incomming_call_state==1){//During ringing DAHDI gives OFFHOOK
				printk("DAHDI_TXSIG_OFFHOOK GSM %d\n", chan->chanpos - 1);
				incomming_call_state=2;
				GSM_send(wc,chan->chanpos - 1,"ata");//Answer the call
			}
			if (debug)
			    printk("DAHDI_TXSIG_OFFHOOK GSM %d\n", chan->chanpos - 1);
			wc->mod.fxo.offhook[chan->chanpos - 1] = 1;
			break;
		case DAHDI_TXSIG_ONHOOK:
			if(outgoing_call_state==2) {
				GSM_send(wc,chan->chanpos - 1,"ath");//hangup the outgoing GSM call
				outgoing_call_state=0;
				*CallerID=0;
			}
                        if(incomming_call_state==2) {
                                GSM_send(wc,chan->chanpos - 1,"ath");//hangup the incamming GSM call
                                incomming_call_state=0;
				*CallerID=0;
                        }

			if (debug)
			    printk("DAHDI_TXSIG_ONHOOK GSM %d\n", chan->chanpos - 1);
			wc->mod.fxo.offhook[chan->chanpos - 1] = 0;
			break;
		default:
			printk("wcfxo: Can't set tx state to %d (GSM)\n", txsig);
		}
#if 1
		if (debug) {
		    char tmp;
		    tmp = wcfxs_getreg(wc, chan->chanpos - 1, 64);
		    printk("  reg 64 before: 0x%0x\n", tmp);
                }
		wcfxs_setreg(wc, chan->chanpos - 1, 64, wc->mod.fxs.lasttxhook[chan->chanpos-1]);
		if (debug)
		    printk("  FXS: %d  lasttxhook: %d\n", chan->chanpos - 1, wc->mod.fxs.lasttxhook[chan->chanpos-1]);
#endif
	}
	return 0;
}

static const struct dahdi_span_ops wcfxs_span_ops = {
                                                        .owner = THIS_MODULE,
                                                        .hooksig = wcfxs_hooksig,
                                                        .open = wcfxs_open,
                                                        .close = wcfxs_close,
                                                        .ioctl = wcfxs_ioctl,
                                                        .watchdog = wcfxs_watchdog};

static int wcfxs_initialize(struct wcfxs *wc)
{
	int x;

	/* Zapata stuff */
	sprintf(wc->span.name, "WCTDM/%d", wc->pos);
	sprintf(wc->span.desc, "%s Board %d", wc->variety, wc->pos + 1);
	wc->span.deflaw = DAHDI_LAW_MULAW;
	for (x=0;x<wc->cards;x++) {
		sprintf(wc->chans[x].name, "WCTDM/%d/%d", wc->pos, x);
		wc->chans[x].sigcap = DAHDI_SIG_FXOKS | DAHDI_SIG_FXOLS | DAHDI_SIG_FXOGS | DAHDI_SIG_SF | DAHDI_SIG_EM | DAHDI_SIG_CLEAR;
		wc->chans[x].sigcap |= DAHDI_SIG_FXSKS | DAHDI_SIG_FXSLS;
		wc->chans[x].chanpos = x+1;
		wc->chans[x].pvt = wc;
		wc->_chans[x]=&(wc->chans[x]);
	}
        wc->span.manufacturer   = "Rowetel";
        strlcpy(wc->span.devicetype, wc->variety, sizeof(wc->span.devicetype));
        wc->span.chans = wc->_chans;
        wc->span.channels = wc->cards;
        wc->span.flags = DAHDI_FLAG_RBS;

        wc->span.ops = &wcfxs_span_ops;
	if (dahdi_register(&wc->span, 0)) {
		printk("Unable to register span with DAHDI\n");
		return -1;
	}
	return 0;
}

static void wcfxs_post_initialize(struct wcfxs *wc)
{
	int x;
	/* Finalize signalling  */
	for (x=0;x<wc->cards;x++) {
		if (wc->cardflag & (1 << x)) {
			switch(wc->modtype[x])
			{
				case MOD_TYPE_FXO:
					wc->chans[x].sigcap = DAHDI_SIG_FXSKS | DAHDI_SIG_FXSLS | DAHDI_SIG_SF | DAHDI_SIG_CLEAR;
					break;
				case MOD_TYPE_FXS: 
					wc->chans[x].sigcap = DAHDI_SIG_FXOKS | DAHDI_SIG_FXOLS | DAHDI_SIG_FXOGS | DAHDI_SIG_SF | DAHDI_SIG_EM | DAHDI_SIG_CLEAR;			
					break;
				case MOD_TYPE_GSM://Added YN
					wc->chans[x].sigcap = DAHDI_SIG_FXSKS | DAHDI_SIG_FXSLS | DAHDI_SIG_SF | DAHDI_SIG_CLEAR;
					break;
			}
		}
	}
}

int wcfxs_proc_read(char *buf, char **start, off_t off, 
		    int count, int *eof, void *data)
{
	int i, len=0, real_count;
	struct wcfxs *wc = devs;
	
	real_count=count;
	count = PAGE_SIZE-1024;

	for(i=0;i<wc->cards;i++){
		if (wc->cardflag & (1 << i))
    		len += snprintf(buf + len, count - len, "Card[%d] -> reg64.....: 0x%x\n", i, wcfxs_getreg(wc, 0, 64));
	}

	count=real_count;
        // If everything printed so far is before beginning of request
        if (len <= off) {off = 0;len = 0;}
        *start = buf + off;
        len -= off;          		 // un-count any remaining offset 
        *eof = 1;
        if (len > count) len = count;    // don't return bytes not asked for 
        return len;
}

static int wcfxs_hardware_init(struct wcfxs *wc)
{
	/* Hardware stuff */
	unsigned char x,TDM_channel_offset;
        int           i;

#ifdef CONFIG_4FX_SPI_INTERFACE
	#ifdef MULTI_SPI_FRAMEWORK
		printk("Multi SPI \n");
		multi_spi_init();
	#else
		bfsi_spi_init(SPI_BAUDS, (1<<SPI_NCSA) | (1<<SPI_NCSB));
	#endif
#else
		sport_interface_init ( SPI_BAUDS, (1<<SPI_NCSA) | (1<<SPI_NCSB) );
#endif

	// select port 1 SPI device
	//bfsi_spi_write_8_bits(2, 2);
	proc_mkdir("wcfxs", 0);
    	create_proc_read_entry("wcfxs/wcfxs", 0, NULL, wcfxs_proc_read, NULL);

	printk("bfsi_sport_init res: %d\n",bfsi_sport_init(regular_interrupt_processing, DAHDI_CHUNKSIZE, debug));
#ifdef MULTI_SPI_FRAMEWORK
	multi_spi_reset();
#else
	bfsi_reset(RESET_BIT);
#endif

	#ifdef DAISY
	/* put 3210 in daisy chain mode */
	__write_8bits(wc, 0x00); /* reg 0 write */
	__write_8bits(wc, 0x80); /* value to write (set bit 7) */
	#endif
	printk("After DASIY\n");
	/* auto-detect each port type */

#ifdef MULTI_SPI_FRAMEWORK
	multi_spi_auto_detect(port_type);
        gsm_auto_detect(wc,port_type, RESET_BIT);//added YN

	for(i=0; i<multi_spi_board_size(); i++) {
	  printk("port: %d port_type: %c\n", i+1, port_type[i]);
	}
	
#else
	fx_auto_detect(port_type, RESET_BIT);
        gsm_auto_detect(wc,port_type, RESET_BIT);//added YN

	for(i=0; i<FX_MAX_PORTS; i++) {
	  printk("port: %d port_type: %c\n", i+1, port_type[i]);
	}
#endif
	

#ifdef MULTI_SPI_FRAMEWORK
	multi_spi_reset();
#else
	bfsi_reset(RESET_BIT);
#endif
	/* configure daughter cards */

	// YN Initialization of an array (resheduling TDM slots)
	memset(card2TDMslot_mapping, -1, sizeof(card2TDMslot_mapping));//Initially no slot is occupied!

	
	//YN
	//Since the TDM slot of the GSM module is not configurable we must allow
	//it to take the first and second TDM slots. For this purpose we must
	//check if a GSM module is available  and assign the first TDM slots to it.
	TDM_channel_offset=0;
	for (x=0;x<wc->cards;x++) {
		if (port_type[x] == 'G') {
			//Init GSM
			if (!(Initialize_GSM_module(wc,x))){

				wc->modtype[x]=MOD_TYPE_FXO;//WAS MOD_TYPE_GSM
				gsm_port=x;
	
				//Each GSM channel occupies two TDM slots (16-bit word!).
				//Since we will use companding only the first 8 bits are meaningful. 
				//The second 8 bits will however also be occupied!
				TDM_channel_offset=2;//The next available TDM slot. 
				card2TDMslot_mapping[x]=0;//Slot number occupied
				wc->cardflag |= (1 << x);//YN This port is registered with zaptel
				printk("Module %d - GSM : TDM slot %d\n",x,card2TDMslot_mapping[x]);//YN
				break;//Only one GSM Module is allowed!
			}
			else{
				printk("Module %d: FAILED GSM\n", x);
			}
		}//end of port_type[x]=='G'
	}//end for
	//bfin_write_FIO_FLAG_S(1<<14);//for test only

	for (x=0;x<wc->cards;x++) {
	  int sane=0,ret=0,readi=0;

	  if (port_type[x] == 'O') {
	    if (!(ret = wcfxs_init_voicedaa(wc, x,TDM_channel_offset, 0, 0, sane))) {// changed YN
	      wc->cardflag |= (1 << x);//YN
	      printk("Module %d: Installed -- AUTO FXO (%s mode), TDM slot %d\n",x, fxo_modes[_opermode].name,TDM_channel_offset);//YN
	      card2TDMslot_mapping[x]=TDM_channel_offset;
	    } //end if (!...
	    else
	      printk("Module %d: Not installed\n", x);
	   TDM_channel_offset++; 
 	   continue;
	  }//end port_type[x]=='O'
	  //else {
	  if (port_type[x] == 'S') {
		
	    sane=0;
	    /* Init with Automatic Calibaration */
	    if (!(ret = wcfxs_init_proslic(wc, x,TDM_channel_offset, 0, 0, sane))) {//changed YN
	      wc->cardflag |= (1 << x);//YN
	      card2TDMslot_mapping[x]=TDM_channel_offset;//added YN
	      if (debug) {
		readi = wcfxs_getreg(wc,x,LOOP_I_LIMIT);
		printk("Proslic module %d loop current is %dmA\n",x,
		       ((readi*3)+20));
	      }//end debug
	      printk("Module %d: Installed -- AUTO FXS, TDM slot %d\n",x,TDM_channel_offset);
	      TDM_channel_offset++;//YN
	    } //end if(!(ret=...
	    else 
	    {
	          if(ret != -2) 
		   {
			sane=1;
			/* Init with Manual Calibration */
			if (!wcfxs_init_proslic(wc, x,TDM_channel_offset, 0, 1, sane)) 
			{
				wc->cardflag |= (1 << x);//YN
				card2TDMslot_mapping[x]=TDM_channel_offset;//Added YN
                            if (debug) 
				{
                                   readi = wcfxs_getreg(wc,x,LOOP_I_LIMIT);
                                    printk("Proslic module %d loop current is %dmA\n",x,
                                    ((readi*3)+20));
                            }//end debug
				printk("Module %d: Installed -- MANUAL FXS, TDM slot %d\n",x,TDM_channel_offset);
				TDM_channel_offset++;//added YN
			} //end if(!wcfxs_init....
			else 
			{
				printk("Module %d: FAILED FXS (%s)\n", x, fxshonormode ? fxo_modes[_opermode].name : "FCC");
			} //end else
		  } //end (ret!=-2
	    } //end else
	  }//end if (port_type[x] == 'S') 
	//TDM_channel_offset++;
	}//end for (x=0;x<wc->cards;x++) 


	/* Return error if nothing initialized okay. */
	if (!wc->cardflag && !timingonly) {
	        printk("couldn't init OK...returning -1\n");
		bfsi_sport_close();
		return -1;
	}
	return 0;
}

static int wcfxs_init_one(struct wcfxs_desc *d)
{
	int res;
	struct wcfxs *wc;
	int x;
	int y;
	static int initd_ifaces=0;
	
	wcfxs_init_ok = 0;

	if(initd_ifaces){
		memset((void *)ifaces,0,(sizeof(struct wcfxs *))*WC_MAX_IFACES);
		initd_ifaces=1;
	}
	for (x=0;x<WC_MAX_IFACES;x++)
		if (!ifaces[x]) break;
	if (x >= WC_MAX_IFACES) {
		printk("Too many interfaces\n");
		return -EIO;
	}
	
	wc = kmalloc(sizeof(struct wcfxs), GFP_KERNEL);
	if (wc) {
		ifaces[x] = wc;
		memset(wc, 0, sizeof(struct wcfxs));
		spin_lock_init(&wc->lock);
		wc->curcard = -1;
		wc->cards = NUM_CARDS;
		wc->pos = x;
		wc->variety = d->name;
		wc->irq = IRQ_SPORT0_RX;
		devs = wc;
		for (y=0;y<NUM_CARDS;y++)
			wc->flags[y] = d->flags;

		if (wcfxs_initialize(wc)) {
			printk("wcfxs: Unable to intialize FXS\n");
			kfree(wc);
			return -EIO;
		}

		if (wcfxs_hardware_init(wc)) {
			dahdi_unregister(&wc->span);
			printk("wcfxs_hardware_init() failed...\n");
			kfree(wc);
			return -EIO;
		}

 		wcfxs_init_ok = 1;

		wcfxs_post_initialize(wc);
			
		printk("Found: %s (%d modules)\n", wc->variety, wc->cards);
		res = 0;
	} else
		res = -ENOMEM;
	
	return res;
}

static void wcfxs_release(struct wcfxs *wc)
{
 
	printk("wcfxs_init_ok = %d\n", wcfxs_init_ok);
	if (wcfxs_init_ok) {
	  /* disable serial port, this will stop DMA and interrupts */
	  bfsi_sport_close();
	  dahdi_unregister(&wc->span);
	  kfree(wc);

      #ifdef SF_IP01
      	fx_set_led(1, FX_LED_OFF);
      #endif
	}
        remove_proc_entry("wcfxs/wcfxs", NULL);
	remove_proc_entry("wcfxs", NULL);

	unregister_chrdev(wcgsm_major, "wcgsm");

	printk("Freed a Wildcard\n");
}

static int __init wcfxs_init(void)
{
	int x, res;
	wcgsm_state = WCGSM_NA;

	SMS.count=0;

	//Experimentaly add some short messages 
	/*
	{
		char *messgs[2]={"This is the first mesage tralala", "Vtora gabka tralala :)"};
                
		SMS.sms[0]=kmalloc(160, GFP_KERNEL);
		if(SMS.sms[0]){ 
			strcpy(SMS.sms[0], messgs[0]);	
			SMS.count++;
		}
		else
			printk(KERN_ALERT "Error allocating memory for sms[0]\n");
		

                SMS.sms[1]=kmalloc(160, GFP_KERNEL);
                if(SMS.sms[1]){
                        strcpy(SMS.sms[1], messgs[1]);
                        SMS.count++;
                }
                else
                        printk(KERN_ALERT "Error allocating memory for sms[1]\n");
	
	}
	*/
	
	if(debug)
	{
		printk(KERN_ALERT "Code test: code function addr = 0x%p\n", dahdi_ec_chunk);
	}

	for (x=0;x<(sizeof(fxo_modes) / sizeof(fxo_modes[0])); x++) {
		if (!strcmp(fxo_modes[x].name, opermode))
			break;
	}
	if (x < sizeof(fxo_modes) / sizeof(fxo_modes[0])) {
		_opermode = x;
	} else {
		printk("Invalid/unknown operating mode '%s' specified.  Please choose one of:\n", opermode);
		for (x=0;x<sizeof(fxo_modes) / sizeof(fxo_modes[0]); x++)
			printk("  %s\n", fxo_modes[x].name);
		printk("Note this option is CASE SENSITIVE!\n");
		return -ENODEV;
	}

	wcfxs_init_one(&wcfxs_bf);   

   	wait_just_a_bit(10);
        printk("wcfxs_init_ok = %d\n", wcfxs_init_ok);
	

	wcgsm_state = WCGSM_FREE;

        /* Registering device */
        res = register_chrdev(wcgsm_major, "wcgxs", &wcgsm_fops);
        if (res < 0) {
                printk("cannot obtain wcgsm major number %d\n", wcgsm_major);
                return res;
        }
	return 0;
}

static void __exit wcfxs_cleanup(void)
{

	wcfxs_release(devs);

	#ifdef OLD_DR
	if (loopback) {
		for(r=0; r<DAHDI_CHUNKSIZE*2; r++) {
			printk("[%03d] ", r*8);
			for(c=0; c<8; c++) {
				printk("0x%02x 0x%02x  ", 
				       iTxBuffer1[r*8+c]&0xff, 
				       iRxBuffer1[r*8+c]&0xff);
			}
			printk("\n");
		}
		//for(r=0; r<10; r++) {
		//	printk("[%03d] 0x%04x 0x%04x %d\n", r, logdma1[r], logdma2[r], logdma3[r]);
		//}
		for(r=0; r<LOG_LEN; r++) {
			printk("[%03d] %d\n", r, logdma1[r]);
		}
		printk("serialnum = %d ilogdma = %d\n",serialnum, ilogdma);
	}
	#endif
}

//Char device open/release ---------------------
int wcgsm_open(struct inode *inode, struct file *filp)
{

        // Check first if device is not busy.
        if (wcgsm_state == WCGSM_BUSY) {
                printk("Wcfxs char device is busy !!\n");
		return -EFAULT;
        }
        if (wcgsm_state == WCGSM_NA) {
                printk("Wcfxs char device is not available !!\n");
                return -EFAULT;
        }
        wcgsm_state = WCGSM_BUSY;
        
	/* Success */
        return 0;
}

int wcgsm_release(struct inode *inode, struct file *filp)
{
        wcgsm_state = WCGSM_FREE;
        
	/* Success */
        return 0;
}

module_param(SIM_pin, charp, 0600);// New parameter YN
module_param(debug, int, 0600);
module_param(loopcurrent, int, 0600);
module_param(robust, int, 0600);
module_param(_opermode, int, 0600);
module_param(opermode, charp, 0600);
module_param(timingonly, int, 0600);
module_param(lowpower, int, 0600);
module_param(boostringer, int, 0600);
module_param(fxshonormode, int, 0600);
module_param(loopback, int, 0600); /* uCasterisk test mode */
//module_param(internalclock, int, 0600); /* uCasterisk test mode */

MODULE_DESCRIPTION("Wildcard TDM400P Zaptel Driver");
MODULE_AUTHOR("Mark Spencer <markster@digium.com>");
#ifdef MODULE_LICENSE
MODULE_LICENSE("GPL");
#endif

module_init(wcfxs_init);
module_exit(wcfxs_cleanup);

