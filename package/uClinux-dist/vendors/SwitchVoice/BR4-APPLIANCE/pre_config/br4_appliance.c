/*
 * File:         arch/blackfin/mach-bf537/boards/br4_appliance.c
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
 *               May 2011, updated for uClinux 2010R1-RC5
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
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>
#include <linux/spi/spi.h>
#include <linux/spi/flash.h>
#include <linux/spi/mmc_spi.h>
#include <asm/irq.h>
#include <asm/bfin5xx_spi.h>
#include <asm/dma.h>
#include <asm/reboot.h>
#include <asm/portmux.h>
#include <linux/i2c.h>
#include <asm-generic/io.h>

const char bfin_board_name[] = "BR4 APPLIANCE";

/* All RTC info goes here  ---------------------------------------------------------------------------------- */
#if defined(CONFIG_RTC_DRV_BFIN) || defined(CONFIG_RTC_DRV_BFIN_MODULE)
static struct platform_device rtc_device = {
	.name = "rtc-bfin",
	.id   = -1,
};
#endif

/* All MAC info goes here  ---------------------------------------------------------------------------------- */
#if defined(CONFIG_BFIN_MAC) || defined(CONFIG_BFIN_MAC_MODULE)
#include <linux/bfin_mac.h>
static const unsigned short bfin_mac_peripherals[] = P_MII0;

static struct bfin_phydev_platform_data bfin_phydev_data[] = {
        {
                .addr = 1,
                .irq = PHY_POLL, /* IRQ_MAC_PHYINT */
        },
};

static struct bfin_mii_bus_platform_data bfin_mii_bus_data = {
        .phydev_number = 1,
        .phydev_data = bfin_phydev_data,
        .phy_mode = PHY_INTERFACE_MODE_MII,
        .mac_peripherals = bfin_mac_peripherals,
};

static struct platform_device bfin_mii_bus = {
        .name = "bfin_mii_bus",
        .dev = {
                .platform_data = &bfin_mii_bus_data,
        }
};

static struct platform_device bfin_mac_device = {
        .name = "bfin_mac",
        .dev = {
                .platform_data = &bfin_mii_bus,
        }
};
#endif

#if defined(CONFIG_SPI_BFIN) || defined(CONFIG_SPI_BFIN_MODULE)
/* All SPI peripherals info goes here  ---------------------------------------------------------------------- */

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

#if defined(CONFIG_SPI_MMC) || defined(CONFIG_SPI_MMC_MODULE)
static struct bfin5xx_spi_chip spi_mmc_chip_info = {
	.enable_dma = 1,
	.bits_per_word = 8,
};
#endif

static struct spi_board_info bfin_spi_board_info[] __initdata = {
#if defined(CONFIG_MTD_M25P80) || defined(CONFIG_MTD_M25P80_MODULE)
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

/* All UART  stuff goes here -------------------------------------------------------------------------------- */
#if defined(CONFIG_SERIAL_BFIN) || defined(CONFIG_SERIAL_BFIN_MODULE)
#ifdef CONFIG_SERIAL_BFIN_UART0
static struct resource bfin_uart0_resources[] = {
        {
                .start = UART0_THR,
                .end = UART0_GCTL+2,
                .flags = IORESOURCE_MEM,
        },
        {
                .start = IRQ_UART0_RX,
                .end = IRQ_UART0_RX+1,
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
#endif
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

/* All I2C  stuff goes here -------------------------------------------------------------------------------- */
#if defined(CONFIG_I2C_BLACKFIN_TWI) || defined(CONFIG_I2C_BLACKFIN_TWI_MODULE)
static struct resource bfin_twi0_resource[] = {
	[0] = {
		.start = TWI0_REGBASE,
		.end   = TWI0_REGBASE,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = IRQ_TWI,
		.end   = IRQ_TWI,
		.flags = IORESOURCE_IRQ,
	},
};

static struct platform_device i2c_bfin_twi_device = {
	.name = "i2c-bfin-twi",
	.id = 0,
	.num_resources = ARRAY_SIZE(bfin_twi0_resource),
	.resource = bfin_twi0_resource,
};
#endif

#if defined(CONFIG_I2C) || defined(CONFIG_SENSORS_PCA9539)
static struct i2c_board_info __initdata br4_i2c_device[] = { 
	{ 
		.type = "pca9539", 
		.addr = 0x74, 		
	}, 
};
#endif

#if defined(CONFIG_MTD_NAND_PLATFORM) || defined(CONFIG_MTD_NAND_PLATFORM_MODULE)
/* All NAND  stuff goes here -------------------------------------------------------------------------------- */

#ifdef CONFIG_MTD_PARTITIONS
const char *part_probes[] = { NULL };

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

/* The nand driver in uClinux 2010R1-Rc5 seems to have timing issue, use software ready check
static int bfin_plat_nand_dev_ready(struct mtd_info *mtd)
{
        return gpio_get_value(CONFIG_BFIN_NAND_PLAT_READY);
}
*/

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
//		.dev_ready = bfin_plat_nand_dev_ready,   // The nand driver in uClinux 2010R1-Rc5 
							 // seems to have timing issue, use software ready check
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
static struct platform_device *br4_appliance_devices[] __initdata = {

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
	&bfin_uart0_device,
#endif

#if defined(CONFIG_I2C_BLACKFIN_TWI) || defined(CONFIG_I2C_BLACKFIN_TWI_MODULE)
	&i2c_bfin_twi_device,
#endif

#if defined(CONFIG_SERIAL_BFIN_SPORT) || defined(CONFIG_SERIAL_BFIN_SPORT_MODULE)
	&bfin_sport0_uart_device,
	&bfin_sport1_uart_device,
#endif
#if defined(CONFIG_MTD_NAND_PLATFORM) || defined(CONFIG_MTD_NAND_PLATFORM_MODULE)
        &bfin_async_nand_device,
#endif
};

/* Board Init  function --------------------------------------------------------------------------------------- */
static int __init br4_appliance_init(void)
{
	printk(KERN_INFO "%s(): chip_id=%08lX,dspid=%08X\n",
                __FUNCTION__,
                *((volatile unsigned long *)CHIPID),
                bfin_read_DSPID());

	printk(KERN_INFO "%s(): registering device resources\n", __FUNCTION__);
	bfin_plat_nand_init();
	platform_add_devices(br4_appliance_devices, ARRAY_SIZE(br4_appliance_devices));
#if defined(CONFIG_SPI_BFIN) || defined(CONFIG_SPI_BFIN_MODULE)
	spi_register_board_info(bfin_spi_board_info, ARRAY_SIZE(bfin_spi_board_info));
#endif

#if defined(CONFIG_I2C) || defined(CONFIG_SENSORS_PCA9539)
	i2c_register_board_info(0, br4_i2c_device, ARRAY_SIZE(br4_i2c_device));
#endif

	return 0;
}

arch_initcall(br4_appliance_init);

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
