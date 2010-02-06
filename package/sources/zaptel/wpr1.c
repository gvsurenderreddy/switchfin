/*
 * BF1 Team  - BF1 / PR1 E1/T1 extension card Driver
 *
 *
 * Based on :
 *
 * Digium, Inc.  Wildcard TE110P T1/PRI card Driver
 * Written by Mark Spencer <markster@digium.com>
 *            Matthew Fredrickson <creslin@digium.com>
 *            William Meadows <wmeadows@digium.com>
 * Copyright (C) 2004, Digium, Inc.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. 
 *
 */
 /*****************************************************************************
 * History :
 *
 * 26/02/07 : MP - Initial file 
 * 17/04/07 : Mathieu Padovani - updated regs values for initial hardware tests
 * 08/12/07 : Mark @ Astfin . org- adapted for pr1-appliance
 * 16/12/07 : Pawel Pastuszak - added sport0 support for Blackfin BF537
 * 04/14/08 : Mark @ Astfin . org - Applied David Rowe's DMA buffer selection bugfix
 * 04/14/08 : Mark @ Astfin . org - Overall code cleanup
 * 10/04/08 : Mark @ Astfin . org - Added support for Zarlink echo can (double rate clock)
 * 10/05/08 : DPN @ SwitchVoice . com   - Added support for clock master mode
 * 10/13/08 : DPN @ SwitchVoice . com   - Added support for different terminations we need for T1/E1
 * 				    T1: 100 Ohm; E1: 120 Ohm 	
 * 12/16/08 : Mark @ Astfin . org - Added support for Legacy 2.1 version
 *
 * Copyright @ 2010 SwitchFin <dpn@switchvoice.com>
 *
 ******************************************************************************/

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/spinlock.h>

#include <asm/blackfin.h>
#include <asm/cacheflush.h>
#include <asm/irq.h>
#include <asm/dma.h>
#include <linux/dma-mapping.h>
#include <linux/proc_fs.h>

#include "zaptel.h"

#ifdef LINUX26
#include <linux/moduleparam.h>
#endif

#include "wpr1.h"
#include "zl_wrap.h"

//Use line below for FALC 56 ver. 2.2
#define FALC56_2_2

//Use line below for FALC 56 ver. 2.1
//#undef FALC56_2_2


//#define WPR_8MBDATA /* 8Mhz clock on system highway */
#undef WPR_8MBDATA /* 2.048Mhz clock on system highway */

/*
 int chanmap_t1[] =
{ 0,1,2,4,5,6,8,9,10,
  12,13,14,16,17,18,20,21,22,
  24,25,26,28,29,30,31};
*/
// Chanmap = 1 below

int chanmap_t1[] =
{ 0,1,2,3,4,5,6,7,8,9,10,
  11,12,13,14,15,16,17,18,
  19,20,21,22,23};


static int chanmap_e1[] =
{ 0,1,2,3,4,5,6,7,8,9,10,11,
  12,13,14,15,16,17,18,19,20,
  21,22,23,24,25,26,27,28,29,30,31};

#ifdef FANCY_ALARM
static int altab[] = {
0, 0, 0, 1, 2, 3, 4, 6, 8, 9, 11, 13, 16, 18, 20, 22, 24, 25, 27, 28, 29, 30, 31, 31, 32, 31, 31, 30, 29, 28, 27, 25, 23, 22, 20, 18, 16, 13, 11, 9, 8, 6, 4, 3, 2, 1, 0, 0, 
};
#endif

/* Module Parameters */

static int debug = 0;   /* default to debug at this step ... */
static int j1mode = 0;
static int alarmdebounce = 0;
static int loopback = 0;

#ifdef CONFIG_PR1_CLOCK_10
static int clockfreq = 0;
#endif
#ifdef CONFIG_PR1_CLOCK_8
static int clockfreq = 1;
#endif
#ifdef CONFIG_PR1_CLOCK_2
static int clockfreq = 2;
#endif

static int t1e1mode = 2; /*DPN: Select E1 by default*/	

#define PING 0
#define PONG 1


static int fakemode = 0; /* default to fakemode as hardware not available .... */

/* Module Globals */

/* debug variables */

static int readchunk_total = 0;
static int readchunk_first = 0;
static int readchunk_second = 0;
static int readchunk_didntswap = 0;
static int readchunk_lastdidntswap = 0;
static int lastreadpingpong;
static int readchunk_x = 0;

static int bad_x[5];
static int last_x[5];
static u8 *log_readchunk;

static int writechunk_total = 0;
static int writechunk_first = 0;
static int writechunk_second = 0;
static int writechunk_didntswap = 0;
static int lastwritepingpong;
static int writechunk_lastdidntswap = 0;

/* previous and worst case number of cycles we took to process an interrupt */
static u32 isr_cycles_last = 0;
static u32 isr_cycles_worst = 0;
static u32 isr_cycles_average = 0; /* scaled up by 2x */

/* constants for isr cycle averaging */

#define TC    1024 /* time constant    */
#define LTC   10   /* base 2 log of TC */

/* Driver specific Globals */

static struct t1 *pCard;
static void (*pr1_isr_callback)(u8 *read_samples, u8 *write_samples) = NULL;

static u8 *iTxBuffer1;
static u8 *iRxBuffer1;

#define FAKEBUFLEN 	256
static u8 *tFakeBuffer;

#define WPR_NBCHAN 32 
/* Proc FS specific */
#define PROCFS_MAX_SIZE		1024
#define PROCFS_NAME 		"wpr1"

static struct proc_dir_entry *Our_Proc_File;
static char procfs_buffer[PROCFS_MAX_SIZE];
static unsigned long procfs_buffer_size = 0;

int wpr1_proc_read(char *buf, char **start, off_t offset, int count, int *eof, void *data);
int wpr1_proc_write(struct file *file, const char *buffer, unsigned long count, void *data);

/* sample cycles register of Blackfin */

static inline unsigned int cycles(void) {
  int ret;

   __asm__ __volatile__
   (
   "%0 = CYCLES;\n\t"
   : "=&d" (ret)
   :
   : "R1"
   );

   return ret;
}

int wpr1_proc_read(char *buf, char **start, off_t offset,
                    int count, int *eof, void *data)
{
        int len,i,j;
	char sBuf[2048];
	char str[10];

	*sBuf=0;
	strcat(sBuf,"Tx:");
	for (j=0;j<1;j++)
	{
	for (i=0;i<32;i++)
	{ sprintf(str,"%02x ",*(iTxBuffer1+(j*32)+i));
	  strcat(sBuf,str);
	}
	  strcat(sBuf,"\n");
	}
	  strcat(sBuf,"\n");

	strcat(sBuf,"Rx:");
	for (j=0;j<1;j++)
	{
	for (i=0;i<32;i++)
	{ sprintf(str,"%02x ",*(iRxBuffer1+(j*32)+i));
	  strcat(sBuf,str);
	}
	  strcat(sBuf,"\n");
	}

        len = sprintf(buf,
                      "readchunk_first.....: %d\n"
                      "readchunk_second....: %d\n"
                      "readchunk_didntswap.: %d\n"
		      "bad_x...................: %d %d %d %d %d\n"
                      "log_readchunk...........: %p\n"
                      "read  lastdidntswap.: %d\n"
                      "writechunk_first....: %d\n"
                      "writechunk_second...: %d\n"
                      "writechunk_didntswap: %d\n"
                      "write  lastdidntswap: %d\n"
                      "isr_cycles_last.....: %d\n"
                      "isr_cycles_worst....: %d\n"
                      "isr_cycles_average..: %d\n"
		      "readchunk_x : %d \n"
		      "frame \n%s \n",
                      readchunk_first,
                      readchunk_second,
                      readchunk_didntswap,
		      bad_x[0],bad_x[1],bad_x[2],bad_x[3],bad_x[4],
                      log_readchunk,
                      readchunk_lastdidntswap,
                      writechunk_first,
                      writechunk_second,
                      writechunk_didntswap,
                      writechunk_lastdidntswap,
                      isr_cycles_last,
                      isr_cycles_worst,
                      isr_cycles_average>>1,
		      readchunk_x,
		      sBuf
                      );

        *eof=1;
        return len;
}

int wpr1_proc_write(struct file *file, const char *buffer, unsigned long count, void *data)
{
int reg=0,val=0;
struct t1 *wc = pCard;

	/* get buffer size */
	procfs_buffer_size = count;
	if (procfs_buffer_size > PROCFS_MAX_SIZE ) {
		procfs_buffer_size = PROCFS_MAX_SIZE;
	}
	
	/* write data to the buffer */
	if ( copy_from_user(procfs_buffer, buffer, procfs_buffer_size) ) {
		return -EFAULT;
	}
	*(procfs_buffer+count)=0;
	if(*procfs_buffer=='?')
	{
	if(sscanf(procfs_buffer+1,"%x",&reg)==1)
	{
		val= t1_framer_in8(wc, reg);
		printk("Reading reg : %02x val : %02x \n",reg,val);
	}
	}
	else
	if(*procfs_buffer=='*')
	{
	for (reg=0;reg < 172;reg++)
	{
		val= t1_framer_in8(wc, reg);
		printk("Reading reg : %02x val : %02x \n",reg,val);
	}
	}
	else
	if(sscanf(procfs_buffer,"%x %x",&reg,&val)==2)
	{
		printk("Writing reg : %02x val : %02x \n",reg,val);
		t1_framer_out8(wc, reg, val);
	}
	
	return procfs_buffer_size;
}

static inline void start_alarm(struct t1 *wc)
{
if (debug) printk("wpr1 : start_alarm\n");
#ifdef FANCY_ALARM
	wc->alarmpos = 0;
#endif
	wc->blinktimer = 0;
}

static inline void stop_alarm(struct t1 *wc)
{
if (debug) printk("wpr1 : stop_alarm\n");
#ifdef FANCY_ALARM
	wc->alarmpos = 0;
#endif
	wc->blinktimer = 0;
}

static int pr1_open(struct zt_chan *chan)
{
	struct t1 *wc = chan->pvt;

	if (debug) printk("wpr1 : pr1_open \n");

	if (wc->dead)
		return -ENODEV;
	wc->usecount++;
#ifndef LINUX26	
	MOD_INC_USE_COUNT;
#else
	try_module_get(THIS_MODULE);
#endif	
	return 0;
}

