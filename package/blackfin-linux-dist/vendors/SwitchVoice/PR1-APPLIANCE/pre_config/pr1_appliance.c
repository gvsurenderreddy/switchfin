/*
 * File:         arch/blackfin/mach-bf537/boards/pr1_appliance.c
 * Based on:     arch/blackfin/mach-bf533/boards/stamp.c
 * Author:       Mark T <mark@astfin.org>
 *
 * Created:
 * Description:
 *
 * Modified:
 *               Copyright 2005 National ICT Australia (NICTA)
 *               Copyright 2004-2006 Analog Devices Inc.
 *               Copyright 2008 Astfin
 *		 Copyright @ 2010 SwitchFin <dpn@switchfin.org>
 *
 * Bugs:         Enter bugs at http://blackfin.uclinux.org/
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
 * along with this program; if not, see the file COPYING, or write
 * to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/physmap.h>
#include <linux/spi/spi.h>
#include <linux/spi/flash.h>
#include <linux/spi/mmc_spi.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <asm/dma.h>
#include <asm/bfin5xx_spi.h>
#include <asm/reboot.h>
#include <asm/portmux.h>

/*
 * Name the Board for the /proc/cpuinfo
 */
const char bfin_board_name[] = "PR1 APPLIANCE";


#if defined(CONFIG_RTC_DRV_BFIN) || defined(CONFIG_RTC_DRV_BFIN_MODULE)
static struct platform_device rtc_device = {
	.name = "rtc-bfin",
	.id   = -1,
};
#endif

#if defined(CONFIG_BFIN_MAC) || defined(CONFIG_BFIN_MAC_MODULE)
static struct platform_device bfin_mii_bus = {
        .name = "bfin_mii_bus",
};

static struct platform_device bfin_mac_device = {
        .name = "bfin_mac",
        .dev.platform_data = &bfin_mii_bus,
};
#endif

#if defined(CONFIG_SPI_BFIN) || defined(CONFIG_SPI_BFIN_MODULE)
/* all SPI peripherals info goes here */

#if defined(CONFIG_MTD_M25P80) \
	|| defined(CONFIG_MTD_M25P80_MODULE)
static struct mtd_partition bfin_spi_flash_partitions[] = {
	{
                .name = "bootloader",
                .size = 0x00020000,
                .offset = 0,
                .mask_flags = MTD_CAP_ROM
        },{
                .name = "bootloader-backup",
                .size = 0x20000,
                .offset = 0x20000
        }

};

static struct flash_platform_data bfin_spi_flash_data = {
	.name = "m25p80",
	.parts = bfin_spi_flash_partitions,
	.nr_parts = ARRAY_SIZE(bfin_spi_flash_partitions),
	.type = "m25p64",
};

/* SPI flash chip (m25p64) */
static struct bfin5xx_spi_chip spi_flash_chip_info = {
	.enable_dma = 0,         /* use dma transfer with this chip*/
	.bits_per_word = 8,
};
#endif

#if defined(CONFIG_MMC_SPI) || defined(CONFIG_MMC_SPI_MODULE)
static int bfin_mmc_spi_init(struct device *dev, irqreturn_t (*detect_int)(int, void *), void *data) {
        return 0;
}

static void bfin_mmc_spi_exit(struct device *dev, void *data){
}

static struct mmc_spi_platform_data bfin_mmc_spi_pdata = {
        .init = bfin_mmc_spi_init,
        .exit = bfin_mmc_spi_exit,
        .detect_delay = 500, /* msecs */
};

//This information is specific to the Blackfin SPI driver:
static struct bfin5xx_spi_chip  mmc_spi_chip_info = {
        .enable_dma = 1,
        .bits_per_word = 8,
};
#endif

static struct spi_board_info bfin_spi_board_info[] __initdata = {
#if defined(CONFIG_MTD_M25P80) \
	|| defined(CONFIG_MTD_M25P80_MODULE)
	{
		/* the modalias must be the same as spi device driver name */
		.modalias = "m25p80", /* Name of spi_driver for this device */
		.max_speed_hz = 25000000,     /* max spi clock (SCK) speed in HZ */
		.bus_num = 0, /* Framework bus number */
		.chip_select = 1, /* Framework chip select. On STAMP537 it is SPISSEL1*/
		.platform_data = &bfin_spi_flash_data,
		.controller_data = &spi_flash_chip_info,
		.mode = SPI_MODE_3,
	},
#endif

#if defined(CONFIG_MMC_SPI) || defined(CONFIG_MMC_SPI_MODULE)

