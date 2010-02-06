/*
 * U-boot - br4.c
 *
 * Prepared for BR4-Appliance board mark@astfin.org
 *
 * Copyright (c) 2005 blackfin.uclinux.org
 *
 * (C) Copyright 2000-2004
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <config.h>
#include <command.h>
#include <asm/blackfin.h>
#include "ether_bf537.h"
#include <environment.h>

DECLARE_GLOBAL_DATA_PTR;
#include <post.h>
#include <watchdog.h>


#define POST_WORD_ADDR 0xFF903FFC

int factory_reset = 0;
int hard_reset = 0;
int post_failed = 0;
static int post=0;
void led_blue_on();
void led_blue_off();
void led_red_on();
void led_red_off();

static u32 start_time;

extern int image_info (ulong addr);
/*
 * the bootldr command loads an address, checks to see if there
 *   is a Boot stream that the on-chip BOOTROM can understand,
 *   and loads it via the BOOTROM Callback. It is possible
 *   to also add booting from SPI, or TWI, but this function does
 *   not currently support that.
 */
int do_bootldr (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	ulong   addr, entry;
	ulong *data;

	/* Get the address */
	if (argc < 2) {
		addr = load_addr;
	} else {
		addr = simple_strtoul(argv[1], NULL, 16);
	}

	/* Check if it is a LDR file */
	data = (ulong *)addr;
	if ( *data == 0xFF800060 || *data == 0xFF800040 || *data == 0xFF800020 ) {
		/* We want to boot from FLASH or SDRAM */
		entry = _BOOTROM_BOOT_DXE_FLASH;
		printf ("## Booting ldr image at 0x%08lx ...\n", addr);
		if (icache_status ())
			icache_disable();
		if (dcache_status ())
			dcache_disable ();

		__asm__(
			"R7=%[a];\n"
			"P0=%[b];\n"
			"JUMP (P0);\n"
			:
			: [a] "d" (addr), [b] "a" (entry)
			: "R7", "P0" );

	} else {
		printf ("## No ldr image at address 0x%08lx\n", addr);
	}

	return 0;
}

U_BOOT_CMD(bootldr,2,0,do_bootldr,
	"bootldr - boot ldr image from memory\n",
	"[addr]\n         - boot ldr image stored in memory\n");


int br4_factory_defaults(void)
{
	printf("\nResetting uBoot and Persistent settings to factory defaults!!!!\n");
	setenv("bootdelay", "3");
	setenv("ipaddr", "192.168.1.100");
	setenv("serverip", "192.168.1.5");
	setenv("gatewayip", "192.168.1.1");
	setenv("netmask", "255.255.255.0");
	setenv("factory_default","0"); 
        setenv("hard_default","0");
	saveenv();
	setenv("factory_default","1"); 
	setenv("bootcmd","run restore_soft"); 
	return 0;
}
int br4_hard_reset(void)
{
	printf("\nResetting the BR4-Appliance to factory defaults!!!!\n");
        setenv("baudrate", "115200");
        setenv("bootdelay", "3");
        setenv("ipaddr", "192.168.1.100");
        setenv("serverip", "192.168.1.5");
        setenv("gatewayip", "192.168.1.1");
        setenv("netmask", "255.255.255.0");
	setenv("factory_default","0"); 
        setenv("hard_default","0");
        setenv("offset","0");
        saveenv();
        setenv("hard_default","1");
        setenv("bootcmd","run restore_hard");
        return 0;
}
int init_reset_gpio(void)
{
	*pPORTF_FER   &= ~PF2;
        *pPORTFIO_DIR &= ~PF2;
        *pPORTFIO_INEN|=  PF2;
	return 0;
}

int sw_reset_pressed(void)
{
	unsigned short value;
	value = *pPORTFIO & PF2;
	if(value != 0)
		return 0;
	else
		return 1;

}

int chkreset(void)
{
	int delta;
	int count = 0;
	start_time = get_timer(0);

	init_reset_gpio();

	led_blue_on();

	if (!sw_reset_pressed()) {
		printf("SW-Reset already high (Button released)\n");
		printf("Proceeding to normal boot sequence...\n");
		return 0;
	}

	printf("Waiting for SW-Reset button to be released.");


	while (1) {
		delta = get_timer(start_time);
		if (!sw_reset_pressed())
			break;

		if ((count > CFG_TIME_POST) && !post) {
			printf("\nWhen released now, POST tests will be started.");
			led_blue_on();
			led_red_on();
			post = 1;
		}

		if ((count > CFG_TIME_SOFT_RESET) && !factory_reset) {
			printf("\nWhen released now, uBoot factory default values"
			       " will be restored.");
			led_blue_off();
			factory_reset = 1;
		}

		if ((count > CFG_TIME_HARD_RESET) && !hard_reset) {
                        printf("\nWhen released now, uClinux image will be restored to"
                               " factory defaults.");
                        led_red_off();
                        hard_reset = 1;
                }


		udelay(1000);
		if (!(count++ % 1000))
			printf(".");
	}


	printf("\nSW-Reset Button released after %d milli-seconds!\n", count);
	
	return 0;

}