static int pr1_close(struct zt_chan *chan)
{
	struct t1 *wc = chan->pvt;

	if (debug)
		printk("wpr1 : pr1_close \n");

	wc->usecount--;
#ifndef LINUX26	
	MOD_DEC_USE_COUNT;
#else
	module_put(THIS_MODULE);
#endif
	/* If we're dead, release us now */
	if (!wc->usecount && wc->dead) 
		pr1_release(wc);
	return 0;
}


static inline u16 __t1_framer_in16(struct t1 *wc, const unsigned int reg)
{
 volatile u16 *pVal;

	pVal= (u16 *)(wc->ioaddr + (reg- (reg&1))); 

	/* Get 16b value  From PEF Register */
	return *pVal;
}

static inline u8 __t1_framer_in8(struct t1 *wc, const unsigned int reg)
{
 volatile u16 *pVal;
 u16 uVal=0;
	
	/* Get 16b value  From PEF Register */
	pVal= (u16 *)(wc->ioaddr + (reg- (reg&1))); 

	uVal=*pVal;

	return (!(reg&1))?(u8)(uVal&0xFF):(u8)(uVal >>8);
}

static inline unsigned int t1_framer_in8(struct t1 *wc, const unsigned int addr)
{
unsigned long flags;
unsigned int ret;

	spin_lock_irqsave(&wc->lock, flags);
	ret = __t1_framer_in8(wc, addr);
	spin_unlock_irqrestore(&wc->lock, flags);

	/*if (debug)
		printk("ioadd  %#x Reading from register %#x  value  %#x\n", wc->ioaddr,addr,ret); */

	return ret;
}

static inline void __t1_framer_out8(struct t1 *wc, const unsigned int reg, const unsigned int val)
{
volatile u16 *paddr;
u16 pval=0,oth_val=0;

	oth_val= __t1_framer_in8(wc,(reg&1?reg-1:reg+1));

	/* Write value to PEF Register */
 	paddr= (u16 *)(wc->ioaddr+(reg-(reg&1))) ;
	pval = (!(reg&1))?(u16)((oth_val << 8)|(val&0xFF)):(u16)((val << 8)|(oth_val&0xFF) );

	/* if (debug)
		printk("ioadd  %#x Writing %#x to reg %#x\n", wc->ioaddr,val,reg); */

 	*paddr = pval;
}

static inline void t1_framer_out8(struct t1 *wc, const unsigned int addr, const unsigned int value)
{
	unsigned long flags;
	spin_lock_irqsave(&wc->lock, flags);
	__t1_framer_out8(wc, addr, value);
	spin_unlock_irqrestore(&wc->lock, flags);

}

static void pr1_release(struct t1 *wc)
{
	if (debug) printk("wpr1 : pr1_release \n");

	zt_unregister(&wc->span);
	kfree(wc);
	if(fakemode) kfree(tFakeBuffer);
	printk("Freed a Wildcard\n");
}


static  int pr1_check_framer (struct t1 *wc)
{ u8 falcver;

	if (debug) printk("wpr1 : pr1_check_framer \n");

	/* check version */
	t1_framer_out8(wc, FALC56_VSTR, 0xaa);/* PEF VSTR */
	falcver = t1_framer_in8(wc ,0x4a);
	printk("FALC version: %08x\n", falcver);

	if (!falcver)
	{
		printk("wpr1 : Falc version 1.2 not supported \n");
		return 0;
	}
return 0;
}

static void pr1_dma_sport_start(void)
{
	if (debug) printk("wpr1 : Start DMA\n");

	bfin_write_SPORT0_MCMC2(bfin_read_SPORT0_MCMC2() | MCMEN);

#if (defined(CONFIG_BF532) || defined(CONFIG_BF533))

	/*bfin_write_SPORT0_MCMC2(bfin_read_SPORT0_MCMC2() | MCMEN);*/

	bfin_write_DMA2_CONFIG(bfin_read_DMA2_CONFIG() | DMAEN);
	bfin_write_DMA1_CONFIG(bfin_read_DMA1_CONFIG() | DMAEN);

#endif

#if (defined(CONFIG_BF536) || defined(CONFIG_BF537))

	bfin_write_DMA4_CONFIG(bfin_read_DMA4_CONFIG() | DMAEN);
        bfin_write_DMA3_CONFIG(bfin_read_DMA3_CONFIG() | DMAEN);
        __builtin_bfin_ssync();
        if(debug)
                printk(KERN_INFO "DMA3/DMA4 enabled\n");

#endif
	if (debug) printk("wpr1 : Enable SPORT Rx Tx\n");
	bfin_write_SPORT0_TCR1(bfin_read_SPORT0_TCR1() | TSPEN);
	bfin_write_SPORT0_RCR1(bfin_read_SPORT0_RCR1() | RSPEN);

	__builtin_bfin_ssync();
}

static void pr1_dma_sport_stop(void)
{
	if (debug) printk("wpr1 : Stop DMA\n");
	bfin_write_SPORT0_MCMC2(bfin_read_SPORT0_MCMC2() & ~MCMEN);

#if (defined(CONFIG_BF532) || defined(CONFIG_BF533))

	/*bfin_write_SPORT0_MCMC2(bfin_read_SPORT0_MCMC2() & ~MCMEN);*/

	bfin_write_DMA2_CONFIG(bfin_read_DMA2_CONFIG() & ~DMAEN);
	bfin_write_DMA1_CONFIG(bfin_read_DMA1_CONFIG() & ~DMAEN);

#endif

#if (defined(CONFIG_BF536) || defined(CONFIG_BF537))

        /* disable DMA3 and DMA4 */
        bfin_write_DMA4_CONFIG(bfin_read_DMA4_CONFIG() & (~DMAEN));
        bfin_write_DMA3_CONFIG(bfin_read_DMA3_CONFIG() & (~DMAEN));

	__builtin_bfin_ssync();
#endif
	if (debug) printk("wpr1 : disable SPORT Rx Tx\n");
	bfin_write_SPORT0_TCR1(bfin_read_SPORT0_TCR1() & ~TSPEN);
	bfin_write_SPORT0_RCR1(bfin_read_SPORT0_RCR1() & ~RSPEN);

	__builtin_bfin_ssync();
}

static int pr1_enable_interrupts(struct t1 *wc)
{
	if (debug) printk("wpr1 : Enable RX interrupt\n");
	
	if(request_irq(IRQ_SPORT0_RX, sport_rx_isr,
                       IRQF_DISABLED, "sport rx", NULL) != 0) {
        if (debug) printk("wpr1 : Enable RX interrupt : Failure \n");
                return -EBUSY;
        }
	/* enable DMA1 for BF533 or BF532 */
        /* enable DMA3 for BF537 sport0 Rx interrupt */
        bfin_write_SIC_IMASK(bfin_read_SIC_IMASK() | SCI_IMASK_SPORT0_RX);

	__builtin_bfin_ssync();

	return 0;
}

static void pr1_disable_interrupts()	
{
	if (debug) printk("wpr1 : Disable RxInterrupt\n");

	bfin_write_SIC_IMASK(bfin_read_SIC_IMASK() | ~SCI_IMASK_SPORT0_RX);
	__builtin_bfin_ssync();
}


static u8 *isr_write_processing(void) {
	u8 *writechunk;
	int writepingpong;
	int x;

	writechunk_total++;

	__builtin_bfin_ssync();
	/* select which ping-pong buffer to write to */
#if (defined(CONFIG_BF532) || defined(CONFIG_BF533))
	x = (int)(bfin_read_DMA2_CURR_ADDR()) - (int)iTxBuffer1;
#endif  

#if (defined(CONFIG_BF536) || defined(CONFIG_BF537))

	x = (int)(bfin_read_DMA4_CURR_ADDR()) - (int)iTxBuffer1;
#endif


	/* for some reason x for tx tends to be 0xe and 0x4e, whereas
	   x for rx is 0x40 and 0x80.  Note sure why they would be
	   different.  We could perhaps consider having
	   different interrupts for tx and rx side.  Hope this
	   offset doesn't kill the echo cancellation, e.g. if we
	   get echo samples in rx before tx has sent them!
	*/
	if (x >= (WPR_NBCHAN*ZT_CHUNKSIZE)) {
		writechunk = iTxBuffer1;
		writechunk_first++;
		writepingpong = PING;
	}
	else {
		writechunk = iTxBuffer1 + ZT_CHUNKSIZE*WPR_NBCHAN;
		writechunk_second++;
		writepingpong = PONG;
	}

	/* make sure writechunk actually ping pongs */

	if (writepingpong == lastwritepingpong) {
		writechunk_didntswap++;
		writechunk_lastdidntswap=x;
	}
	lastwritepingpong = writepingpong;

	return writechunk;
}


static u8 *isr_read_processing(void) {
	u8 *readchunk;
	int readpingpong;
	int x, i;

	readchunk_total++;
	/* select which ping-pong buffer to write to */

	__builtin_bfin_ssync();
#if (defined(CONFIG_BF532) || defined(CONFIG_BF533))
	x = (int)bfin_read_DMA1_CURR_ADDR() - (int)iRxBuffer1;
#endif

#if (defined(CONFIG_BF536) || defined(CONFIG_BF537))

	x = (int)bfin_read_DMA3_CURR_ADDR() - (int)iRxBuffer1;
#endif

	readchunk_x=x;

	/* possible values for x are 8*ZT_CHUNKSIZE=0x100 at the
	   end of the first row and 2*8*ZT_CHUNKSIZE=0x200 at the
	   end of the second row */
	if ((x >= (WPR_NBCHAN*ZT_CHUNKSIZE))&&(x<(2*(WPR_NBCHAN*ZT_CHUNKSIZE)))) {
		readchunk = iRxBuffer1;
		readchunk_first++;
		readpingpong = PING;
	}
	else {
		readchunk = iRxBuffer1 + ZT_CHUNKSIZE*WPR_NBCHAN;
		readchunk_second++;
		readpingpong = PONG;
	}

	log_readchunk = readchunk;

	/* memory of x for debug */

        for(i = 0; i < 4; i++)
            last_x[i] = last_x[i+1];

        last_x[4] = x;


	/* make sure readchunk actually ping pongs */

	if (readpingpong == lastreadpingpong) {
		readchunk_didntswap++;
		memcpy(bad_x, last_x, sizeof(bad_x));
	}
	lastreadpingpong = readpingpong;

	return readchunk;
}

