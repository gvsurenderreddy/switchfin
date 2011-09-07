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
 /*****************************************************************
 * History :
 *
 * 26/02/07 : MP - Initial file 
 * 08/12/07 : Mark - adapted for pr1-appliance
 * 16/12/07 : Pawel Pastuszak - added sport0 support for Blackfin BF537
 * 04/14/08 : Mark @ Astfin . org - Overall code cleanup
 * 12/23/09 : dpn@switchfin.org - Dimitar Penev, migration to DAHDI
 *
 * Copyright @ 2010 SwitchFin <dpn@switchfin.org>
 *****************************************************************/
#ifndef __F56_H
#define __F56_H

#define SSYNC __builtin_bfin_ssync()

#ifdef WPR1_DEBUG
#define PRINTK(args...) PRINTK(args)
#else
#define PRINTK(args...)
#endif

#define BF_ASYNMEMBASE  0x20000000
#define BF_ASYNMEMSIZE  0x00100000
#define BF_ASYNBANKNUM	0x1

#define bf_resource_start   (BF_ASYNMEMBASE+(BF_ASYNMEMSIZE*BF_ASYNBANKNUM))

/* define for sport 0 values for BF537 */

#if (defined(CONFIG_BF536) || defined(CONFIG_BF537))
#define DMA_PERIPHERAL_MAP_RX	0x3000
#define DMA_PERIPHERAL_MAP_TX	0x4000
#define SCI_IMASK_SPORT0_RX 	0x00000020

#endif

/* BF533 or BF532 */
#if (defined(CONFIG_BF532) || defined(CONFIG_BF533))

#define DMA_PERIPHERAL_MAP_RX 	0x1000
#define DMA_PERIPHERAL_MAP_TX 	0x2000
#define SCI_IMASK_SPORT0_RX 	0x00000200

#endif


/* Define to get more attention-grabbing but slightly more I/O using alarm status */
//#define FANCY_ALARM
#define FANCY_ALARM

/* Offset between transmit and receive */

#define BIT_LED1	(1 << 0)
#define BIT_LED0	(1 << 1)
#define BIT_TEST	(1 << 2)

#define FLAG_NMF 		(1 << 1)
#define FLAG_SENDINGYELLOW 	(1 << 2)
#define FLAG_FALC12		(1 << 3)

#define	T1	1		/* is a T1 card */
#define	E1	2		/* is an E1 card */

#define WPR1_CLK10	0	/* defautl PEF2256 Clock Xtal 10Mhz */
#define WPR1_CLK8	1	/* Xtal 8,192Mhz */
#define WPR1_CLK2	2	/* Xtal 2,096Mhz */

#define WPR1_CLK_SLAVE	0	/* Default mode : clock Slave */
#define WPR1_CLK_MASTER	1	/* Clock Master mode */

#define WC_OFFSET 4




#define IDLE             0x00
#define ENABLE_ALL       0x00
#define DISABLE_ALL      0xFF
#define ON               0x01
#define OFF              0x00
#define ACTIVE_HIGH      0x01
#define ACTIVE_LOW       0x00
#define HA_FISU          0x01 /* SS7 High Address modes */
#define HA_LSSU          0x02 /* SS7 High Address modes */
#define HA_MSU           0x03 /* SS7 High Address modes */

/* ----------Basic FALC56 device settings-------------*/
#define NUM_OF_FALC          1
#define F56_OFFSET         0x100 /*size in BYTE */
#define F56_OFFSET_DUINT   0x40  /*size in DUINT*/
#define NUM_OF_HDLC_CONTROLLERS 3


/* -------- offsets for write only registers --------------------------*/
#define FALC56_XFIFO     0x00   /* 00/01    Transmission FIFO                        */
#define FALC56_CMDR      0x02   /* 02       Command Register                         */
#define FALC56_DEC       0x60   /* 60       Disable Error Counter                    */
#define FALC56_XS        0x70   /* 70-7F    Transmit CAS Register                    */
#define FALC56_CMDR2     0x87   /* 87       Command Register no.2                    */
#define FALC56_CMDR3     0x88   /* 88       Command Register 3                       */
#define FALC56_CMDR4     0x89   /* 89       Command Register 4                       */
#define FALC56_XFIFO2    0x9C   /* 9C-9D    Transmit FIFO 2                          */
#define FALC56_XFIFO3    0x9E   /* 9E-9F    Transmit FIFO 3                          */

