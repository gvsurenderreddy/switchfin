/*
 * File:         arch/blackfin/mach-bf533/ip0x.c
 * Based on:     arch/blackfin/mach-bf533/bf1.c
 * Based on:     arch/blackfin/mach-bf533/stamp.c
 * Author:       Ivan Danov <idanov@gmail.com>
 *               Modified for FX0X Edgepbx
 *
 * Created:      2008
 * Description:  Board info file for the FX08 boards, which
 *               are derived from the BlackfinOne V2.0 boards.
 *
 * Modified:
 *               COpyright 2007 David Rowe     
 *               Copyright 2006 Intratrade Ltd.
 *               Copyright 2005 National ICT Australia (NICTA)
 *               Copyright 2004-2006 Analog Devices Inc.
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
#if defined(CONFIG_USB_ISP1362_HCD) || defined(CONFIG_USB_ISP1362_HCD_MODULE)
#include <linux/usb/isp1362.h>
#endif
#include <linux/pata_platform.h>
#include <linux/irq.h>
#include <asm/dma.h>
#include <asm/bfin5xx_spi.h>
#include <asm/reboot.h>
#include <asm/portmux.h>


/*
 * Name the Board for the /proc/cpuinfo
 */
#if defined (CONFIG_BFIN532_FX0X)
const char bfin_board_name[] = "FX08";
#else
#error Unknown board
#endif

#if defined(CONFIG_RTC_DRV_BFIN) || defined(CONFIG_RTC_DRV_BFIN_MODULE)
static struct platform_device rtc_device = {
        .name = "rtc-bfin",
        .id   = -1,
};
#endif

/*
 *  Driver needs to know address, irq and flag pin.
 */
#if defined (CONFIG_BFIN532_FX0X)
#if defined(CONFIG_DM9000) || defined(CONFIG_DM9000_MODULE)

#include <linux/dm9000.h>

static struct resource dm9000_resource1[] = {
	[0] = {
		.start = CONFIG_FX0X_NET1,
		.end   = CONFIG_FX0X_NET1 + 1,
		.flags = IORESOURCE_MEM
	},
	[1] = {
		.start = CONFIG_FX0X_NET1 + 2,
		.end   = CONFIG_FX0X_NET1 + 3,
		.flags = IORESOURCE_MEM
	},
	[2] = {
		.start = IRQ_PF15,
		.end   = IRQ_PF15,
		.flags = IORESOURCE_IRQ | IORESOURCE_IRQ_HIGHEDGE
	}
};

static struct resource dm9000_resource2[] = {
       [0] = {
               .start = CONFIG_FX0X_NET2,
               .end   = CONFIG_FX0X_NET2 + 1,
               .flags = IORESOURCE_MEM
       },
       [1] = {
               .start = CONFIG_FX0X_NET2 + 2,
               .end   = CONFIG_FX0X_NET2 + 3,
               .flags = IORESOURCE_MEM
       },
       [2] = {
               .start = IRQ_PF14,
               .end   = IRQ_PF14,
               .flags = IORESOURCE_IRQ | IORESOURCE_IRQ_HIGHEDGE
       }
};

/*
* for the moment we limit ourselves to 16bit IO until some
* better IO routines can be written and tested
*/
static struct dm9000_plat_data dm9000_platdata1 = {
        .flags          = DM9000_PLATF_16BITONLY,
};

static struct platform_device dm9000_device1 = {
	.name           = "dm9000",
	.id             = 0,
	.num_resources  = ARRAY_SIZE(dm9000_resource1),
	.resource       = dm9000_resource1,
	.dev            = {
	.platform_data = &dm9000_platdata1,
	}
};

static struct dm9000_plat_data dm9000_platdata2 = {
        .flags          = DM9000_PLATF_16BITONLY,
};

static struct platform_device dm9000_device2 = {
	.name           = "dm9000",
	.id             = 1,
	.num_resources  = ARRAY_SIZE(dm9000_resource2),
	.resource       = dm9000_resource2,
	.dev            = {
	.platform_data = &dm9000_platdata2,
	}
};

#endif
#endif // #if defined (CONFIG_BF532_FX0X)

#if defined(CONFIG_SPI_BFIN) || defined(CONFIG_SPI_BFIN_MODULE)
/* all SPI peripherals info goes here */

#if defined(CONFIG_SPI_MMC) || defined(CONFIG_SPI_MMC_MODULE)
static struct bfin5xx_spi_chip spi_mmc_chip_info = {
//CPOL (Clock Polarity)
// 0 - Active high SCK
// 1 - Active low SCK
// CPHA (Clock Phase) Selects transfer format and operation mode
// 0 - SCLK toggles from middle of the first data bit, slave select
//     pins controlled by hardware.
// 1 - SCLK toggles from beginning of first data bit, slave select
//     pins controller by user software.
//	.ctl_reg = 0x1c00,		// CPOL=1,CPHA=1,Sandisk 1G work
//NO NO	.ctl_reg = 0x1800,		// CPOL=1,CPHA=0
//NO NO	.ctl_reg = 0x1400,		// CPOL=0,CPHA=1
	//.ctl_reg = 0x1000,		// CPOL=0,CPHA=0,Sandisk 1G work
        //.enable_dma = 0,		// if 1 - block!!!
        //.bits_per_word = 8,
	//.cs_change_per_word = 0,
        .enable_dma = 1,
        .bits_per_word = 8,
};
#endif