static irqreturn_t sport_rx_isr(int irq, void *dev_id, struct pt_regs * regs)
{
  unsigned int  start_cycles = cycles();
  u8           *read_samples;
  u8           *write_samples;


  /* confirm interrupt handling, write 1 to DMA_DONE bit */
#if (defined(CONFIG_BF532) || defined(CONFIG_BF533))
  bfin_write_DMA1_IRQ_STATUS(0x0001); 
#endif

#if (defined(CONFIG_BF536) || defined(CONFIG_BF537))
  bfin_write_DMA3_IRQ_STATUS(DMA_DONE); 
#endif

  
  __builtin_bfin_ssync(); 

  read_samples = isr_read_processing();
  write_samples = isr_write_processing();
  if (pr1_isr_callback != NULL) {
    pr1_isr_callback(read_samples, write_samples);
  }

  __builtin_bfin_ssync();

  /* some stats to help monitor the cycles used by ISR processing */

  /* 
     Simple IIR averager: 

       y(n) = (1 - 1/TC)*y(n) + (1/TC)*x(n)

     After conversion to fixed point:

       2*y(n) = ((TC-1)*2*y(n) + 2*x(n) + half_lsb ) >> LTC 
  */

  isr_cycles_average = ( (u32)(TC-1)*isr_cycles_average + 
			 (((u32)isr_cycles_last)<<1) + TC) >> LTC;

  if (isr_cycles_last > isr_cycles_worst)
    isr_cycles_worst = isr_cycles_last;

  /* we sample right at the end to make sure we count cycles used to 
     measure cycles! */
  isr_cycles_last = cycles() - start_cycles;
  
  return IRQ_HANDLED;
}

static void pr1_set_led(struct t1 *wc)	
{
u8 uVal,uReg;

	uVal= wc->ledtestreg;

	/* Led control on RPB / RPC of PEF 2256 */
	uReg=__t1_framer_in8(wc,FALC56_PC2);

	if(uVal&BIT_LED0)
		uReg&=~(1<<4);
	else
		uReg|=(1<<4);

	__t1_framer_out8(wc, FALC56_PC2, uReg); /* PEF - RPB */

	uReg=__t1_framer_in8(wc,FALC56_PC3);

	if(uVal&BIT_LED1)
		uReg&=~(1<<4);
	else
		uReg|=(1<<4);

	__t1_framer_out8(wc, FALC56_PC3, uReg); /* PEF - RPC */

	return;
}

static void __t1_set_clear(struct t1 *wc)
{
	int i,j;
	unsigned short val=0;
	for (i=0;i<24;i++) {
		j = (i/8);
		if (wc->span.chans[i].flags & ZT_FLAG_CLEAR) 
			val |= 1 << (7 - (i % 8));
		if ((i % 8)==7) {
			if (debug)
				printk("wpr1: __t1_set_clear - Putting %d in register %02x\n", val, 0x2f + j);
			__t1_framer_out8(wc, 0x2f + j, val); /* PEF CCB1->CCB3 */
			val = 0;
		}
	}
	return;
}

static int pr1_ioctl(struct zt_chan *chan, unsigned int cmd, unsigned long data)
{
	int val;
	switch(cmd){
               case ZT_TONEDETECT:
                        val=1;         //We don't do DTMF detection in hardware so chan_zap should do it
                        break;

               default:  val=0;
       }
	if (debug)
		printk("wpr1_ioctl : cmd=%d.\n", cmd);

       return val;
}

static int __t1_maint(struct zt_span *span, int cmd)
{
	struct t1 *wc = span->pvt;

	if (wc->spantype == E1) {
		switch(cmd) {
		case ZT_MAINT_NONE:
			printk("wpr1: E1 - Turn off local and remote loops E1 XXX\n");
			break;
		case ZT_MAINT_LOCALLOOP:
			printk("wpr1: E1 - Turn on local loopback E1 XXX\n");
			break;
		case ZT_MAINT_REMOTELOOP:
			printk("wpr1: E1 - Turn on remote loopback E1 XXX\n");
			break;
		case ZT_MAINT_LOOPUP:
			printk("wpr1: E1 - Send loopup code E1 XXX\n");
			break;
		case ZT_MAINT_LOOPDOWN:
			printk("wpr1: E1 - Send loopdown code E1 XXX\n");
			break;
		case ZT_MAINT_LOOPSTOP:
			printk("wpr1: E1 - Stop sending loop codes E1 XXX\n");
			break;
		default:
			printk("wpr1: Unknown E1 maint command: %d\n", cmd);
			break;
		}
	} else {
		switch(cmd) {
	    case ZT_MAINT_NONE:
			printk("wpr1: T1 -  Turn off local and remote loops T1 XXX\n");
			break;
	    case ZT_MAINT_LOCALLOOP:
			printk("wpr1: T1 - Turn on local loop and no remote loop XXX\n");
			break;
	    case ZT_MAINT_REMOTELOOP:
			printk("wpr1: T1 - Turn on remote loopup XXX\n");
			break;
	    case ZT_MAINT_LOOPUP:
			t1_framer_out8(wc, FALC56_XSP_FMR5, 0x50);	/* FMR5: Nothing but RBS mode */
			break;
	    case ZT_MAINT_LOOPDOWN:
			t1_framer_out8(wc, FALC56_XSP_FMR5, 0x60);	/* FMR5: Nothing but RBS mode */
			break;
	    case ZT_MAINT_LOOPSTOP:
			t1_framer_out8(wc, FALC56_XSP_FMR5, 0x40);	/* FMR5: Nothing but RBS mode */
			break;
	    default:
			printk("wpr1: Unknown T1 maint command: %d\n", cmd);
			break;
	   }
    }
	return 0;
}

static int t1_rbsbits(struct zt_chan *chan, int bits)
{
int iRet=0;
struct t1 *wc = chan->pvt;
unsigned long flags;

        spin_lock_irqsave(&wc->lock, flags);
	iRet=  __t1_rbsbits(chan, bits);
        spin_unlock_irqrestore(&wc->lock, flags);

	return iRet;

}

static int __t1_rbsbits(struct zt_chan *chan, int bits)
{
        u_char m,c;
        int n,b;
        struct t1 *wc = chan->pvt;

        if(debug) printk("wpr1: Setting bits to %d on channel %s\n", bits, chan->name);
        if (wc->spantype == E1) { /* do it E1 way */
                if (chan->chanpos == 16) {
                        return 0;
                }
                n = chan->chanpos - 1;
                if (chan->chanpos > 15) n--;
                b = (n % 15);
                c = wc->txsigs[b];
                m = (n / 15) << 2; /* nibble selector */
                c &= (0xf << m); /* keep the other nibble */
                c |= (bits & 0xf) << (4 - m); /* put our new nibble here */
                wc->txsigs[b] = c;
                  /* output them to the chip */
                __t1_framer_out8(wc,0x71 + b,c); /* PEF XS1->XS16 Transmit CAS Register */
        } else if (wc->span.lineconfig & ZT_CONFIG_D4) {
                n = chan->chanpos - 1;
                b = (n/4);
                c = wc->txsigs[b];
                m = ((3 - (n % 4)) << 1); /* nibble selector */
                c &= ~(0x3 << m); /* keep the other nibble */
                c |= ((bits >> 2) & 0x3) << m; /* put our new nibble here */
                wc->txsigs[b] = c;
                  /* output them to the chip */
                __t1_framer_out8(wc,0x70 + b,c); /* RS1 + b Receive Signaling Register */
                __t1_framer_out8(wc,0x70 + b + 6,c);/* RS1 + b +6 */
        } else if (wc->span.lineconfig & ZT_CONFIG_ESF) {
                n = chan->chanpos - 1;
                b = (n/2);
                c = wc->txsigs[b];
                m = ((n % 2) << 2); /* nibble selector */
                c &= (0xf << m); /* keep the other nibble */
                c |= (bits & 0xf) << (4 - m); /* put our new nibble here */
                wc->txsigs[b] = c;

                  /* output them to the chip */
                __t1_framer_out8(wc,0x70 + b,c); /* RS1 + b */
        }
        if (debug > 1)
                printk("wpr1 : Finished setting RBS bits\n");
        return 0;
}

static void __t1_check_sigbits(struct t1 *wc)
{
	int a,i,rxs;

        //if (debug) printk("wpr1 : __t1_check_sigbits\n");

	if (!(wc->span.flags & ZT_FLAG_RUNNING))
		return;
	if (wc->spantype == E1) { /* This  case for E1 with  Robbed bits signaling */
		for (i = 0; i < 15; i++) {
			a = __t1_framer_in8(wc, 0x71 + i); /* XS1 + i Tx CAS Register */
			/* Get high channel in low bits */
			rxs = (a & 0xf);
			if (!(wc->span.chans[i+16].sig & ZT_SIG_CLEAR)) {
				if (wc->span.chans[i+16].rxsig != rxs)
					zt_rbsbits(&wc->span.chans[i+16], rxs);
			}
			rxs = (a >> 4) & 0xf;
			if (!(wc->span.chans[i].sig & ZT_SIG_CLEAR)) {
				if (wc->span.chans[i].rxsig != rxs)
					zt_rbsbits(&wc->span.chans[i], rxs);
			}
		}
	} else if (wc->span.lineconfig & ZT_CONFIG_D4) {
		for (i = 0; i < 24; i+=4) {
			a = __t1_framer_in8(wc, 0x70 + (i>>2)); /* RS1 + x */
			/* Get high channel in low bits */
			rxs = (a & 0x3) << 2;
			if (!(wc->span.chans[i+3].sig & ZT_SIG_CLEAR)) {
				if (wc->span.chans[i+3].rxsig != rxs)
					zt_rbsbits(&wc->span.chans[i+3], rxs);
			}
			rxs = (a & 0xc);
			if (!(wc->span.chans[i+2].sig & ZT_SIG_CLEAR)) {
				if (wc->span.chans[i+2].rxsig != rxs)
					zt_rbsbits(&wc->span.chans[i+2], rxs);
			}
			rxs = (a >> 2) & 0xc;
			if (!(wc->span.chans[i+1].sig & ZT_SIG_CLEAR)) {
				if (wc->span.chans[i+1].rxsig != rxs)
					zt_rbsbits(&wc->span.chans[i+1], rxs);
			}
			rxs = (a >> 4) & 0xc;
			if (!(wc->span.chans[i].sig & ZT_SIG_CLEAR)) {
				if (wc->span.chans[i].rxsig != rxs)
					zt_rbsbits(&wc->span.chans[i], rxs);
			}
		}
	} else {
		for (i = 0; i < 24; i+=2) {
			a = __t1_framer_in8(wc, 0x70 + (i>>1));/* PEF RS1 + x */
			/* Get high channel in low bits */
			rxs = (a & 0xf);
			if (!(wc->span.chans[i+1].sig & ZT_SIG_CLEAR)) {
				if (wc->span.chans[i+1].rxsig != rxs)
					zt_rbsbits(&wc->span.chans[i+1], rxs);
			}
			rxs = (a >> 4) & 0xf;
			if (!(wc->span.chans[i].sig & ZT_SIG_CLEAR)) {
				if (wc->span.chans[i].rxsig != rxs)
					zt_rbsbits(&wc->span.chans[i], rxs);
			}
		}
	}
}