/* ------------ offsets for read and write registers ------------------*/
#define FALC56_MODE      0x03   /* 03       Mode Register                            */
#define FALC56_RAH1      0x04   /* 04       Receive Address High 1                   */
#define FALC56_RAH2      0x05   /* 05       Receive Address High 2                   */
#define FALC56_RAL1      0x06   /* 06       Receive Address Low 1                    */
#define FALC56_RAL2      0x07   /* 07       Receive Address Low 2                    */
#define FALC56_IPC       0x08   /* 08       Interrupt Port Configuration             */
#define FALC56_CCR1      0x09   /* 09       Common Configuration Register 1          */
#define FALC56_CCR2      0x0A   /* 0A       Common Configuration Register 3          */
#define FALC56_PRE       0x0B   /* 0B       Preamble Register                        */
#define FALC56_RTR1      0x0C   /* 0C       Receive Timeslot Register 1              */
#define FALC56_RTR2      0x0D   /* 0D       Receive Timeslot Register 2              */
#define FALC56_RTR3      0x0E   /* 0E       Receive Timeslot Register 3              */
#define FALC56_RTR4      0x0F   /* 0F       Receive Timeslot Register 4              */
#define FALC56_TTR1      0x10   /* 10       Transmit Timeslot Register 1             */
#define FALC56_TTR2      0x11   /* 11       Transmit Timeslot Register 2             */
#define FALC56_TTR3      0x12   /* 12       Transmit Timeslot Register 3             */
#define FALC56_TTR4      0x13   /* 13       Transmit Timeslot Register 4             */
#define FALC56_IMR0      0x14   /* 14       Interrupt Mask Register 0                */
#define FALC56_IMR1      0x15   /* 15       Interrupt Mask Register 1                */
#define FALC56_IMR2      0x16   /* 16       Interrupt Mask Register 2                */
#define FALC56_IMR3      0x17   /* 17       Interrupt Mask Register 3                */
#define FALC56_IMR4      0x18   /* 18       Interrupt Mask Register 4                */
#define FALC56_IMR5      0x19   /* 19       Interrupt Mask Register 5                */
#define FALC56_IERR      0x1B   /* 1B       Single Bit Insertion Register            */
#define FALC56_FMR0      0x1C   /* 1C       Framer Mode Register 0                   */
#define FALC56_FMR1      0x1D   /* 1D       Framer Mode Register 1                   */
#define FALC56_FMR2      0x1E   /* 1E       Framer Mode Register 2                   */
#define FALC56_LOOP      0x1F   /* 1F       Channel Loop Back                        */
#define FALC56_XSW_FMR4  0x20   /* 20       Transmit Service UINT                    */
                                /*          Framer Mode Reigster 4                   */
#define FALC56_XSP_FMR5  0x21   /* 21       Transmit Spare Bits                      */
                                /*          Framer Mode Reigster 5                   */
#define FALC56_XC0       0x22   /* 22       Transmit Control 0                       */
#define FALC56_XC1       0x23   /* 23       Transmit Control 1                       */
#define FALC56_RC0       0x24   /* 24       Receive Control 0                        */
#define FALC56_RC1       0x25   /* 25       Receive Control 1                        */
#define FALC56_XPM0      0x26   /* 26       Transmit Pulse Mask 0                    */
#define FALC56_XPM1      0x27   /* 27       Transmit Pulse Mask 1                    */
#define FALC56_XPM2      0x28   /* 28       Transmit Pulse Mask 2                    */
#define FALC56_TSWM      0x29   /* 29       Transparent Service UINT Mask            */
#define FALC56_IDLE      0x2B   /* 2B       Idle Channel Code                        */
#define FALC56_XSA4_XDL1 0x2C   /* 2C       Transmit SA4 Bit Register                */
                                /*          Fransmit DL-Bit Register 1               */
#define FALC56_XSA5_XDL2 0x2D   /* 2D       Transmit SA5 Bit Register                */
                                /*          Fransmit DL-Bit Register 2               */
#define FALC56_XSA6_XDL3 0x2E   /* 2E       Transmit SA6 Bit Register                */
                                /*          Fransmit DL-Bit Register 3               */
#define FALC56_XSA7_CCB1 0x2F   /* 2F       Transmit SA7 Bit Register                */
                                /*          Clear Channel Register 1                 */
#define FALC56_XSA8_CCB2 0x30   /* 30       Transmit SA8 Bit Register                */
                                /*          Clear Channel Register 2                 */
#define FALC56_FMR3_CCB3 0x31   /* 31       Framer Mode Reg. 3                       */
                                /*          Clear Channel Register 3                 */
