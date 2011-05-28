/*
 * File:         arch/blackfin/mach-bf533/ip0x.c
 * Based on:     arch/blackfin/mach-bf533/bf1.c
 * Based on:     arch/blackfin/mach-bf533/stamp.c
 * Author:       Ivan Danov <idanov@gmail.com>
 *               Modified for IP0X David Rowe
 *
 * Created:      2007
 * Description:  Board info file for the IP01 boards, which
 *               are derived from the BlackfinOne V2.0 boards.
 *
 * Modified:
 *               COpyright 2007 David Rowe     
 *               Copyright 2006 Intratrade Ltd.
 *               Copyright 2005 National ICT Australia (NICTA)
 *               Copyright 2004-2006 Analog Devices Inc.
 *
 *               May 2011, updated for uClinux 2010R1-RC5
 *               COpyright 2011 Switchfin.org   
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
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>
#include <linux/spi/spi.h>
#include <linux/spi/flash.h>
#if defined(CONFIG_USB_ISP1362_HCD) || defined(CONFIG_USB_ISP1362_HCD_MODULE)
#include <linux/usb/isp1362.h>
#endif
#include <asm/irq.h>
#include <asm/bfin5xx_spi.h>
#include <asm/dma.h>
#include <asm/reboot.h>
#include <asm/portmux.h>

#include <asm-generic/io.h>

/*
 * Name the Board for the /proc/cpuinfo
 */
#if defined (CONFIG_BFIN532_IP0X)
const char bfin_board_name[] = "IP01";
#else
#error Unknown board
#endif

#if defined(CONFIG_DM9000) || defined(CONFIG_DM9000_MODULE)
/* All dm9000 stuff info goes here  --------------------------------------------------------------------- */
#include <linux/dm9000.h>