static void __t1_pef_setup(struct t1 *wc)
{
	printk("wpr1: PEF2256 parameters setup %s \n", wc->spantype == E1 ? "E1" : "T1" );

	/* Global Clock for - PR1 card : default is 10 Mhz */
	switch(clockfreq)
	{
#ifdef FALC56_2_2
		case WPR1_CLK8:
		/* Global clocks (8.192 Mhz CLK) */
		t1_framer_out8(wc, FALC56_GCM1, 0x00);	
		t1_framer_out8(wc, FALC56_GCM2, 0x18);
		t1_framer_out8(wc, FALC56_GCM3, 0xfb);
		t1_framer_out8(wc, FALC56_GCM4, 0x0b);
		t1_framer_out8(wc, FALC56_GCM5, 0x00);
		t1_framer_out8(wc, FALC56_GCM6, 0x0b);
		t1_framer_out8(wc, FALC56_GCM7, 0xdb);
		t1_framer_out8(wc, FALC56_GCM8, 0xdf);
		break;

		case WPR1_CLK2:
		t1_framer_out8(wc, FALC56_GCM1, 0x00);
		t1_framer_out8(wc, FALC56_GCM2, 0x18);
		t1_framer_out8(wc, FALC56_GCM3, 0xfb);
		t1_framer_out8(wc, FALC56_GCM4, 0x0b);
		t1_framer_out8(wc, FALC56_GCM5, 0x00);
		t1_framer_out8(wc, FALC56_GCM6, 0x2f);
		t1_framer_out8(wc, FALC56_GCM7, 0xdb);
		t1_framer_out8(wc, FALC56_GCM8, 0xdf);
		break;

		default:
		case WPR1_CLK10:
		t1_framer_out8(wc, FALC56_GCM1, 0x40);
		t1_framer_out8(wc, FALC56_GCM2, 0x1b);
		t1_framer_out8(wc, FALC56_GCM3, 0x3d);
		t1_framer_out8(wc, FALC56_GCM4, 0x0a);
		t1_framer_out8(wc, FALC56_GCM5, 0x00);
		t1_framer_out8(wc, FALC56_GCM6, 0x07);
		t1_framer_out8(wc, FALC56_GCM7, 0xc9);
		t1_framer_out8(wc, FALC56_GCM8, 0xdc);
		break;
#else
		case WPR1_CLK8:
                /* Global clocks (8.192 Mhz CLK) */
                t1_framer_out8(wc, FALC56_GCM1, 0x00);
                t1_framer_out8(wc, FALC56_GCM2, 0x00);
                t1_framer_out8(wc, FALC56_GCM3, 0xfb);
                t1_framer_out8(wc, FALC56_GCM4, 0x0b);
                t1_framer_out8(wc, FALC56_GCM5, 0x02);
                t1_framer_out8(wc, FALC56_GCM6, 0x30);
                t1_framer_out8(wc, FALC56_GCM7, 0xa0);
                t1_framer_out8(wc, FALC56_GCM8, 0xdf);
                break;

                case WPR1_CLK2:
                t1_framer_out8(wc, FALC56_GCM1, 0x00);
                t1_framer_out8(wc, FALC56_GCM2, 0x00);
                t1_framer_out8(wc, FALC56_GCM3, 0xfb);
                t1_framer_out8(wc, FALC56_GCM4, 0x0b);
                t1_framer_out8(wc, FALC56_GCM5, 0x00);
                t1_framer_out8(wc, FALC56_GCM6, 0x3f);
                t1_framer_out8(wc, FALC56_GCM7, 0x80);
                t1_framer_out8(wc, FALC56_GCM8, 0xdf);
                break;

                default:
                case WPR1_CLK10:
                t1_framer_out8(wc, FALC56_GCM1, 0xa2);
                t1_framer_out8(wc, FALC56_GCM2, 0x0f);
                t1_framer_out8(wc, FALC56_GCM3, 0x3d);
                t1_framer_out8(wc, FALC56_GCM4, 0x0a);
                t1_framer_out8(wc, FALC56_GCM5, 0x03);
                t1_framer_out8(wc, FALC56_GCM6, 0x34);
                t1_framer_out8(wc, FALC56_GCM7, 0x90);
                t1_framer_out8(wc, FALC56_GCM8, 0xdc);
                break;
#endif
	}


        t1_framer_out8(wc, FALC56_GCR, 0x40);  /* GCR: Interrupt on Activation/Deactivation of AIX, LOS - Power ON  */

#ifdef WPR_8MBDATA
	t1_framer_out8(wc, FALC56_SIC1, 0xC2); /* SIC1: 8.192 Mhz clock/bus, do        uble buffer receive / transmit, byte interleaved */
#else
	t1_framer_out8(wc, FALC56_SIC1, 0x02); /* SIC1: 2.048 Mhz clock/bus, do        uble buffer receive / transmit, byte interleaved */
#endif

	/* Configure system interface */
	t1_framer_out8(wc, FALC56_SIC2, 0x00); /* SIC2: No FFS, no center receive eliastic buffer, phase 0 */
	t1_framer_out8(wc, FALC56_SIC3, 0x0C);	/* SIC3: Edges for capture */
	t1_framer_out8(wc, FALC56_CMR1, 0x28);	/* CMR1: RCLK is at 2 Mhz dejittered- Disable Clock Switch to SYNC */
	t1_framer_out8(wc, FALC56_CMR2, 0xE5);	/* CMR2: ECFAX - ECFAR - DCOXC - IXSC */

	t1_framer_out8(wc, FALC56_XC0, 0x00);	/* XC0: Normal operation of Sa-bits */
	t1_framer_out8(wc, FALC56_XC1, 0x04);	/* XC1: 0 offset */
	t1_framer_out8(wc, FALC56_RC0, 0x00);	/* RC0: Just shy of 255 */
	t1_framer_out8(wc, FALC56_RC1, 0x05);	/* RC1: The rest of RC0 */
	
	/* Configure ports */
	t1_framer_out8(wc, FALC56_PC1, 0x00);	/* PC1: SPYR/SPYX input on RPA/XPA */
	t1_framer_out8(wc, FALC56_PC2, 0xBB);	/* PC2: Init to Low  - drive LED1 */
	t1_framer_out8(wc, FALC56_PC3, 0xBB);	/* PC3: Init to Low - drive LED2  */
	t1_framer_out8(wc, FALC56_PC4, 0xBB);	/* PC4: Init to Low - unused */
	t1_framer_out8(wc, FALC56_PC5, 0x33);	/* PC5: XMFS unset, SCLKR is output, RCLK is output, CLK1 and CLK2 are output */

	t1_framer_out8(wc, FALC56_PC6, 0x0A);	/* PC6: TP4-TP5  CLK1 is 4.096 Mhz, CLK2 is 4.096 Mhz from DCO-R*/
	t1_framer_out8(wc, FALC56_LCR1, 0x00);	/* Clear LCR1 */

	printk("PR1-Appliance: Successfully initialized serial bus for card\n");
}