#define FALC56_ICB1      0x32   /* 32       Idle Channel Register 1                  */
#define FALC56_ICB2      0x33   /* 33       Idle Channel Register 2                  */
#define FALC56_ICB3      0x34   /* 34       Idle Channel Register 3                  */
#define FALC56_ICB4      0x35   /* 35       Idle Channel Register 4                  */
#define FALC56_LIM0      0x36   /* 36       Line Interface Mode 0                    */
#define FALC56_LIM1      0x37   /* 37       Line Interface Mode 1                    */
#define FALC56_PCD       0x38   /* 38       Pulse Count Detection                    */
#define FALC56_PCR       0x39   /* 39       Pulse Count Recovery                     */
#define FALC56_LIM2      0x3A   /* 3A       Line Interface Mode Register 2           */
#define FALC56_LCR1      0x3B   /* 3B       Line Code Register 1                     */
#define FALC56_LCR2      0x3C   /* 3C       Line Code Register 2                     */
#define FALC56_LCR3      0x3D   /* 3D       Line Code Register 3                     */
#define FALC56_SIC1      0x3E   /* 3E       System Interface Control 1               */
#define FALC56_SIC2      0x3F   /* 3F       System Interface Control 2               */
#define FALC56_SIC3      0x40   /* 40       System Interface Control 3               */
#define FALC56_CMR1      0x44   /* 44       Clock Mode Register 1                    */
#define FALC56_CMR2      0x45   /* 45       Clock Mode Register 2                    */
#define FALC56_GCR       0x46   /* 46       Global Configuration Register            */
#define FALC56_CMR3      0x48   /* 48       Clock Mode Register 3                    */
#define FALC56_ESM       0x47   /* 47       Errored Second Mask                      */
#define FALC56_PC1       0x80   /* 80       Port Configuration 1                     */
#define FALC56_PC2       0x81   /* 81       Port Configuration 2                     */
#define FALC56_PC3       0x82   /* 82       Port Configuration 3                     */
#define FALC56_PC4       0x83   /* 83       Port Configuration 4                     */
#define FALC56_PC5       0x84   /* 84       Port Configuration 5                     */
#define FALC56_PC6       0x86   /* 86       Port Configuration 6                     */
#define FALC56_GPC1      0x85   /* 85       Global Port Configuration 1              */
#define FALC56_CCR3      0x8B   /* 8B       Common Configuration Register 3          */
#define FALC56_CCR4      0x8c   /* 8C       Common Configuration Register 4          */
#define FALC56_CCR5      0x8D   /* 8D       Common Configuration Register 5          */
#define FALC56_MODE2     0x8E   /* 8E       Mode Register 2                          */
#define FALC56_MODE3     0x8F   /* 8F       Mode Register 3                          */
#define FALC56_GCM1      0x92   /* 92       Global Clocking Modes                    */
#define FALC56_GCM2      0x93   /* 93       Channel Interrupt Status                 */
#define FALC56_GCM3      0x94   /* 94       Global Clocking Modes                    */
#define FALC56_GCM4      0x95   /* 95       Channel Interrupt Status                 */
#define FALC56_GCM5      0x96   /* 96       Global Clocking Modes                    */
#define FALC56_GCM6      0x97   /* 97       Global Clocking Modes                    */
#define FALC56_GCM7      0x98   /* 98       Global Clocking Modes                    */
#define FALC56_GCM8      0x99   /* 99       Global Clocking Modes                    */
#define FALC56_TSEO      0xA0   /* A0       Time Slot Even/Odd Select                */
#define FALC56_TSBS1     0xA1   /* A1       Time Slot Bit Select 1                   */
#define FALC56_TSBS2     0xA2   /* A2       Time Slot Bit Select 2                   */
#define FALC56_TSBS3     0xA3   /* A3       Time Slot Bit Select 3                   */
#define FALC56_TSS2      0xA4   /* A4       Time Slot Select 2                       */
#define FALC56_TSS3      0xA5   /* A5       Time Slot Select 3                       */
#define FALC56_TPC0      0xA8   /* A8       Test Pattern Control 0                   */
#define FALC56_GLC1      0xAF   /* AF       Global Line Control Register 1           */
#define FALC56_BB        0xBB   /* BB       Receiver Sensitivity optimizatoin        */
#define FALC56_BC        0xBC   /* BC       Receiver Sensitivity optimization        */