static struct resource dm9000_resource1[] = {
	{
		.start = CONFIG_IP0X_NET1,
		.end   = CONFIG_IP0X_NET1 + 1,
		.flags = IORESOURCE_MEM
	},
	{
		.start = CONFIG_IP0X_NET1 + 2,
		.end   = CONFIG_IP0X_NET1 + 3,
		.flags = IORESOURCE_MEM
	},
	{
		.start = IRQ_PF15,
		.end   = IRQ_PF15,
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

#endif


#if defined(CONFIG_SPI_BFIN) || defined(CONFIG_SPI_BFIN_MODULE)
/* All SPI peripherals info goes here  ------------------------------------------------------------------ */

#if defined(CONFIG_MTD_M25P80) || defined(CONFIG_MTD_M25P80_MODULE)
static struct mtd_partition bfin_spi_flash_partitions[] = {
        {
                .name = "boot loader(spi)",
                .size = 0x00020000,
                .offset = 0,
                .mask_flags = MTD_CAP_ROM
        },{
                .name = "linux kernel(spi)",
                .size = 0xe0000,
                .offset = 0x20000
        },{
                .name = "file system(spi)",
                .size = 0x700000,
                .offset = 0x00100000,
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


/* Notice: for blackfin, the speed_hz is the value of register
 * SPI_BAUD, not the real baudrate */
static struct spi_board_info bfin_spi_board_info[] __initdata = {
#if defined(CONFIG_MTD_M25P80) || defined(CONFIG_MTD_M25P80_MODULE)
        {
                /* the modalias must be the same as spi device driver name */
                .modalias = "m25p80", /* Name of spi_driver for this device */
                .max_speed_hz = 25000000,     /* max spi clock (SCK) speed in HZ */
                .bus_num = 1, /* Framework bus number */
                .chip_select = 2, /* Framework chip select. On STAMP537 it is SPISSEL1*/
                .platform_data = &bfin_spi_flash_data,
                .controller_data = &spi_flash_chip_info,
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
                .flags = IORESOURCE_DMA,
        },
        [2] = {
                .start = IRQ_SPI,
                .end   = IRQ_SPI,
                .flags = IORESOURCE_IRQ,
        },
};

/* MASTER INFO */
static struct bfin5xx_spi_master bfin_spi0_info = {
        .num_chipselect = 8,
        .enable_dma = 1,  /* master has the ability to do dma transfer */
        .pin_req = {P_SPI0_SCK, P_SPI0_MISO, P_SPI0_MOSI, 0},
};
/* MASTER DEVICE */
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
/* All UART stuff goes here -------------------------------------------------------------------------------- */

#ifdef CONFIG_SERIAL_BFIN_UART0
static struct resource bfin_uart0_resources[] = {
        {
                .start = BFIN_UART_THR,
                .end = BFIN_UART_GCTL+2,
                .flags = IORESOURCE_MEM,
        },
        {
                .start = IRQ_UART0_RX,
                .end = IRQ_UART0_RX + 1,
                .flags = IORESOURCE_IRQ,
        },
        {
                .start = IRQ_UART0_ERROR,
                .end = IRQ_UART0_ERROR,
                .flags = IORESOURCE_IRQ,
        },
        {
                .start = CH_UART0_TX,
                .end = CH_UART0_TX,
                .flags = IORESOURCE_DMA,
        },
        {
                .start = CH_UART0_RX,
                .end = CH_UART0_RX,
                .flags = IORESOURCE_DMA,
        },
};

unsigned short bfin_uart0_peripherals[] = {
        P_UART0_TX, P_UART0_RX, 0
};

static struct platform_device bfin_uart0_device = {
        .name = "bfin-uart",
        .id = 0,
        .num_resources = ARRAY_SIZE(bfin_uart0_resources),
        .resource = bfin_uart0_resources,
        .dev = {
                .platform_data = &bfin_uart0_peripherals, /* Passed to driver */
        },
};
#endif
#endif

#if defined(CONFIG_USB_ISP1362_HCD) || defined(CONFIG_USB_ISP1362_HCD_MODULE)
/* All USB stuff goes here -------------------------------------------------------------------------------- */
static struct resource isp1362_hcd_resources[] = {
	{
		.start = CONFIG_IP0X_USB,
		.end   = CONFIG_IP0X_USB + 1,
		.flags = IORESOURCE_MEM,
	},{
		.start = CONFIG_IP0X_USB + 2,
		.end   = CONFIG_IP0X_USB + 3,
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


#if defined(CONFIG_MTD_NAND_PLATFORM) || defined(CONFIG_MTD_NAND_PLATFORM_MODULE)
/* All NAND  stuff goes here -------------------------------------------------------------------------------- */

#ifdef CONFIG_MTD_PARTITIONS
const char *part_probes[] = { "cmdlinepart", "RedBoot", NULL };

static struct mtd_partition bfin_plat_nand_partitions[] = {
        {
                .name       = "linux kernel(nand)",
                .size       = 0xA00000,
                .offset     = 0,
        }, {
                .name       = "linux kernel backup(nand)",
                .size       = 0xA00000,
                .offset     = 0xA00000,
        }, {
                .name       = "persistent file system(nand)",
                .size       = (CONFIG_BFIN_NAND_PLAT_SIZE-0x1400000),
                .offset     = 0x1400000,
        }
};
#endif


static void bfin_plat_nand_cmd_ctrl(struct mtd_info *mtd, int cmd, unsigned int ctrl)
{
        struct nand_chip *this = mtd->priv;

        if (cmd == NAND_CMD_NONE)
                return;

        if (ctrl & NAND_CLE)
                writeb(cmd, this->IO_ADDR_W + (1 << CONFIG_BFIN_NAND_PLAT_CLE));
        else
                writeb(cmd, this->IO_ADDR_W + (1 << CONFIG_BFIN_NAND_PLAT_ALE));
}

static int bfin_plat_nand_dev_ready(struct mtd_info *mtd)
{
        return gpio_get_value(CONFIG_BFIN_NAND_PLAT_READY);
}


static struct platform_nand_data bfin_plat_nand_data = {
        .chip = {
                .nr_chips = 1,
                .chip_delay = 30,
#ifdef CONFIG_MTD_PARTITIONS
                .part_probe_types = part_probes,
                .partitions = bfin_plat_nand_partitions,
                .nr_partitions = ARRAY_SIZE(bfin_plat_nand_partitions),
#endif
        },
        .ctrl = {
                .cmd_ctrl  = bfin_plat_nand_cmd_ctrl,
                .dev_ready = bfin_plat_nand_dev_ready,
        },
};

#define MAX(x, y) (x > y ? x : y)
static struct resource bfin_plat_nand_resources = {
        .start = 0x20000000,
        .end   = 0x20000000 + (1 << MAX(CONFIG_BFIN_NAND_PLAT_CLE, CONFIG_BFIN_NAND_PLAT_ALE)),
        .flags = IORESOURCE_MEM,
};

static struct platform_device bfin_async_nand_device = {
        .name = "gen_nand",
        .id = -1,
        .num_resources = 1,
        .resource = &bfin_plat_nand_resources,
        .dev = {
                .platform_data = &bfin_plat_nand_data,
        },
};

static void bfin_plat_nand_init(void)
{
        if (gpio_request(CONFIG_BFIN_NAND_PLAT_READY, "bfin_nand_plat"))
                printk(KERN_ERR"Requesting NAND Ready GPIO %d failed\n",CONFIG_BFIN_NAND_PLAT_READY);
        else
                printk(KERN_ERR"gpio_request OK\n");
}
#else
static void bfin_plat_nand_init(void) {}
#endif

/* Platform  devices ----------------------------------------------------------------------------------------- */
static struct platform_device *ip0x_devices[] __initdata = {
#if defined (CONFIG_BFIN532_IP0X)
#if defined(CONFIG_DM9000) || defined(CONFIG_DM9000_MODULE)
	&dm9000_device1,
#endif
#endif
#if defined(CONFIG_SPI_BFIN) || defined(CONFIG_SPI_BFIN_MODULE)
	&bfin_spi0_device,
#endif
#if defined(CONFIG_SERIAL_BFIN) || defined(CONFIG_SERIAL_BFIN_MODULE)
	&bfin_uart0_device,
#endif
#if defined(CONFIG_USB_ISP1362_HCD) || defined(CONFIG_USB_ISP1362_HCD_MODULE)
	&isp1362_hcd_device,
#endif
#if defined(CONFIG_MTD_NAND_PLATFORM) || defined(CONFIG_MTD_NAND_PLATFORM_MODULE)
        &bfin_async_nand_device,
#endif  
};

/* Platform  early devices ----------------------------------------------------------------------------------- */
static struct platform_device *ip0x_early_devices[] __initdata = {
#if defined(CONFIG_SERIAL_BFIN_CONSOLE) || defined(CONFIG_EARLY_PRINTK)
#ifdef CONFIG_SERIAL_BFIN_UART0
        &bfin_uart0_device,
#endif
#endif
};

void __init native_machine_early_platform_add_devices(void)
{
        printk(KERN_INFO "register early platform devices\n");
        early_platform_add_devices(ip0x_early_devices,
                ARRAY_SIZE(ip0x_early_devices));
}



/* Board Init  function --------------------------------------------------------------------------------------- */
static int __init ip0x_init(void)
{
        u_int i, j;

        printk(KERN_INFO "%s(): chip_id=%08lX,dspid=%08X\n", __FUNCTION__, *((volatile unsigned long *)CHIPID), bfin_read_DSPID());

	printk(KERN_INFO "%s(): registering device resources\n", __FUNCTION__);
	bfin_plat_nand_init();
	platform_add_devices(ip0x_devices, ARRAY_SIZE(ip0x_devices));
#if defined(CONFIG_SPI_BFIN) || defined(CONFIG_SPI_BFIN_MODULE)
        for (i = 0; i < ARRAY_SIZE(bfin_spi_board_info); i ++) {
               j = 1 << bfin_spi_board_info [i]. chip_select;
               // set spi cs to 1
               bfin_write_FIO_DIR (bfin_read_FIO_DIR() | j);
               bfin_write_FIO_FLAG_S (j);
        }
	spi_register_board_info(bfin_spi_board_info, ARRAY_SIZE(bfin_spi_board_info));
#endif
	return 0;
}

arch_initcall(ip0x_init);