static void __t1_configure_t1(struct t1 *wc, int lineconfig, int txlevel)
{
	unsigned int fmr4, fmr2, fmr1, fmr0, lim2;
	char *framing, *line;
	int mytxlevel;

	if ((txlevel > 7) || (txlevel < 4))
		mytxlevel = 0;
	else
		mytxlevel = txlevel - 4;

	// Use fmr1 below for mapping chanmap = 0
      	//fmr1 = 0x1c; /* FMR1: Mode 0, T1 mode, CRC on for ESF, 8.192/2.048 Mhz         system data rate, no XAIS */
	// Use fmr1 below for mapping chanmap = 1
      	fmr1 = 0x9c; /* FMR1: Mode 0, T1 mode, CRC on for ESF, 8.192/2.048 Mhz         system data rate, no XAIS */
        fmr2 = 0x22; /* FMR2: no payload loopback, auto send yellow alarm */

	if (loopback)
		fmr2 |= 0x4;

	if (j1mode)
		fmr4 = 0x1c;
	else
		fmr4 = 0x0c; /* FMR4: Lose sync on 2 out of 5 framing bits, auto resync */

	lim2 = 0x21; 			/* LIM2: 50% peak is a "1", Advanced Loss recovery */
	lim2 |= 0x02;			/* DPN: set MPAS bit in LIM2 register so we get proper 100 Ohm line 
					   termination for T1. AS1-AS2 analog switch and external resistor 
					   is used for this purpose*/ 
	lim2 |= (mytxlevel << 6);	/* LIM2: Add line buildout */

	__t1_framer_out8(wc, FALC56_FMR1, fmr1);
	__t1_framer_out8(wc, FALC56_FMR2, fmr2);

	/* Configure line interface */
	if (lineconfig & ZT_CONFIG_AMI) 
	{
		line = "AMI";
		fmr0 = 0xa0;
	} 
	else 
	{
		line = "B8ZS";
		fmr0 = 0xf0;
	}
	if (lineconfig & ZT_CONFIG_D4) 
	{
		framing = "D4";
	} 
	else 
	{
		framing = "ESF";
		fmr4 |= 0x2;
		fmr2 |= 0xc0;
	}

	__t1_framer_out8(wc, FALC56_FMR0, fmr0);

	__t1_framer_out8(wc, FALC56_XSW_FMR4, fmr4);
	__t1_framer_out8(wc, FALC56_XSP_FMR5, 0x40);	/* FMR5: Enable RBS mode */

	__t1_framer_out8(wc, FALC56_LIM1, 0xf8);	/* LIM1: Clear data in case of LOS, Set receiver threshold (0.5V), No remote loop, no DRS */

								    /*DPN: +(wc->sync==0) master/slave clock switching based on the framing parameter*/
								    /*inside span configuration line in zaptel.conf*/	
	__t1_framer_out8(wc, FALC56_LIM0, 0x08+(wc->sync==0));      /*LIM0: Enable auto long haul mode, no local loop (must be after LIM1) */


	__t1_framer_out8(wc, FALC56_CMDR, 0x50);	/* CMDR: Reset the receiver and transmitter line interface */
	__t1_framer_out8(wc, FALC56_CMDR, 0x00);	/* CMDR: Reset the receiver and transmitter line interface */

	__t1_framer_out8(wc, FALC56_LIM2, lim2);	/* LIM2: 50% peak amplitude is a "1" */
	__t1_framer_out8(wc, FALC56_PCD, 0x0a);	/* PCD: LOS after 176 consecutive "zeros" */
	__t1_framer_out8(wc, FALC56_PCR, 0x15);	/* PCR: 22 "ones" clear LOS */

	if (j1mode)
		__t1_framer_out8(wc, FALC56_RC0, 0x80); /* J1 overide */
		
	/* Generate pulse mask for T1 */
	switch(mytxlevel) {
	case 3:
		__t1_framer_out8(wc, FALC56_XPM0, 0x07);	/* XPM0 */
		__t1_framer_out8(wc, FALC56_XPM1, 0x01);	/* XPM1 */
		__t1_framer_out8(wc, FALC56_XPM2, 0x00);	/* XPM2 */
		break;
	case 2:
		__t1_framer_out8(wc, FALC56_XPM0, 0x8c);	/* XPM0 */
		__t1_framer_out8(wc, FALC56_XPM1, 0x11);	/* XPM1 */
		__t1_framer_out8(wc, FALC56_XPM2, 0x01);	/* XPM2 */
		break;
	case 1:
		__t1_framer_out8(wc, FALC56_XPM0, 0x8c);	/* XPM0 */
		__t1_framer_out8(wc, FALC56_XPM1, 0x01);	/* XPM1 */
		__t1_framer_out8(wc, FALC56_XPM2, 0x00);	/* XPM2 */
		break;
	case 0:
	default:
		__t1_framer_out8(wc, FALC56_XPM0, 0xd7);	/* XPM0 */
		__t1_framer_out8(wc, FALC56_XPM1, 0x22);	/* XPM1 */
		__t1_framer_out8(wc, FALC56_XPM2, 0x01);	/* XPM2 */
		break;
	}
	printk("wpr1: Span configured for %s/%s\n", framing, line);
}

static void __t1_configure_e1(struct t1 *wc, int lineconfig)
{
	unsigned int fmr2, fmr1, fmr0;
	unsigned int cas = 0;
	char *crc4 = "";
	char *framing, *line;

        if (debug) printk("wpr1 : configure E1\n");
	fmr1 = 0x44; /* FMR1: E1 mode, Automatic force resync, PCM30 mode, 8.192 Mhz / 2048 Mhz backplane, no XAIS */
	fmr2 = 0x03; /* FMR2: Auto transmit remote alarm, auto loss of multiframe recovery, no payload loopback */
	if (loopback)
		fmr2 |= 0x4;

	if (lineconfig & ZT_CONFIG_CRC4) {
		fmr1 |= 0x08;	/* CRC4 transmit */
		fmr2 |= 0xc0;	/* CRC4 receive */
		crc4 = "/CRC4";
	}
	__t1_framer_out8(wc, FALC56_FMR1, fmr1);
	__t1_framer_out8(wc, FALC56_FMR2, fmr2);

	/* Configure line interface */
	if (lineconfig & ZT_CONFIG_AMI) {
		line = "AMI";
		fmr0 = 0xa0;
	} else {
		line = "HDB3";
		fmr0 = 0xf0;
	}

	if (lineconfig & ZT_CONFIG_CCS) {
		framing = "CCS";
	} else {
		framing = "CAS";
		cas = 0x40;
	}
	__t1_framer_out8(wc, FALC56_FMR0, fmr0);

	__t1_framer_out8(wc, FALC56_LIM1, 0xf0 /*| 0x6 */ );	/* LIM1: Clear data in case of LOS, Set receiver threshold (0.5V), No remote loop, no DRS */

                                                                    /*DPN: +(wc->sync==0) master/slave clock switching based on the framing parameter*/                                                                                                                           /*inside span configuration line in zaptel.conf*/
        __t1_framer_out8(wc, FALC56_LIM0, 0x08+(wc->sync==0));      /*LIM0: Enable auto long haul mode, no local loop (must be after LIM1) */


	__t1_framer_out8(wc, FALC56_CMDR, 0x50);	/* CMDR: Reset the receiver and transmitter line interface */
	__t1_framer_out8(wc, FALC56_CMDR, 0x00);	/* CMDR: Reset the receiver and transmitter line interface */

	/* Condition receive line interface for E1 after reset  */
	__t1_framer_out8(wc, FALC56_BB, 0x17);
	__t1_framer_out8(wc, FALC56_BC, 0x55);
	__t1_framer_out8(wc, FALC56_BB, 0x97);
	__t1_framer_out8(wc, FALC56_BB, 0x11);
	__t1_framer_out8(wc, FALC56_BC, 0xaa);
	__t1_framer_out8(wc, FALC56_BB, 0x91);
	__t1_framer_out8(wc, FALC56_BB, 0x12);
	__t1_framer_out8(wc, FALC56_BC, 0x55);
	__t1_framer_out8(wc, FALC56_BB, 0x92);
	__t1_framer_out8(wc, FALC56_BB, 0x0c);
	__t1_framer_out8(wc, FALC56_BB, 0x00);
	__t1_framer_out8(wc, FALC56_BB, 0x8c);
	
	__t1_framer_out8(wc, FALC56_LIM2, 0x20);	/* LIM2: 50% peak amplitude is a "1" */
	__t1_framer_out8(wc, FALC56_PCD, 0x0a);	/* PCD: LOS after 176 consecutive "zeros" */
	__t1_framer_out8(wc, FALC56_PCR, 0x15);	/* PCR: 22 "ones" clear LOS */
	
	__t1_framer_out8(wc, FALC56_XSW_FMR4, 0x9f);	/* XSW: Spare bits all to 1 */
	__t1_framer_out8(wc, FALC56_XSP_FMR5, 0x1c|cas);	/* XSP: E-bit set when async. AXS auto, XSIF to 1 */
	
	/* Generate pulse mask for E1 */
	__t1_framer_out8(wc, FALC56_XPM0, 0x54);	/* XPM0 */
	__t1_framer_out8(wc, FALC56_XPM1, 0x02);	/* XPM1 */
	__t1_framer_out8(wc, FALC56_XPM2, 0x00);	/* XPM2 */


	printk("wpr1: Span configured for %s/%s%s\n", framing, line, crc4);
}

static void pr1_framer_start(struct t1 *wc, struct zt_span *span)
{
	int alreadyrunning = wc->span.flags & ZT_FLAG_RUNNING;
	unsigned long flags;

        if (debug) printk("wpr1 : pr1_framer_start\n");

	spin_lock_irqsave(&wc->lock, flags);

	if (wc->spantype == E1) 
	{ 
		__t1_configure_e1(wc, span->lineconfig);
	} 
	else 
	{ 
		__t1_configure_t1(wc, span->lineconfig, span->txlevel);
		__t1_set_clear(wc);
	}
	
	if (!alreadyrunning) 
		wc->span.flags |= ZT_FLAG_RUNNING;

	spin_unlock_irqrestore(&wc->lock, flags);
}


static int pr1_startup(struct zt_span *span)
{
	struct t1 *wc = span->pvt;
	int i,alreadyrunning = span->flags & ZT_FLAG_RUNNING;

	if(debug)
		printk("wpr1: pr1_startup - Calling startup (flags is %d)\n", span->flags);

	/* initialize the start value for the entire chunk of last ec buffer */
	for(i = 0; i < span->channels; i++)
	{
		memset(wc->ec_chunk1[i],
			ZT_LIN2X(0,&span->chans[i]),ZT_CHUNKSIZE);
		memset(wc->ec_chunk2[i],
			ZT_LIN2X(0,&span->chans[i]),ZT_CHUNKSIZE);
	}

	/* Reset framer with proper parameters and start */
	pr1_framer_start(wc, span);

	if (!alreadyrunning) {
		/* Only if we're not already going */
		pr1_dma_sport_start();
		pr1_enable_interrupts(wc);
		span->flags |= ZT_FLAG_RUNNING;
	}
	return 0;
}

static int pr1_shutdown(struct zt_span *span)
{
	struct t1 *wc = span->pvt;
	unsigned long flags;

	if (debug)
		printk("wpr1 : pr1_shutdown \n");

	spin_lock_irqsave(&wc->lock, flags);
	__t1_framer_out8(wc, FALC56_GCR, 0x41);	/* GCR: Interrupt on Activation/Deactivation of AIX, LOS */
	pr1_dma_sport_stop();
	span->flags &= ~ZT_FLAG_RUNNING;
	spin_unlock_irqrestore(&wc->lock, flags);
	return 0;
}