/* -------- offsets for read only registers --------------------------*/
#define FALC56_RFIFO     0x00   /* 00/01    Receive FIFO                             */
#define FALC56_RBD       0x49   /* 49       Receive Buffer Delay                     */
#define FALC56_VSTR      0x4A   /* 4A       Version Status                           */
#define FALC56_RES       0x4B   /* 4B       Receive Equalizer Status                 */
#define FALC56_FRS0      0x4C   /* 4C       Framer Receive Status 0                  */
#define FALC56_FRS1      0x4D   /* 4D       Framer Receive Status 1                  */
#define FALC56_RSW_FRS2  0x4E   /* 4E       Receive Service UINT                     */
                                /*          Framer Receive Status 2                  */
#define FALC56_RSP       0x4F   /* 4F       Receive Spare Bits                       */
#define FALC56_FEC       0x50   /* 50/51    Framing Error Counter                    */
#define FALC56_CVC       0x52   /* 52/53    Code Violation Counter                   */
#define FALC56_CEC1      0x54   /* 54/55    CRC Error Counter 1                      */
#define FALC56_EBC       0x56   /* 56/57    E-Bit Error Counter                      */
#define FALC56_CEC2      0x58   /* 58/59    CRC Error Counter 2   (E1)               */
#define FALC56_BEC       0x58   /* 58/59    Bit Error Counter     (T1)               */
                                /* 58/59    CRC Error Counter 2   (E1); Bit Error Counter     (T1) */
#define FALC56_CEC3      0x5A   /* 5A/5B    CRC Error Counter 3 (E1)                 */
#define FALC56_COFA      0x5A   /* 5A       Cofa Event Counter (T1)                  */ 
                                    /* 5A/5B    CRC Error Counter 3 (E1); Cofa Event Counter (T1) */
#define FALC56_RSA4_RDL1 0x5C   /* 5C       Receive SA4 Bit Register                 */
                                /*          Receive DL-Bit Register 1                */
#define FALC56_RSA5_RDL2 0x5D   /* 5D       Receive SA5 Bit Register                 */
                                /*          Receive DL-Bit Register 2                */
#define FALC56_RSA6_RDL3 0x5E   /* 5E       Receive SA6 Bit Register                 */
                                /*          Receive DL-Bit Register 3                */
#define FALC56_RSA7      0x5F   /* 5F       Receive SA7 Bit Register                 */
#define FALC56_RSA8      0x60   /* 60       Receive SA8 Bit Register                 */
#define FALC56_RSA6S     0x61   /* 61       Receive Sa6 Bit Status Register          */
#define FALC56_RSP1      0x62   /* 62       Receive Signaling Pointer 1              */
#define FALC56_RSP2      0x63   /* 63       Receive Signaling Pointer 2              */
#define FALC56_SIS       0x64   /* 64       Signaling Status Register                */
#define FALC56_RSIS      0x65   /* 65       Receive Signaling Status Register        */
#define FALC56_RBC       0x66   /* 66/67    Receive Byte Control                     */
#define FALC56_ISR0      0x68   /* 68       Interrupt Status Register 0              */
#define FALC56_ISR1      0x69   /* 69       Interrupt Status Register 1              */
#define FALC56_ISR2      0x6A   /* 6A       Interrupt Status Register 2              */
#define FALC56_ISR3      0x6B   /* 6B       Interrupt Status Register 3              */
#define FALC56_ISR4      0x6C   /* 6C       Interrupt Status Register 4              */
#define FALC56_ISR5      0x6D   /* 6D       Interrupt Status Register 5              */
#define FALC56_GIS       0x6E   /* 6E       Global Interrupt Status                  */
#define FALC56_CIS       0x6F   /* 6F       Channel Interrupt Status                 */
#define FALC56_RS        0x70   /* 70-7F    Receive CAS Register   1...16            */
#define FALC56_RBC2      0x90   /* 90       Receive Byte Count Register 2            */
#define FALC56_RBC3      0x91   /* 91       Receive Byte Count Register 3            */
#define FALC56_SIS3      0x9A   /* 9A       Signaling Status Register 3              */
#define FALC56_RSIS3     0x9B   /* 9B       Receive Signaling Status Register 3      */
#define FALC56_RFIFO2    0x9C   /* 9C/9D    Receive FIFO 2                           */
#define FALC56_RFIFO3    0x9E   /* 9E/9F    Receive FIFO 3                           */
#define FALC56_SIS2      0xA9   /* A9       Signaling Status Register 2              */
#define FALC56_RSIS2     0xAA   /* AA       Receive Signaling Status Register 2      */
#define FALC56_MFPI1     0xAB   /* AB       Multi Function Port Input Status 1       */