        {
                .modalias = "mmc_spi",              	    // Name of the device drivers
                .max_speed_hz = 25000000,           	    // Maximum Clock Rate for SD cards
                .bus_num = 0,                       	    // Bus number
                .chip_select = 4,	    		    // We use SPISEL4 for PR1 to Select the SD card
                .platform_data = &bfin_mmc_spi_pdata, 	    // Platform data
                .controller_data = &mmc_spi_chip_info, 	    // Controller data

//              .mode = SPI_MODE_3,                 	    // Clock Polarity und Phase. Mode 3 MUST
                                                    	    // be used (CPHA=1 und CPOL=1), because
                                                    	    // Mode 0 sets the interface between the individual
						    	    // bytes in short, the chip select inactive. This is very important!
							    // In thi scase the SPI_CLK line should be pulled up 	

		.mode = SPI_MODE_0,			    // Now it seems Mode 0 is supported and in our case 
							    // we should use it as we have SPI_CLK pulled down
        },

#endif
};

/* SPI controller data */
static struct bfin5xx_spi_master bfin_spi0_info = {
	.num_chipselect = 8,
	.enable_dma = 1,  /* master has the ability to do dma transfer */
	.pin_req = {P_SPI0_SCK, P_SPI0_MISO, P_SPI0_MOSI, 0},
};

/* SPI (0) */
static struct resource bfin_spi0_resource[] = {
	[0] = {
		.start = SPI0_REGBASE,
		.end   = SPI0_REGBASE + 0xFF,
		.flags = IORESOURCE_MEM,
		},
	[1] = {
		.start = CH_SPI,
		.end   = CH_SPI,
		.flags = IORESOURCE_IRQ,
	},
};

static struct platform_device bfin_spi0_device = {
	.name = "bfin-spi",
	.id = 0, /* Bus number */
	.num_resources = ARRAY_SIZE(bfin_spi0_resource),
	.resource = bfin_spi0_resource,
	.dev = {
		.platform_data = &bfin_spi0_info, /* Passed to driver */
	},
};
#endif  /* spi master and devices */

#if defined(CONFIG_SERIAL_BFIN) || defined(CONFIG_SERIAL_BFIN_MODULE)
static struct resource bfin_uart_resources[] = {
#ifdef CONFIG_SERIAL_BFIN_UART0
	{
		.start = 0xFFC00400,
		.end = 0xFFC004FF,
		.flags = IORESOURCE_MEM,
	},
#endif
#ifdef CONFIG_SERIAL_BFIN_UART1
	{
		.start = 0xFFC02000,
		.end = 0xFFC020FF,
		.flags = IORESOURCE_MEM,
	},
#endif
};

static struct platform_device bfin_uart_device = {
	.name = "bfin-uart",
	.id = 1,
	.num_resources = ARRAY_SIZE(bfin_uart_resources),
	.resource = bfin_uart_resources,
};
#endif

#if defined(CONFIG_SERIAL_BFIN_SPORT) || defined(CONFIG_SERIAL_BFIN_SPORT_MODULE)
static struct platform_device bfin_sport0_uart_device = {
	.name = "bfin-sport-uart",
	.id = 0,
};

static struct platform_device bfin_sport1_uart_device = {
	.name = "bfin-sport-uart",
	.id = 1,
};
#endif


static struct platform_device *pr1_appliance_devices[] __initdata = {

#if defined(CONFIG_RTC_DRV_BFIN) || defined(CONFIG_RTC_DRV_BFIN_MODULE)
	&rtc_device,
#endif

#if defined(CONFIG_BFIN_MAC) || defined(CONFIG_BFIN_MAC_MODULE)
        &bfin_mii_bus,
        &bfin_mac_device,
#endif


#if defined(CONFIG_SPI_BFIN) || defined(CONFIG_SPI_BFIN_MODULE)
	&bfin_spi0_device,
#endif

#if defined(CONFIG_SERIAL_BFIN) || defined(CONFIG_SERIAL_BFIN_MODULE)
	&bfin_uart_device,
#endif

#if defined(CONFIG_SERIAL_BFIN_SPORT) || defined(CONFIG_SERIAL_BFIN_SPORT_MODULE)
	&bfin_sport0_uart_device,
	&bfin_sport1_uart_device,
#endif

};

static int __init pr1_appliance_init(void)
{
	printk(KERN_INFO "%s(): chip_id=%08lX,dspid=%08X\n",
                __FUNCTION__,
                *((volatile unsigned long *)CHIPID),
                bfin_read_DSPID());


	printk(KERN_INFO "%s(): registering device resources\n", __FUNCTION__);
	platform_add_devices(pr1_appliance_devices, ARRAY_SIZE(pr1_appliance_devices));
#if defined(CONFIG_SPI_BFIN) || defined(CONFIG_SPI_BFIN_MODULE)
	spi_register_board_info(bfin_spi_board_info,
				ARRAY_SIZE(bfin_spi_board_info));
#endif

	return 0;
}

arch_initcall(pr1_appliance_init);

void native_machine_restart(char *cmd)
{
	/* workaround reboot hang when booting from SPI */
	if ((bfin_read_SYSCR() & 0x7) == 0x3)
		bfin_reset_boot_spi_cs(GPIO_PF1);
}

void bfin_get_ether_addr(char *addr)
{
}
EXPORT_SYMBOL(bfin_get_ether_addr);