static int pr1_chanconfig(struct zt_chan *chan, int sigtype)
{
	struct t1 *wc = chan->pvt;
	unsigned long flags;
	int alreadyrunning = chan->span->flags & ZT_FLAG_RUNNING;

	if (debug)
		printk("wpr1 : pr1_chanconfig \n");

	spin_lock_irqsave(&wc->lock, flags);

	if (alreadyrunning && (wc->spantype != E1))
		__t1_set_clear(wc);

	spin_unlock_irqrestore(&wc->lock, flags);
	return 0;
}

static int pr1_spanconfig(struct zt_span *span, struct zt_lineconfig *lc)
{
	struct t1 *wc = span->pvt;

	if (debug)
		printk("wpr1 : pr1_spanconfig \n");

	span->lineconfig = lc->lineconfig;
	span->txlevel = lc->lbo;
	span->rxlevel = 0;
	/* Do we want to SYNC on receive or not */
	wc->sync = lc->sync;
	/* If already running, apply changes immediately */
	if (span->flags & ZT_FLAG_RUNNING)
		return pr1_startup(span);
	return 0;
}


static int pr1_software_init(struct t1 *wc)
{
 int x;
	if (debug)
		printk("wpr1 : pr1_software_init \n");

 	pr1_sport_start(wc, regular_interrupt_processing, ZT_CHUNKSIZE, debug);

	wc->num = 1;
	sprintf(wc->span.name, "WCT1/%d", wc->num);
	sprintf(wc->span.desc, "%s Card %d", wc->variety, wc->num);

	wc->span.manufacturer 	= "SwitchVoice";
	strncpy(wc->span.devicetype, wc->variety, sizeof(wc->span.devicetype) - 1);
	wc->span.spanconfig 	= pr1_spanconfig;
	wc->span.chanconfig 	= pr1_chanconfig;
	wc->span.startup 	= pr1_startup;
	wc->span.shutdown 	= pr1_shutdown;
	wc->span.rbsbits 	= t1_rbsbits;
	wc->span.maint 		= __t1_maint;
	wc->span.open 		= pr1_open;
	wc->span.close 		= pr1_close;
#if defined(ECHO_CAN_ZARLINK)
	int res = echo_can_wrap_init();
	if( res == 0 ){
		wc->span.echocan = echo_can_wrap_tail_length;
	}
#endif
	if (wc->spantype == E1) {
		wc->span.channels = 31;
		wc->span.linecompat = ZT_CONFIG_HDB3 | ZT_CONFIG_CCS | ZT_CONFIG_CRC4;
		wc->span.deflaw = ZT_LAW_ALAW;
		wc->span.spantype = "E1";
	} else {
		wc->span.channels = 24;
		wc->span.linecompat = ZT_CONFIG_AMI | ZT_CONFIG_B8ZS | ZT_CONFIG_D4 | ZT_CONFIG_ESF;
		wc->span.deflaw = ZT_LAW_MULAW;
		wc->span.spantype = "T1";
	}
	wc->span.chans = wc->chans;
	wc->span.flags = ZT_FLAG_RBS;
	wc->span.ioctl = pr1_ioctl;
	wc->span.pvt = wc;

	init_waitqueue_head(&wc->span.maintq);

	for (x=0;x<wc->span.channels;x++) {
		sprintf(wc->chans[x].name, "WCT1/%d/%d", wc->num, x + 1);
		wc->chans[x].sigcap = ZT_SIG_EM | ZT_SIG_CLEAR | ZT_SIG_EM_E1 | 
				      ZT_SIG_FXSLS | ZT_SIG_FXSGS | 
				      ZT_SIG_FXSKS | ZT_SIG_FXOLS | ZT_SIG_DACS_RBS |
				      ZT_SIG_FXOGS | ZT_SIG_FXOKS | ZT_SIG_CAS | ZT_SIG_SF;
		wc->chans[x].pvt = wc;
		wc->chans[x].chanpos = x + 1;
	}
	if (zt_register(&wc->span, 0)) {
		printk("Unable to register span with zaptel\n");
		return -1;
	}
	return 0;
}

static inline void __t1_handle_leds(struct t1 *wc)
{
	int oldreg;


	if (wc->span.alarms & (ZT_ALARM_RED | ZT_ALARM_BLUE)) {
		/* Red/Blue alarm */
		wc->blinktimer++;
#ifdef FANCY_ALARM
		if (wc->blinktimer == (altab[wc->alarmpos] >> 1)) {
			wc->ledtestreg = (wc->ledtestreg | BIT_LED1) & ~BIT_LED0;
			pr1_set_led(wc);
		}
		if (wc->blinktimer >= 0xf) {
			wc->ledtestreg = wc->ledtestreg & ~(BIT_LED0 | BIT_LED1);
			pr1_set_led(wc);
			wc->blinktimer = -1;
			wc->alarmpos++;
			if (wc->alarmpos >= (sizeof(altab) / sizeof(altab[0])))
				wc->alarmpos = 0;
		}
#else
		if (wc->blinktimer == 160) {
			wc->ledtestreg = (wc->ledtestreg | BIT_LED1) & ~BIT_LED0;
			pr1_set_led(wc);
		} else if (wc->blinktimer == 480) {
			wc->ledtestreg = wc->ledtestreg & ~(BIT_LED0 | BIT_LED1);
			pr1_set_led(wc);
			wc->blinktimer = 0;
		}
#endif
	} else if (wc->span.alarms & ZT_ALARM_YELLOW) {
		/* Yellow Alarm */
		if (!(wc->blinktimer % 2)) 
			wc->ledtestreg = (wc->ledtestreg | BIT_LED1) & ~BIT_LED0;
		else
			wc->ledtestreg = (wc->ledtestreg | BIT_LED0) & ~BIT_LED1;
			pr1_set_led(wc);
	} else {
		/* No Alarm */
		oldreg = wc->ledtestreg;
		if (wc->span.maintstat != ZT_MAINT_NONE)
			wc->ledtestreg |= BIT_TEST;
		else
			wc->ledtestreg &= ~BIT_TEST;
		if (wc->span.flags & ZT_FLAG_RUNNING)
			wc->ledtestreg = (wc->ledtestreg | BIT_LED0) & ~BIT_LED1;
		else
			wc->ledtestreg = wc->ledtestreg & ~(BIT_LED0 | BIT_LED1);
		if (oldreg != wc->ledtestreg)
			pr1_set_led(wc);
	}
}

static void pr1_transmitprep(struct t1 *wc, u8 *pTransmit)
{
	volatile unsigned char *txbuf;
	int x,y;

	txbuf = pTransmit;

	zt_transmit(&wc->span);

	for (y=0;y<ZT_CHUNKSIZE;y++) 
	{
		for (x=0;x<wc->span.channels;x++) 
		{
			txbuf[((WPR_NBCHAN * y) + (wc->chanmap[x] +  1))]= wc->chans[x].writechunk[y];
		}
	//*(txbuf+(y*32)+3)=0x04;
	}

}

static void pr1_receiveprep(struct t1 *wc, u8 *pReceive)
{
	volatile unsigned char *rxbuf;
	int x;
	int y;

	rxbuf=pReceive;

	for (y=0;y<ZT_CHUNKSIZE;y++) 
	{
		for (x=0;x<wc->span.channels;x++) 
		{
			/* XXX Optimize, remove * and + XXX */
			/* Must map received channels into appropriate data */
			wc->chans[x].readchunk[y] = 
			rxbuf[((WPR_NBCHAN * y) + (wc->chanmap[x] +  1))]; 
		}
	}

	for (x=0;x<wc->span.channels;x++) 
	{
		zt_ec_chunk(&wc->chans[x], wc->chans[x].readchunk, wc->ec_chunk2[x]);
		memcpy(wc->ec_chunk2[x],wc->ec_chunk1[x],ZT_CHUNKSIZE);
		memcpy(wc->ec_chunk1[x],wc->chans[x].writechunk,ZT_CHUNKSIZE);
	}
	zt_receive(&wc->span);
}