struct bf_device_id {
        char *name;
        int flags;
};

struct t1 {
        unsigned int    irq;
	spinlock_t lock;
	int spantype;
	int spanflags;			/* Span flags */
	unsigned char txsigs[16];  	/* Copy of tx sig registers */
	int num;
	int alarmcount;			/* How much red alarm we've seen */
	int alarmdebounce;
	int offset;			/* Our offset for finding channel 1 */
	char *variety;
	unsigned int intcount;
	int usecount;
	int sync;
	int dead;
	int blinktimer;
	int alarmtimer;
	int checktiming;	/* Set >0 to cause the timing source to be checked */
	int loopupcnt;
	int loopdowncnt;
	int miss;
	int misslast;
	int *chanmap;
#ifdef FANCY_ALARM
	int alarmpos;
#endif
	unsigned char ledtestreg;
	unsigned char outbyte;
	unsigned long ioaddr;
	/* T1 signalling */
	unsigned char ec_chunk1[31][DAHDI_CHUNKSIZE];
	unsigned char ec_chunk2[31][DAHDI_CHUNKSIZE];
	unsigned char tempo[32];
	struct dahdi_span span;						/* Span */
	struct dahdi_chan chans[31];					/* Channels */
	struct dahdi_chan *_chans[31];
};

/* E1 - T1 related functions & PEF 2256 setup */

static inline u16 __t1_framer_in16(struct t1 *wc, const unsigned int reg);
static inline u8  __t1_framer_in8(struct t1 *wc, const unsigned int reg);
static inline unsigned int t1_framer_in8(struct t1 *wc, const unsigned int addr);
static inline void __t1_framer_out8(struct t1 *wc, const unsigned int reg, const unsigned int val);
static inline void t1_framer_out8(struct t1 *wc, const unsigned int addr, const unsigned int value);

static void __t1_set_clear(struct t1 *wc);
static int __t1_maint(struct dahdi_span *span, int cmd);
static int __t1_rbsbits(struct dahdi_chan *chan, int bits);
static void __t1_check_sigbits(struct t1 *wc);
static void __t1_pef_setup(struct t1 *wc);
static void __t1_configure_t1(struct t1 *wc, int lineconfig, int txlevel);
static void __t1_configure_e1(struct t1 *wc, int lineconfig);
static void __t1_check_alarms(struct t1 *wc);
static void __t1_do_counters(struct t1 *wc);

static inline void start_alarm(struct t1 *wc);
static inline void stop_alarm(struct t1 *wc);

static int pr1_open(struct dahdi_chan *chan);
static int pr1_close(struct dahdi_chan *chan);

static void pr1_release(struct t1 *wc);

static  int pr1_check_framer (struct t1 *wc);

static void pr1_dma_sport_start(void);
static void pr1_dma_sport_stop(void);

static int pr1_enable_interrupts(struct t1 *wc);
static void pr1_disable_interrupts(void);

static u8 *isr_write_processing(void);
static u8 *isr_read_processing(void);

static irqreturn_t sport_rx_isr(int irq, void *dev_id);


static void pr1_set_led(struct t1 *wc);

static int pr1_ioctl(struct dahdi_chan *chan, unsigned int cmd, unsigned long data);


static void pr1_framer_start(struct t1 *wc, struct dahdi_span *span);
static int pr1_startup(struct file *file, struct dahdi_span *span);
static int pr1_shutdown(struct dahdi_span *span);

static int pr1_chanconfig(struct file *file, struct dahdi_chan *chan, int sigtype);
static int pr1_spanconfig(struct file *file, struct dahdi_span *span, struct dahdi_lineconfig *lc);

static inline void __t1_handle_leds(struct t1 *wc);

static void pr1_transmitprep(struct t1 *wc, u8 *write_samples);
static void pr1_receiveprep(struct t1 *wc, u8 *read_samples);


static int pr1_dma_init(struct t1 *wc);
static void pr1_dma_close(void);

static void pr1_sport0_init(void);

static int pr1_sport_start(struct t1 *wc, void (*isr_callback)(u8 *read_samples, u8 *write_samples), int samples, int debug);
static void pr1_sport_stop(void);

static int pr1_software_init(struct t1 *wc);
static int pr1_hardware_init(struct t1 *wc);

void regular_interrupt_processing(u8 *read_samples, u8 *write_samples);

static int __init pr1_init(void);
static void __exit pr1_cleanup(void);

#endif /*__F56_H*/