/* Notice: for blackfin, the speed_hz is the value of register
 * SPI_BAUD, not the real baudrate */
static struct spi_board_info bfin_spi_board_info[] __initdata = {
#if defined(CONFIG_SPI_MMC) || defined(CONFIG_SPI_MMC_MODULE)
        {
                .modalias = "spi_mmc_dummy",
                .max_speed_hz = 20000000,     /* max spi clock (SCK) speed in HZ */
                .bus_num = 0,
                .chip_select = 0,
                .platform_data = NULL,
                .controller_data = &spi_mmc_chip_info,
                .mode = SPI_MODE_3,
        },
	{
                .modalias = "spi_mmc",
                .max_speed_hz = 20000000,     /* max spi clock (SCK) speed in HZ */
                .bus_num = 0,
                .chip_select = CONFIG_SPI_MMC_CS_CHAN,
                .platform_data = NULL,
                .controller_data = &spi_mmc_chip_info,
                .mode = SPI_MODE_3,

	},
#endif
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
        }
};

/* SPI controller data */
static struct bfin5xx_spi_master bfin_spi0_info = {
        .num_chipselect = 8,
        .enable_dma = 1,  /* master has the ability to do dma transfer */
        .pin_req = {P_SPI0_SCK, P_SPI0_MISO, P_SPI0_MOSI, 0},
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
	{
		.start = 0xFFC00400,
		.end = 0xFFC004FF,
		.flags = IORESOURCE_MEM,
	},
};

static struct platform_device bfin_uart_device = {
	.name = "bfin-uart",
	.id = 1,
	.num_resources = ARRAY_SIZE(bfin_uart_resources),
	.resource = bfin_uart_resources,
};
#endif

#if defined(CONFIG_USB_ISP1362_HCD) || defined(CONFIG_USB_ISP1362_HCD_MODULE)
static struct resource isp1362_hcd_resources[] = {
	{
		.start = CONFIG_FX0X_USB,
		.end   = CONFIG_FX0X_USB + 1,
		.flags = IORESOURCE_MEM,
	},{
		.start = CONFIG_FX0X_USB + 2,
		.end   = CONFIG_FX0X_USB + 3,
		.flags = IORESOURCE_MEM,
	},{
		.start = IRQ_PF11,
		.end   = IRQ_PF11,
		.flags = IORESOURCE_IRQ | IORESOURCE_IRQ_HIGHLEVEL,
	},
};

static struct isp1362_platform_data isp1362_priv = {
	.sel15Kres = 1,
	.clknotstop = 0,
	.oc_enable = 0,		// external OC
	.int_act_high = 0,
	.int_edge_triggered = 0,
	.remote_wakeup_connected = 0,
	.no_power_switching = 1,
	.power_switching_mode = 0,
};

static struct platform_device isp1362_hcd_device = {
	.name = "isp1362-hcd",
	.id = 0,
	.dev = {
		.platform_data = &isp1362_priv,
	},
	.num_resources = ARRAY_SIZE(isp1362_hcd_resources),
	.resource = isp1362_hcd_resources,
};
#endif


static struct platform_device *ip0x_devices[] __initdata = {
#if defined (CONFIG_BFIN532_FX0X)
#if defined(CONFIG_DM9000) || defined(CONFIG_DM9000_MODULE)
	&dm9000_device1,
	&dm9000_device2,
#endif
#endif

#if defined(CONFIG_SPI_BFIN) || defined(CONFIG_SPI_BFIN_MODULE)
	&bfin_spi0_device,
#endif

#if defined(CONFIG_SERIAL_BFIN) || defined(CONFIG_SERIAL_BFIN_MODULE)
	&bfin_uart_device,
#endif
#if defined(CONFIG_USB_ISP1362_HCD) || defined(CONFIG_USB_ISP1362_HCD_MODULE)
	&isp1362_hcd_device,
#endif
};

static int __init ip0x_init(void)
{
	printk(KERN_INFO "%s(): registering device resources\n", __FUNCTION__);
	platform_add_devices(ip0x_devices, ARRAY_SIZE(ip0x_devices));
#if defined(CONFIG_SPI_BFIN) || defined(CONFIG_SPI_BFIN_MODULE)
        spi_register_board_info(bfin_spi_board_info, ARRAY_SIZE(bfin_spi_board_info));
#endif
	return 0;
}

arch_initcall(ip0x_init);

void native_machine_restart(char *cmd)
{
}