int checkboard(void)
{
#if (BFIN_CPU == ADSP_BF534)	
	printf("CPU:   ADSP BF534 Rev.: 0.%d\n", *pCHIPID >>28);
#elif (BFIN_CPU == ADSP_BF536)	
	printf("CPU:   ADSP BF536 Rev.: 0.%d\n", *pCHIPID >>28);
#else	
	printf("CPU:   ADSP BF537 Rev.: 0.%d\n", *pCHIPID >>28);
#endif
	printf("Board: BR4-Appliance\n");
	chkreset();
	if(post && !factory_reset && !hard_reset)
		setenv("post", "1");
	led_red_on();
	if(!post_failed)
		led_blue_on();
	return 0;
}

long int initdram(int board_type)
{
	DECLARE_GLOBAL_DATA_PTR;
#ifdef DEBUG
	int brate;
	char *tmp = getenv("baudrate");
	brate = simple_strtoul(tmp, NULL, 16);
	printf("Serial Port initialized with Baud rate = %x\n",brate);
	printf("SDRAM attributes:\n");
	printf("tRCD %d SCLK Cycles,tRP %d SCLK Cycles,tRAS %d SCLK Cycles"
	       "tWR %d SCLK Cycles,CAS Latency %d SCLK cycles \n",
	       3, 3, 6, 2, 3);
	printf("SDRAM Begin: 0x%x\n", CFG_SDRAM_BASE);
	printf("Bank size = %d MB\n", CFG_MAX_RAM_SIZE >> 20);
#endif
	gd->bd->bi_memstart = CFG_SDRAM_BASE;
	gd->bd->bi_memsize = CFG_MAX_RAM_SIZE;
	return CFG_MAX_RAM_SIZE;
}

#if defined(CONFIG_MISC_INIT_R)
/* miscellaneous platform dependent initialisations */
int misc_init_r(void)
{
//#if (BFIN_BOOT_MODE == BF537_BYPASS_BOOT)
	char nid[32];
	unsigned short *pMACaddr = (unsigned short *) 0x203F0000;
	u8  SrcAddr[6] = {0x00,0x50,0xC2,0x69,0x92,0x12};

#if (CONFIG_COMMANDS & CFG_CMD_NET)
	/* The 0xFF check here is to make sure we don't use the address
	 * in flash if it's simply been erased (aka all 0xFF values) */
	if (getenv("ethaddr") == NULL && ((pMACaddr[0] & 0xFF) != 0xFF)) {
		sprintf(nid, "%02x:%02x:%02x:%02x:%02x:%02x",
			pMACaddr[0] & 0xFF, pMACaddr[0] >> 8,
			pMACaddr[1] & 0xFF, pMACaddr[1] >> 8,
			pMACaddr[2] & 0xFF, pMACaddr[2] >> 8);
		setenv("ethaddr", nid);
	}

	if ( getenv("ethaddr") ) {
		SetupMacAddr(SrcAddr);
	}
#endif /* CONFIG_COMMANDS & CFG_CMD_NET */
//#endif /* BFIN_BOOT_MODE == BF537_BYPASS_BOOT */	

/* Initialize PF2 to be GPIO Out and High 
*pPORTF_FER &= ~PF2;
*pPORTFIO_DIR |= PF2;
*pPORTFIO_INEN &= ~PF2;
*pPORTFIO |= PF2; */

	if(factory_reset && !hard_reset) 
		br4_factory_defaults();
	if(hard_reset)
		br4_hard_reset();
        if (post && !factory_reset && !hard_reset) {
                printf("POST TEST STARTING.......\n");
                post_run(NULL, POST_RAM | post_bootmode_get(0));
        }

	return 0;
}

U_BOOT_CMD(
        freset,        CFG_MAXARGS,    1,      br4_factory_defaults,
        "freset  - uBoot factory reset\n",
);


/****************************************************
 * POST for LEDS
 ****************************************************/
int led_post_test(int flags)
{

	led_blue_on();
        udelay(1000000);
        printf("BLUE ON\n");
	led_red_on();
        udelay(1000000);
        printf("BLUE+RED ON\n");
	led_blue_off();
	udelay(1000000);
	printf("RED ON\n");
	udelay(1000000);
	led_red_off();
        return 0;
}


#endif /* CONFIG_MISC_INIT_R */