static void __t1_check_alarms(struct t1 *wc)
{
	unsigned char c,d;
	int alarms;
	int x,j;

	if (!(wc->span.flags & ZT_FLAG_RUNNING))
		return;

	c = __t1_framer_in8(wc, 0x4c); /* PEF FRS0 Framer Status 0 */
	d = __t1_framer_in8(wc, 0x4d);

	/* Assume no alarms */
	alarms = 0;

	/* And consider only carrier alarms */
	wc->span.alarms &= (ZT_ALARM_RED | ZT_ALARM_BLUE | ZT_ALARM_NOTOPEN);

	if (wc->spantype == E1) {
		if (c & 0x04) {
			/* No multiframe found, force RAI high after 400ms only if
			   we haven't found a multiframe since last loss
			   of frame */
			if (!(wc->spanflags & FLAG_NMF)) {
				__t1_framer_out8(wc, FALC56_XSW_FMR4, 0x9f | 0x20);	/* PEF XSW -  LIM0: Force RAI High */
				wc->spanflags |= FLAG_NMF;
				printk("NMF workaround on!\n");
			}
			__t1_framer_out8(wc, FALC56_FMR2, 0xc3);	/* PEF FMR 2 Reset to CRC4 mode */
			__t1_framer_out8(wc, FALC56_FMR0, 0xf2);	/* PEF FMR 0 Force Resync */
			__t1_framer_out8(wc, FALC56_FMR0, 0xf0);	/* PEF FMR 0 Force Resync */
		} else if (!(c & 0x02)) {
			if ((wc->spanflags & FLAG_NMF)) {
				__t1_framer_out8(wc, FALC56_XSW_FMR4, 0x9f);	/* PEF XSW LIM0: Clear forced RAI */
				wc->spanflags &= ~FLAG_NMF;
				printk("NMF workaround off!\n");
			}
		}
	} else {
		/* Detect loopup code if we're not sending one */
		if ((!wc->span.mainttimer) && (d & 0x08)) {
			/* Loop-up code detected */
			if ((wc->loopupcnt++ > 80)  && (wc->span.maintstat != ZT_MAINT_REMOTELOOP)) {
				__t1_framer_out8(wc, FALC56_LIM0, 0x08);	/* LIM0: Disable any local loop */
				__t1_framer_out8(wc, FALC56_LIM1, 0xf6 );	/* LIM1: Enable remote loop */
				wc->span.maintstat = ZT_MAINT_REMOTELOOP;
			}
		} else
			wc->loopupcnt = 0;
		/* Same for loopdown code */
		if ((!wc->span.mainttimer) && (d & 0x10)) {
			/* Loop-down code detected */
			if ((wc->loopdowncnt++ > 80)  && (wc->span.maintstat == ZT_MAINT_REMOTELOOP)) {
				__t1_framer_out8(wc, FALC56_LIM0, 0x08);	/* LIM0: Disable any local loop */
				__t1_framer_out8(wc, FALC56_LIM1, 0xf0 );	/* LIM1: Disable remote loop */
				wc->span.maintstat = ZT_MAINT_NONE;
			}
		} else
			wc->loopdowncnt = 0;
	}

	if (wc->span.lineconfig & ZT_CONFIG_NOTOPEN) {
		for (x=0,j=0;x < wc->span.channels;x++)
			if ((wc->span.chans[x].flags & ZT_FLAG_OPEN) ||
			    (wc->span.chans[x].flags & ZT_FLAG_NETDEV))
				j++;
		if (!j)
			alarms |= ZT_ALARM_NOTOPEN;
	}

	if (c & 0xa0) {
		if (wc->alarmcount >= alarmdebounce) { 
			alarms |= ZT_ALARM_RED;
			wc->ledtestreg = (wc->ledtestreg | BIT_LED1) & ~BIT_LED0;
                        pr1_set_led(wc);
		}   else
			wc->alarmcount++;
	} else
		wc->alarmcount = 0;
	if (c & 0x4) 
		alarms |= ZT_ALARM_BLUE;

	if (((!wc->span.alarms) && alarms) || 
	    (wc->span.alarms && (!alarms))) 
		wc->checktiming = 1;

	/* Keep track of recovering */
	if ((!alarms) && wc->span.alarms) 
		wc->alarmtimer = ZT_ALARMSETTLE_TIME;
	if (wc->alarmtimer) {
		alarms |= ZT_ALARM_RECOVER;
		wc->ledtestreg = wc->ledtestreg  | (BIT_LED0 | BIT_LED1);
                pr1_set_led(wc);
	}

	/* If receiving alarms, go into Yellow alarm state */
	if (alarms && !(wc->spanflags & FLAG_SENDINGYELLOW)) {
		unsigned char fmr4;

		if (debug) printk("wpr1: Setting yellow alarm\n");

		/* We manually do yellow alarm to handle RECOVER and NOTOPEN, otherwise it's auto anyway */
		fmr4 = __t1_framer_in8(wc, 0x20);
		__t1_framer_out8(wc, FALC56_XSW_FMR4, fmr4 | 0x20);
		wc->spanflags |= FLAG_SENDINGYELLOW;
	} else if ((!alarms) && (wc->spanflags & FLAG_SENDINGYELLOW)) {
		unsigned char fmr4;

		if(debug) printk("wpr1: Clearing yellow alarm\n");

		/* We manually do yellow alarm to handle RECOVER  */
		fmr4 = __t1_framer_in8(wc, 0x20);
		__t1_framer_out8(wc, FALC56_XSW_FMR4, fmr4 & ~0x20);
		wc->spanflags &= ~FLAG_SENDINGYELLOW;
	}

	/* Re-check the timing source when we enter/leave alarm, not withstanding
	   yellow alarm */
	if (c & 0x10)
		alarms |= ZT_ALARM_YELLOW;
	if (wc->span.mainttimer || wc->span.maintstat) 
		alarms |= ZT_ALARM_LOOPBACK;
	wc->span.alarms = alarms;
	zt_alarm_notify(&wc->span);
}

static void __t1_do_counters(struct t1 *wc)
{
	if (wc->alarmtimer) 
	{
		if (!--wc->alarmtimer) 
		{
			wc->span.alarms &= ~(ZT_ALARM_RECOVER);
			zt_alarm_notify(&wc->span);
		}
	}
}

void regular_interrupt_processing(u8 *read_samples, u8 *write_samples) 
{
	struct t1 *wc = pCard;
	int x;

	wc->intcount++;

        pr1_transmitprep(wc, write_samples);
        pr1_receiveprep(wc, read_samples);


	if (!(wc->intcount % 0x1000)) /* do it every 1/2 sec */
	__t1_handle_leds(wc);
	__t1_do_counters(wc);


	/* Do some things that we don't have to do very often */
	x = wc->intcount & 15 /* 63 */;
	switch(x) {
	case 0:
	case 1:
		break;
	case 2:
		__t1_check_sigbits(wc);
		break;
	case 4:
		/* Check alarms 1/4 as frequently */
		if (!(wc->intcount & 0x30))
			__t1_check_alarms(wc);
		break;
	}
	
}

static int pr1_dma_init(struct t1 *wc)
{
#if (defined(CONFIG_BF532) || defined(CONFIG_BF533))
  /* Set up DMA1 to receive, map DMA1 to Sport0 RX */
  bfin_write_DMA1_PERIPHERAL_MAP(DMA_PERIPHERAL_MAP_RX);
  bfin_write_DMA1_IRQ_STATUS(bfin_read_DMA1_IRQ_STATUS() | 0x2);
#endif
#if (defined(CONFIG_BF536) || defined(CONFIG_BF537))
  bfin_write_DMA3_PERIPHERAL_MAP(DMA_PERIPHERAL_MAP_RX);
  bfin_write_DMA3_IRQ_STATUS(bfin_read_DMA3_IRQ_STATUS() | DMA_ERR);
#endif
  
#if ((L1_DATA_A_LENGTH != 0) || (L1_DATA_B_LENGTH != 0))
  iRxBuffer1 = (char*)l1_data_sram_alloc(2*ZT_CHUNKSIZE*WPR_NBCHAN);
#else	
  { 
    dma_addr_t addr;
    iRxBuffer1 = (char*)dma_alloc_coherent(NULL, 2*ZT_CHUNKSIZE*WPR_NBCHAN, &addr, 0);
  }
#endif
  
  if(!iRxBuffer1)
  {
	printk("wpr1 : Error allocating iRxBuffer");
	return -ENOMEM;
  }
  if (debug)
    printk("iRxBuffer1 = 0x%x\n", (int)iRxBuffer1);

  /* Start address of data buffer */
#if (defined(CONFIG_BF532) || defined(CONFIG_BF533))
  bfin_write_DMA1_START_ADDR(iRxBuffer1);
  /* DMA inner loop count */
  bfin_write_DMA1_X_COUNT(ZT_CHUNKSIZE*WPR_NBCHAN);

  /* Inner loop address increment */
  bfin_write_DMA1_X_MODIFY(1);
  bfin_write_DMA1_Y_MODIFY(1);
  bfin_write_DMA1_Y_COUNT(2);	
	
  /* Configure DMA1
     8-bit transfers, Interrupt Enable,Interrupt after each row, Autobuffer mode */
  bfin_write_DMA1_CONFIG(WNR | WDSIZE_8 | DI_EN | DMAFLOW_AUTO | DI_SEL | DMA2D); 

  /* Set up DMA2 to transmit, map DMA2 to Sport0 TX */
  bfin_write_DMA2_PERIPHERAL_MAP(DMA_PERIPHERAL_MAP_TX);
#endif
#if (defined(CONFIG_BF536) || defined(CONFIG_BF537))
  bfin_write_DMA3_START_ADDR(iRxBuffer1);
  /* DMA inner loop count */
  bfin_write_DMA3_X_COUNT(ZT_CHUNKSIZE*WPR_NBCHAN);

  /* Inner loop address increment */
  bfin_write_DMA3_X_MODIFY(1);
  bfin_write_DMA3_Y_MODIFY(1);
  bfin_write_DMA3_Y_COUNT(2);

  /* Configure DMA3
     8-bit transfers, Interrupt Enable,Interrupt after each row, Autobuffer mode */
  bfin_write_DMA3_CONFIG(WNR | WDSIZE_8 | DI_EN | DMAFLOW_AUTO | DI_SEL | DMA2D);

  /* Set up DMA4 to transmit, map DMA4 to Sport0 TX */
  bfin_write_DMA4_PERIPHERAL_MAP(DMA_PERIPHERAL_MAP_TX);
#endif

#if ((L1_DATA_A_LENGTH != 0) || (L1_DATA_B_LENGTH != 0))
  iTxBuffer1 = (char*)l1_data_sram_alloc(2*ZT_CHUNKSIZE*WPR_NBCHAN);
#else	
  { 
    dma_addr_t addr;
    iTxBuffer1 = (char*)dma_alloc_coherent(NULL, 2*ZT_CHUNKSIZE*WPR_NBCHAN, &addr, 0);
  }
#endif
 if (!iTxBuffer1)
  {
	printk("wpr1 : Error allocating iTxBuffer");
	return -ENOMEM;
  }

  if (debug)
    printk("iTxBuffer1 = 0x%x\n", (int)iTxBuffer1);

#if (defined(CONFIG_BF532) || defined(CONFIG_BF533))

  /* Start address of data buffer */
  bfin_write_DMA2_START_ADDR(iTxBuffer1);

  /* DMA inner loop count */
  bfin_write_DMA2_X_COUNT(ZT_CHUNKSIZE*WPR_NBCHAN);

  /* Inner loop address increment */
  bfin_write_DMA2_X_MODIFY(1);
  bfin_write_DMA2_Y_MODIFY(1);
  bfin_write_DMA2_Y_COUNT(2);
	
  /* Configure DMA2 8-bit transfers, Autobuffer mode */
  /*bfin_write_DMA2_CONFIG( WDSIZE_8 | DI_EN | DMAFLOW_AUTO | DI_SEL | DMA2D); */
  bfin_write_DMA2_CONFIG( WDSIZE_8 | DMAFLOW_AUTO |DI_SEL|  DMA2D); 
#endif
#if (defined(CONFIG_BF536) || defined(CONFIG_BF537))

  /* Start address of data buffer */
  bfin_write_DMA4_START_ADDR(iTxBuffer1);

  /* DMA inner loop count */
  bfin_write_DMA4_X_COUNT(ZT_CHUNKSIZE*WPR_NBCHAN);

  /* Inner loop address increment */
  bfin_write_DMA4_X_MODIFY(1);
  bfin_write_DMA4_Y_MODIFY(1);
  bfin_write_DMA4_Y_COUNT(2);

  /* Configure DMA4 8-bit transfers, Autobuffer mode */
  /*bfin_write_DMA4_CONFIG( WDSIZE_8 | DI_EN | DMAFLOW_AUTO | DI_SEL | DMA2D); */
  bfin_write_DMA4_CONFIG( WDSIZE_8 | DMAFLOW_AUTO |DI_SEL|  DMA2D);

#endif

__builtin_bfin_ssync();

 return 0;
}