#ifdef CONFIG_POST
#if (BFIN_BOOT_MODE != BF537_BYPASS_BOOT)
int post_hotkeys_pressed(void)
{
		return 0;
}
#else
int post_hotkeys_pressed(void)
{
	int delay = 3;
	int i;
	unsigned short value;
	
	*pPORTF_FER   &= ~PF5;
	*pPORTFIO_DIR &= ~PF5;
	*pPORTFIO_INEN|=  PF5;
	
	printf("########Press SW10 to enter Memory POST########: %2d ",delay);	
	while(delay--){
		for(i=0;i<100;i++){
			value = *pPORTFIO & PF5;
			if(value != 0){
				break;
				}
			udelay(10000);		
			}
		printf("\b\b\b%2d ",delay);
	}
	printf("\b\b\b 0");
	printf("\n");
	if(value == 0)
		return 0;
	else
		{
		printf("Hotkey has been pressed, Enter POST . . . . . .\n");
		return 1;
		}
}
#endif
#endif

#if defined(CONFIG_POST) || defined(CONFIG_LOGBUFFER)
void post_word_store(ulong a)
{
	volatile ulong *save_addr =
		(volatile ulong *)POST_WORD_ADDR;
	*save_addr = a;
}

ulong post_word_load(void)
{
	volatile ulong *save_addr = 
		(volatile ulong *)POST_WORD_ADDR;
	return *save_addr;
}
#endif

#ifdef CONFIG_POST
int uart_post_test(int flags)
{
	return 0;
}

#define BLOCK_SIZE 0x10000
#define VERIFY_ADDR 0x2000000
//extern int erase_block_flash(int);
//extern int write_data(long lStart, long lCount, uchar *pnData);


int cache_post_test (int flags) {
	return 1;
}
int watchdog_post_test (int flags) {
	return 1;
}
int rtc_post_test (int flags) {
	return 1;
}
int cpu_post_test (int flags) {
	return 1;
}
int spi_post_test (int flags) {
	return 1;
}
int usb_post_test (int flags) {
	return 1;
}
int spr_post_test (int flags) {
	return 1;
}
int sysmon_post_test (int flags) {
	return 1;
}
int dsp_post_test (int flags) {
	return 1;
}
int codec_post_test (int flags) {
	return 1;
}

int sysmon_init_f (int flags) {
	return 1;
}

int sysmon_reloc(int flags) {
	return 1;
}

int flash_post_test(int flags)
{
	return 1;
}

/************************************************
 *  SW10 ---- PF5	SW11 ---- PF4		*
 *  SW12 ---- PF3	SW13 ---- PF2		*
 ************************************************/

int button_post_test(int flags)
{
	int i,delay = 5;
	unsigned short value = 0;	
	int result = 0;

        *pPORTF_FER   &= ~(PF5|PF4|PF3|PF2);
        *pPORTFIO_DIR &= ~(PF5|PF4|PF3|PF2);
        *pPORTFIO_INEN|=  (PF5|PF4|PF3|PF2);

        printf("\n--------Press SW10: %2d ",delay);
        while(delay--){
                for(i=0;i<100;i++){
                        value = *pPORTFIO & PF5;
                        if(value != 0){
                                break;
                                }
                        udelay(10000);
                        }
                printf("\b\b\b%2d ",delay);
        }
	if(value!=0)
		printf("\b\bOK");
	else{
		result = -1;
		printf("\b\bfailed");
	}	

	delay = 5;
	printf("\n--------Press SW11: %2d ",delay);
        while(delay--){
                for(i=0;i<100;i++){
                        value = *pPORTFIO & PF4;
                        if(value != 0){
                                break;
                                }
                        udelay(10000);
                        }
                printf("\b\b\b%2d ",delay);
        }
        if(value!=0)
                printf("\b\bOK");
        else{
                result = -1;
		printf("\b\bfailed");
	}

	delay = 5;
        printf("\n--------Press SW12: %2d ",delay);
        while(delay--){
                for(i=0;i<100;i++){
                        value = *pPORTFIO & PF3;
                        if(value != 0){
                                break;
                                }
                        udelay(10000);
                        }
                printf("\b\b\b%2d ",delay);
        }
        if(value!=0)
                printf("\b\bOK");
        else{
                result = -1;
                printf("\b\bfailed");
        }

	delay = 5;
        printf("\n--------Press SW13: %2d ",delay);
        while(delay--){
                for(i=0;i<100;i++){
                        value = *pPORTFIO & PF2;
                        if(value != 0){
                                break;
                                }
                        udelay(10000);
                        }
                printf("\b\b\b%2d ",delay);
        }
        if(value!=0)
                printf("\b\bOK");
        else{
                result = -1;
                printf("\b\bfailed");
        }
	printf("\n");
	return result;
}


#endif