static void pr1_dma_close(void)
{
#if ((L1_DATA_A_LENGTH != 0) || (L1_DATA_B_LENGTH != 0))
  l1_data_sram_free(iTxBuffer1);
  l1_data_sram_free(iRxBuffer1);
#else
  dma_free_coherent(NULL, 2*ZT_CHUNKSIZE*WPR_NBCHAN, iTxBuffer1, 0);
  dma_free_coherent(NULL, 2*ZT_CHUNKSIZE*WPR_NBCHAN, iRxBuffer1, 0);
#endif
}

static void pr1_sport0_init(void)
{
       /* Disable SPORT0 before init */
	bfin_write_SPORT0_TCR1(0);
	bfin_write_SPORT0_TCR2(0);
	bfin_write_SPORT0_RCR1(0);
	bfin_write_SPORT0_RCR2(0);

        /* Set SPORT0 in Multichannel mode  */
#ifdef WPR_8MBDATA
        bfin_write_SPORT0_MCMC1(0xF000); /* Window Size 128 (4 w by chan) - Offset 0 */
#else
        bfin_write_SPORT0_MCMC1(0x3000); /* Window Size 32 - Offset 0 */
#endif

#ifdef WPR_8MBDATA
        /* iet 32 channels TX */
	bfin_write_SPORT0_MTCS0(0x11111111);
	bfin_write_SPORT0_MTCS1(0x11111111);
	bfin_write_SPORT0_MTCS2(0x11111111);
	bfin_write_SPORT0_MTCS3(0x11111111);

        /* Set 32 channels RX */
	bfin_write_SPORT0_MRCS0(0x11111111);
	bfin_write_SPORT0_MRCS1(0x11111111);
	bfin_write_SPORT0_MRCS2(0x11111111);
	bfin_write_SPORT0_MRCS3(0x11111111);
#else
        /* iet 32 channels TX */
        bfin_write_SPORT0_MTCS0(0xffffffff);
        bfin_write_SPORT0_MTCS1(0x00000000);
        bfin_write_SPORT0_MTCS2(0x00000000);
        bfin_write_SPORT0_MTCS3(0x00000000);

        /* Set 32 channels RX */
        bfin_write_SPORT0_MRCS0(0xffffffff);
        bfin_write_SPORT0_MRCS1(0x00000000);
        bfin_write_SPORT0_MRCS2(0x00000000);
        bfin_write_SPORT0_MRCS3(0x00000000);
#endif
        /* Initialize SPORT0 Transmitter */
        /* We don't have to init the TSCLKx freq as we are in Multichan mode */
        /* 7+1 bits of serial word length */
	bfin_write_SPORT0_TCR2(7);

	/* (SPORTx_TFSDIV) specify how many receive clock cycles are counted before generating TFS */
#ifdef WPR_8MBDATA
         bfin_write_SPORT0_TFSDIV(1023); /* 8khz FSYNC as received CLK fr        om 2256 is 16.384Mhz */
#else
         bfin_write_SPORT0_TFSDIV(255); /* 8khz FSYNC as received CLK fro        m 2256 is 2.048Mhz */
#endif

        /* Initialize SPORT0 Receiver */
        /* in Multichan mode RCLK will be used for both Tx & Rx */
	/* RCLK is received from PEF2256 so no need to init */ 

        /* 7+1 bits of serial word length */
	bfin_write_SPORT0_RCR2(7);

	/* (SPORTx_RFSDIV) specify how many receive clock cycles are counted before generating RFS */
#ifdef WPR_8MBDATA
         bfin_write_SPORT0_RFSDIV(1023); /* 8khz FSYNC as received CLK fr        om 2256 is 16.384Mhz */
#else
         bfin_write_SPORT0_RFSDIV(255); /* 8khz FSYNC as received CLK fro        m 2256 is 2.048Mhz */
#endif

        /* Don't enable SPORT right now  */
	bfin_write_SPORT0_TCR1(LTFS); //DPN: data sampled at the negative clock edge so the e/c is working

	bfin_write_SPORT0_RCR1(IRFS|LRFS|RFSR); //DPN: data sampled at the negative clock edge so the e/c is working


        bfin_write_SPORT0_MCMC2(0x000c); /* Multichannel - Offset 0 - TX DMA - RX DMA */

	__builtin_bfin_ssync();
}


static int pr1_sport_start(
  struct t1 *wc,
  void (*isr_callback)(u8 *read_samples, u8 *write_samples), 
  int samples,
  int debug
)
{
if (debug) printk("wpr1: pr1_sport_start \n");


  pr1_isr_callback = isr_callback;

  pr1_sport0_init();
  if(pr1_dma_init(wc))
  {
	if (debug) printk("wpr1: dma_init_failed \n");
	return -ENOMEM;
  }

  return 0;
}

static void pr1_sport_stop()
{
if (debug) printk("wpr1: pr1_sport_stop \n");

  pr1_dma_sport_stop();
  pr1_disable_interrupts();
  free_irq(IRQ_SPORT0_RX, NULL);
}

static int pr1_hardware_init(struct t1 *wc)
{
	unsigned int falcver;
	unsigned int x;
	
	if (debug) printk("wpr1: pr1_hardware_init \n");

	/* create the /proc file */
	Our_Proc_File = create_proc_entry(PROCFS_NAME, 0644, NULL);
	
	if (Our_Proc_File == NULL) {
		//remove_proc_entry(PROCFS_NAME, &proc_root); //Penev: new kernel doesn't export proc_root structure. 
		remove_proc_entry(PROCFS_NAME, NULL);
		printk("Error: Could not initialize /proc/%s\n", PROCFS_NAME);
		return -ENOMEM;
	}

	Our_Proc_File->read_proc  = wpr1_proc_read;
	Our_Proc_File->write_proc = wpr1_proc_write;
	Our_Proc_File->owner 	  = THIS_MODULE;
	Our_Proc_File->mode 	  = S_IFREG | S_IRUGO;
	Our_Proc_File->uid 	  = 0;
	Our_Proc_File->gid 	  = 0;
	Our_Proc_File->size 	  = 37;

	if(pr1_check_framer(wc)) 
		return  -EIO;


	/* set span type */
	if (t1e1mode==E1)
	{
		printk("wpr1 : Span type is E1 \n");
		wc->spantype = E1;
		wc->chanmap = chanmap_e1;
	}
	else
	{
		printk("wpr1 : Span type is T1 \n");
		wc->spantype = T1;
		wc->chanmap = chanmap_t1;
	}

	__t1_pef_setup(wc);

	/* reset Led */
	wc->ledtestreg = 0;
	pr1_set_led(wc);

	/* Perform register test on FALC */	
	for (x=0;x<16;x++) {
		t1_framer_out8(wc, FALC56_IMR0, x);
		if ((falcver = t1_framer_in8(wc, FALC56_IMR0)) != x) 
			printk("wpr1 : FALC test - Wrote '%x' but read '%x'\n", x, falcver);
	}

	start_alarm(wc);

	return 0;
}

static int __init pr1_init()
{
	int res;
	struct t1 *wc;
	static int bInitDone=0;


	if (debug) printk("wpr1: pr1_init \n");
	if (bInitDone) return 0;

	if(fakemode) printk("wpr1 : FAKE mode activated \n");
	
	wc = kmalloc(sizeof(struct t1), GFP_KERNEL);
	if (wc) {
		pCard=wc;
		memset(wc, 0x0, sizeof(struct t1));
		spin_lock_init(&wc->lock);

		if(fakemode)
		{
			tFakeBuffer = kmalloc(FAKEBUFLEN, GFP_KERNEL);
			wc->ioaddr = (unsigned long)tFakeBuffer;
			memset((void *)wc->ioaddr, 0x0, FAKEBUFLEN);
		}
		else
			wc->ioaddr = bf_resource_start;

		wc->offset = 0;	/* And you thought 42 was the answer */
				/* MP : Next release of BF1 may find the real question */
		/* Initialize hardware */
		pr1_hardware_init(wc);

		/* We now know which version of card we have */
		wc->variety = "PR1-Appliance T1/E1";

		/* Misc. software stuff */
		pr1_software_init(wc);

		printk("Found a Wildcard: %s\n", wc->variety);
		res = 0;
	} else
	  {
		res = -ENOMEM;
	  }

	if(res==0)
		bInitDone=1;

	printk("w1pr : init done \n");

	return res;
}

static void __exit pr1_cleanup(void)
{
#if defined(ECHO_CAN_ZARLINK)
	echo_can_wrap_shutdown();
#endif
		pr1_sport_stop();
		pr1_dma_close();
		pCard->ledtestreg = 0;

	if(!fakemode)
	{
		pr1_set_led(pCard);
	}
}

#ifdef LINUX26
module_param(alarmdebounce, int, 0600);
module_param(loopback, int, 0600);
module_param(t1e1mode, int, 0600);
module_param(clockfreq, int, 0600);
module_param(debug, int, 0600);
module_param(j1mode, int, 0600);
module_param(fakemode, int, 0600);
#else
MODULE_PARM(alarmdebounce, "i");
MODULE_PARM(loopback, "i");
MODULE_PARM(t1e1mode, "i");
MODULE_PARM(clockfreq, "i");
MODULE_PARM(debug, "i");
MODULE_PARM(j1mode, "i");
MODULE_PARM(fakemode, "i");
#endif
MODULE_DESCRIPTION("Wildcard PR1-Appliance Zaptel Driver");
MODULE_AUTHOR("Astfin - Team");
#ifdef MODULE_LICENSE
MODULE_LICENSE("GPL");
#endif

module_init(pr1_init);
module_exit(pr1_cleanup);
