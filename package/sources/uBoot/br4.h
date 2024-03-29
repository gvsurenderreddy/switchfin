/*
 * U-boot - Configuration file for SwitchVoice/Astfin BR4-Appliance Board board
 * 
 * mark@astfin.org
 * 
 * 27 Sep. 2009 Dimitar Penev (dpn@ucpbx.com) added automatic update of u-boot and linux
 * 		The idea is borrowed from Jeff Palmer (jpalmer@gsat.us)  
 */

#ifndef __CONFIG_BF537_H__
#define __CONFIG_BF537_H__

#define CONFIG_BR4		1
#define CONFIG_CMD_NET          1
#define CONFIG_IP_DEFRAG        1

// 64MB SDRAM with CAS latency 2
//#define SDRAM_64MB_FAST		1
//#define SDRAM_64MB_SLOW		1
//#define SDRAM_128MB		1

#define CFG_HUSH_PARSER		1
#define CFG_PROMPT_HUSH_PS2	"> "
#define CONFIG_RESET_TO_RETRY	1

#define CFG_LONGHELP		1
#define CONFIG_CMDLINE_EDITING	1
#define CONFIG_BAUDRATE		115200
/* Set default serial console for bf537 */
#define CONFIG_UART_CONSOLE	0
#define CONFIG_BF537		1

/* We need to disable the NOR flash */
#define CFG_NO_FLASH		1

/* time specified in ms */
#define CFG_TIME_POST           5000
#define CFG_TIME_SOFT_RESET  	10000
#define CFG_TIME_HARD_RESET  	15000

#define CONFIG_NET_RETRY_COUNT 3

/*
 * Boot Mode Set  
 * Blackfin can support several boot modes
 */
#define BF537_BYPASS_BOOT	0x0011  /* Bootmode 0: Execute from 16-bit externeal memory ( bypass BOOT ROM) 	*/
#define BF537_PARA_BOOT		0x0012  /* Bootmode 1: Boot from 8-bit or 16-bit flash 				*/
#define BF537_SPI_MASTER_BOOT	0x0014	/* Bootmode 3: SPI master mode boot from SPI flash			*/
#define BF537_SPI_SLAVE_BOOT	0x0015	/* Bootmode 4: SPI slave mode boot from SPI flash			*/
#define BF537_TWI_MASTER_BOOT	0x0016	/* Bootmode 5: TWI master mode boot from EEPROM				*/
#define BF537_TWI_SLAVE_BOOT	0x0017	/* Bootmode 6: TWI slave mode boot from EEPROM				*/
#define BF537_UART_BOOT		0x0018	/* Bootmode 7: UART slave mdoe boot via UART host			*/
/* Define the boot mode */
//#define BFIN_BOOT_MODE		BF537_BYPASS_BOOT
#define BFIN_BOOT_MODE		BF537_SPI_MASTER_BOOT
//#define BFIN_BOOT_MODE		BF537_UART_BOOT

#define CONFIG_PANIC_HANG 1

#define ADSP_BF534		0x34
#define ADSP_BF536		0x36
#define ADSP_BF537		0x37
#define BFIN_CPU		ADSP_BF537

/* This sets the default state of the cache on U-Boot's boot */
#define CONFIG_ICACHE_ON
#define CONFIG_DCACHE_ON

/* Define if want to do post memory test */
#define CONFIG_POST_TEST

/* Define where the uboot will be loaded by on-chip boot rom */
#define APP_ENTRY 0x00001000

#define CONFIG_RTC_BF533	1
#define CONFIG_BOOT_RETRY_TIME	-1	/* Enable this if bootretry required, currently its disabled */

/* CONFIG_CLKIN_HZ is any value in Hz                            */
#define CONFIG_CLKIN_HZ          25000000
/* CONFIG_CLKIN_HALF controls what is passed to PLL 0=CLKIN      */
/*                                                  1=CLKIN/2    */
#define CONFIG_CLKIN_HALF               0
/* CONFIG_PLL_BYPASS controls if the PLL is used 0=don't bypass  */
/*                                               1=bypass PLL    */
#define CONFIG_PLL_BYPASS               0
/* CONFIG_VCO_MULT controls what the multiplier of the PLL is.   */
/* Values can range from 1-64                                    */
/* Pawel, use 24 below to get 600MHz core speed */
//#define CONFIG_VCO_MULT			20
/* CONFIG_CCLK_DIV controls what the core clock divider is       */
/* Values can be 1, 2, 4, or 8 ONLY                              */
#define CONFIG_CCLK_DIV			1
/* CONFIG_SCLK_DIV controls what the peripheral clock divider is */
/* Values can range from 1-15                                    */
//#define CONFIG_SCLK_DIV			4	
/* CONFIG_SPI_BAUD controls the SPI peripheral clock divider     */
/* Values can range from 2-65535                                 */
/* SCK Frequency = SCLK / (2 * CONFIG_SPI_BAUD)                  */
#define CONFIG_SPI_BAUD                 2
#if (BFIN_BOOT_MODE == BF537_SPI_MASTER_BOOT)
#define CONFIG_SPI_BAUD_INITBLOCK						   4
#endif

#if ( CONFIG_CLKIN_HALF == 0 )
#define CONFIG_VCO_HZ           ( CONFIG_CLKIN_HZ * CONFIG_VCO_MULT )
#else
#define CONFIG_VCO_HZ           (( CONFIG_CLKIN_HZ * CONFIG_VCO_MULT ) / 2 )
#endif

#if (CONFIG_PLL_BYPASS == 0)
#define CONFIG_CCLK_HZ          ( CONFIG_VCO_HZ / CONFIG_CCLK_DIV )
#define CONFIG_SCLK_HZ          ( CONFIG_VCO_HZ / CONFIG_SCLK_DIV )
#else
#define CONFIG_CCLK_HZ          CONFIG_CLKIN_HZ
#define CONFIG_SCLK_HZ          CONFIG_CLKIN_HZ
#endif

#if (BFIN_BOOT_MODE == BF537_SPI_MASTER_BOOT)
#if (CONFIG_SCLK_HZ / (2*CONFIG_SPI_BAUD) > 20000000) 
#define CONFIG_SPI_FLASH_FAST_READ 1 /* Needed if SPI_CLK > 20 MHz */
#else
#undef CONFIG_SPI_FLASH_FAST_READ
#endif
#endif

#ifdef SDRAM_64MB_FAST
#define CONFIG_MEM_SIZE                 64             /* 128, 64, 32, 16 */
#define CONFIG_MEM_ADD_WDTH            	10             /* 8, 9, 10, 11    */
#define CONFIG_MEM_MT48LC32M8A2_7E	1
#elif SDRAM_64MB_SLOW
#define CONFIG_MEM_SIZE                 64             /* 128, 64, 32, 16 */
#define CONFIG_MEM_ADD_WDTH            	10             /* 8, 9, 10, 11    */
#define CONFIG_MEM_MT48LC32M8A2_75	1
#elif SDRAM_128MB
#define CONFIG_MEM_SIZE                 128             /* 128, 64, 32, 16 */
#define CONFIG_MEM_ADD_WDTH            	11             /* 8, 9, 10, 11    */
#define CONFIG_MEM_MT48LC64M8A2P_75	1
#endif

#define CONFIG_LOADS_ECHO	1

#define CFG_AUTOLOAD                    "no"    /*rarpb, bootp or dhcp commands will perform only a */
                                                /* configuration lookup from the BOOTP/DHCP server, */
                                                /* but not try to load any image using TFTP         */

/*
 * Network Settings
 */
/* network support */
#if (BFIN_CPU != ADSP_BF534)
#define CONFIG_IPADDR           192.168.1.100
#define CONFIG_NETMASK          255.255.255.0
#define CONFIG_GATEWAYIP        192.168.1.1
#define CONFIG_HOSTNAME         BR4
#define CONFIG_SERVERIP         192.168.1.2
#endif

#define CONFIG_ROOTPATH		/romfs
/* Uncomment next line to use fixed MAC address */
#define CONFIG_ETHADDR          02:80:ad:20:21:7a
 #define CONFIG_ENV_OVERWRITE	1 /* allow mac address to be set automatically */
#ifdef  CONFIG_ETHADDR
#define CONFIG_OVERWRITE_ETHADDR_ONCE 1 /* default MAC can be overwritten once  */
#endif
/* This is the routine that copies the MAC in Flash to the 'ethaddr' setting */

#define CFG_LONGHELP			1
#define CONFIG_BOOTDELAY		1	
#define CONFIG_BOOT_RETRY_TIME		-1	/* Enable this if bootretry required, currently its disabled */
#define CONFIG_BOOTCOMMAND 		"run flashboot"

#if (BFIN_BOOT_MODE == BF537_SPI_MASTER_BOOT) && defined(CONFIG_POST_TEST) 
#define I2C_ADDR_LIST  { 0x74}
/* POST support */
#define CONFIG_POST 		( CFG_POST_MEMORY | \
				  CFG_POST_ETHER  | \
				  CFG_POST_LED	  | \
				  CFG_POST_I2C)
#else 
#undef CONFIG_POST
#endif

#ifdef CONFIG_POST
#define CFG_CMD_POST_DIAG	CFG_CMD_DIAG
#define FLASH_START_POST_BLOCK 11       /* Should > = 11 */
#define FLASH_END_POST_BLOCK   71       /* Should < = 71 */
#else
#undef CFG_CMD_POST_DIAG
#endif

/* CF-CARD IDE-HDD Support */

//#define CONFIG_BFIN_TRUE_IDE    /* Add CF flash card support */
//#define CONFIG_BFIN_CF_IDE    /* Add CF flash card support */
//#define CONFIG_BFIN_HDD_IDE   /* Add IDE Disk Drive (HDD) support */


#if defined(CONFIG_BFIN_CF_IDE) || defined(CONFIG_BFIN_HDD_IDE) || defined(CONFIG_BFIN_TRUE_IDE) 
#  define CONFIG_BFIN_IDE	1
#  define ADD_IDE_CMD           CFG_CMD_IDE
#else
#  define ADD_IDE_CMD           0
#endif

#define CONFIG_BF537_NAND 	1		/* Add nand flash support */

#ifdef CONFIG_BF537_NAND
#  define ADD_NAND_CMD		CFG_CMD_NAND
#else
#  define ADD_NAND_CMD		0
#endif

#define CONFIG_SHOW_BOOT_PROGRESS 1
#define CONFIG_NETCONSOLE	1
#define CONFIG_NET_MULTI	1

#if (BFIN_CPU == ADSP_BF534)
#define CONFIG_BFIN_CMD		(CONFIG_CMD_DFL & ~CFG_CMD_NET) 
#else
#define CONFIG_BFIN_CMD		(CONFIG_CMD_DFL | CFG_CMD_PING)
#endif


#if ((BFIN_BOOT_MODE == BF537_BYPASS_BOOT) || (BFIN_BOOT_MODE == BF537_UART_BOOT))
#define CONFIG_COMMANDS			(CONFIG_BFIN_CMD| \
					 CFG_CMD_ELF	| \
					 CFG_CMD_I2C	| \
					 CFG_CMD_CACHE  | \
					 CFG_CMD_JFFS2	| \
					 CFG_CMD_EEPROM | \
					 CFG_CMD_DHCP   | \
					 ADD_IDE_CMD	| \
					 ADD_NAND_CMD	| \
					 CFG_CMD_POST_DIAG | \
					 CFG_CMD_DATE)
#elif (BFIN_BOOT_MODE == BF537_SPI_MASTER_BOOT)
#define CONFIG_COMMANDS			(CONFIG_BFIN_CMD| \
					 CFG_CMD_ELF	| \
					 CFG_CMD_I2C    | \
					 CFG_CMD_CACHE  | \
					 CFG_CMD_JFFS2	| \
					 CFG_CMD_DHCP   | \
					 CFG_CMD_EEPROM | \
					 ADD_NAND_CMD	| \
					 CFG_CMD_POST_DIAG | \
					 CFG_CMD_DIAG | \
					 CFG_CMD_DATE)
#endif

#define CONFIG_BOOTARGS "root=/dev/mtdblock0 rw console=ttyBF0,115200 factory_default=$(factory_default)"	
#define CONFIG_LOADADDR	0x2000000

#if ((BFIN_BOOT_MODE == BF537_BYPASS_BOOT) || (BFIN_BOOT_MODE == BF537_UART_BOOT))
#if (BFIN_CPU != ADSP_BF534)
#define CONFIG_EXTRA_ENV_SETTINGS                               \
        "ramargs=setenv bootargs root=/dev/mtdblock0 rw console=ttyBF0,115200 "      \
	"factory_default=$(factory_default) "			\
	"hard_default=$(hard_default)\0"			\
        "nfsargs=setenv bootargs root=/dev/nfs rw "             \
        "nfsroot=$(serverip):$(rootpath) console=ttyBF0,115200\0"                     \
        "addip=setenv bootargs $(bootargs) "                    \
        "ip=$(ipaddr):$(serverip):$(gatewayip):$(netmask)"      \
        ":$(hostname):eth0:off\0"                               \
        "ramboot=tftpboot $(loadaddr) uImage;"                     \
        "run ramargs;run addip;bootm $(loadaddr)\0"                       \
        "nfsboot=tftpboot $(loadaddr) uImage;"                     \
        "run nfsargs;run addip;bootelf\0"                       \
        "flashboot=bootm 0x20100000\0"                          \
        "update=tftpboot 0x2000000 u-boot.ldr;"                 \
        "eeprom write 0x2000000 0 $(filesize)\0"\
	"factory_default=0\0"                                   \
	"hard_default=0\0"					\
        ""
#else
#define CONFIG_EXTRA_ENV_SETTINGS                               \
        "ramargs=setenv bootargs root=/dev/mtdblock0 rw console=ttyBF0,115200\0" \
        "flashboot=bootm 0x20100000\0" \
        ""
#endif
#elif (BFIN_BOOT_MODE == BF537_SPI_MASTER_BOOT)
#if (BFIN_CPU != ADSP_BF534)
#define CONFIG_EXTRA_ENV_SETTINGS					\
    	"ramargs=setenv bootargs root=/dev/mtdblock0 rw console=ttyBF0,115200 "		\
	"factory_default=$factory_default "			\
	"hard_default=$hard_default\0"			\
	"offset=0\0"			\
    	"nfsargs=setenv bootargs root=/dev/nfs rw "			\
    	"nfsroot=$serverip:$rootpath console=ttyBF0,115200\0"				\
    	"addip=setenv bootargs $bootargs "				\
    	"ip=$ipaddr:$serverip:$gatewayip:$netmask"		\
    	":$hostname:eth0:off\0"					\
    	"ramboot=tftpboot $loadaddr uImage;"				\
    	"run ramargs;run addip;bootm $loadaddr\0"				\
    	"nfsboot=tftpboot $loadaddr uImage;"				\
    	"run nfsargs;run addip;bootm $loadaddr\0"				\
    	"flashboot=nand read.jffs2 $loadaddr 0x0 $image_size;"           \
    	"run ramargs;run addip;bootm $loadaddr\0"				\
    	"update=tftpboot $loadaddr u-boot.ldr;"				\
    	"eeprom write $loadaddr 0x0 $filesize;\0"			\
		"restore_hard=run backup_read;run clean_image;run clean_persistent;"		\
		"nand write $loadaddr $offset $image_size;reset\0"		\
		"image_size=0xA00000\0"					\
		"backupoffset=0xA00000\0"				\
		"persistent_offset=0x1400000\0"				\
		"restore_soft=run clean_persistent;reset\0"		\
		"backup_read=nand read.jffs2 $loadaddr $backupoffset $image_size\0"	\
		"clean_image=nand erase clean $offset $image_size;"	\
		"nand erase 0x0 $image_size\0"			\
		"clean_persistent=nand erase clean $persistent_offset;"	\
		"nand erase $persistent_offset\0"			\
		"factory_default=0\0"                                   \
		"hard_default=0\0"					\
    	""									\
        "memstart=0x2000000\0"                                  								\
        "memforcmp=0x2000010\0"                                 								\
        "uvfile=u-boot-br4.ver\0"                                								\
        "lvfile=uImage-br4.ver\0"                               								\
        "ufile=u-boot-br4.ldr\0"                                         							\
        "lfile=uImage-br4\0"                                        								\
        "uv=ubxxx\0"                                    									\
        "lv=uixxx\0"                                                                                 				\
        "upgrade=if setenv serveriptemp $serverip; setenv serverip 77.70.97.103; then run upgradeu; run upgradel; reset; fi\0"  \
        "upgradeu=if run downloaduv; then run checkuv; fi\0" 	 					                      	\
        "downloaduv=tftp $memstart $uvfile; \0"                                                                  		\
        "checkuv=envtomem uv  $memforcmp; "                                   							\
        	"memtoenv uvtemp $memstart $filesize; "										\
	        "if cmp.b $memstart $memforcmp $filesize; then "								\
			"echo u-boot ver. $uv is up to date; "									\
		"else "														\
			"echo Upgrading u-boot to version $uvtemp; run downloadu; fi\0"						\
        "downloadu=if tftp $memstart $ufile; then run flashu; else echo u-boot upgrade Failed!!!; fi\0"                        	\
        "flashu=eeprom write $memstart 0x0 $filesize; run setuv; \0"                                      			\
        "setuv=setenv uv $uvtemp; setenv serverip $serveriptemp; saveenv;\0"                                             	\
        ""															\
		"upgradel=if run downloadlv; then run checklv; fi\0"                                                                    \
        "downloadlv=tftp $memstart $lvfile; \0"                                                                                 \
        "checklv=envtomem lv  $memforcmp; "                                                       				\
                "memtoenv lvtemp $memstart $filesize; "                                                                      	\
                "if cmp.b $memstart $memforcmp $filesize; then "                                                             	\
                        "echo uImage ver. $lv is up to date; "                                                                  \
                "else "                                                                                                         \
                        "echo Upgrading uImage to version $lvtemp; run downloadl; fi\0"                                         \
        "downloadl=if tftp $memstart $lfile; then run flashl; else echo uImage upgrade Failed!!!; fi\0"                         \
        "flashl= nand erase clean; nand write.jffs2 $memstart 0x0 $filesize; run setlv; \0"                                     \
        "setlv=setenv lv $lvtemp; setenv serverip $serveriptemp; saveenv;\0"                                                    \

#else
#define CONFIG_EXTRA_ENV_SETTINGS                               \
        "ramargs=setenv bootargs root=/dev/mtdblock0 rw console=ttyBF0,115200\0" \
        "flashboot=bootm 0x20100000\0" \
        ""
#endif
#endif

/* this must be included AFTER the definition of CONFIG_COMMANDS (if any) */
#include <cmd_confdefs.h>

#if (BFIN_BOOT_MODE == BF537_SPI_MASTER_BOOT)
#if (BFIN_CPU == ADSP_BF534)
#define	CFG_PROMPT		"serial_bf534> "	/* Monitor Command Prompt */
#elif (BFIN_CPU == ADSP_BF536)
#define	CFG_PROMPT		"serial_bf536> "	/* Monitor Command Prompt */
#else
#define	CFG_PROMPT		"br4> "	/* Monitor Command Prompt */
#endif
#else
#if (BFIN_CPU == ADSP_BF534)
#define	CFG_PROMPT		"bf534> "	/* Monitor Command Prompt */
#elif (BFIN_CPU == ADSP_BF536)
#define	CFG_PROMPT		"bf536> "	/* Monitor Command Prompt */
#else
#define	CFG_PROMPT		"bf537> "	/* Monitor Command Prompt */
#endif
#endif

#if (CONFIG_COMMANDS & CFG_CMD_KGDB)
#define	CFG_CBSIZE		1024	/* Console I/O Buffer Size */
#else
#define	CFG_CBSIZE		512	/* Console I/O Buffer Size */
#endif
#define CFG_MAX_RAM_SIZE       	(CONFIG_MEM_SIZE * 1024*1024)
#define	CFG_PBSIZE		(CFG_CBSIZE+sizeof(CFG_PROMPT)+16)	/* Print Buffer Size */
#define	CFG_MAXARGS		32	/* max number of command args */
#define CFG_BARGSIZE		CFG_CBSIZE	/* Boot Argument Buffer Size */
#define CFG_MEMTEST_START	0x0	/* memtest works on */
#define CFG_MEMTEST_END		( (CONFIG_MEM_SIZE - 1) * 1024*1024)	/* 1 ... 63 MB in DRAM */
#define	CFG_LOAD_ADDR		CONFIG_LOADADDR	/* default load address */
#define	CFG_HZ			1000	/* decrementer freq: 10 ms ticks */
#define CFG_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }
#define	CFG_SDRAM_BASE		0x00000000

#define CFG_FLASH_BASE		0x20000000

#define	CFG_MONITOR_LEN		(256 << 10)	/* Reserve 256 kB for Monitor	*/
#define CFG_MONITOR_BASE	(CFG_MAX_RAM_SIZE - CFG_MONITOR_LEN)
#define	CFG_MALLOC_LEN		(128 << 10)	/* Reserve 128 kB for malloc()	*/
#define CFG_MALLOC_BASE		(CFG_MONITOR_BASE - CFG_MALLOC_LEN)
#define CFG_GBL_DATA_SIZE	0x4000
#define CFG_GBL_DATA_ADDR	(CFG_MALLOC_BASE - CFG_GBL_DATA_SIZE)
#define CONFIG_STACKBASE	(CFG_GBL_DATA_ADDR  - 4)

#define	CFG_BOOTMAPSZ		(8 << 20)	/* Initial Memory map for Linux */
#define CFG_MAX_FLASH_BANKS	1	/* max number of memory banks */
#define CFG_MAX_FLASH_SECT	71	/* max number of sectors on one chip */

#if (BFIN_BOOT_MODE == BF537_BYPASS_BOOT) || (BFIN_BOOT_MODE == BF537_UART_BOOT)    /* for bf537-stamp, usrt boot mode still store env in flash */
#define	CFG_ENV_IS_IN_FLASH	1
#define CFG_ENV_ADDR		0x20004000
#define CFG_ENV_OFFSET		(CFG_ENV_ADDR - CFG_FLASH_BASE)
#elif (BFIN_BOOT_MODE == BF537_SPI_MASTER_BOOT)
#define CFG_ENV_IS_IN_EEPROM           1
#define CFG_ENV_OFFSET                 0x4000
#define CFG_ENV_HEADER                 (CFG_ENV_OFFSET + 0x16e)        /* 0x12A is the length of LDR file header */
#endif
#define CFG_ENV_SIZE		0x2000
#define	CFG_ENV_SECT_SIZE	0x2000	/* Total Size of Environment Sector */
//#if (BFIN_BOOT_MODE == BF537_BYPASS_BOOT)
#define ENV_IS_EMBEDDED
//#endif

/* JFFS Partition offset set  */
#define CFG_JFFS2_FIRST_BANK 0
#define CFG_JFFS2_NUM_BANKS  1
/* 512k reserved for u-boot */
#define CFG_JFFS2_FIRST_SECTOR                 15

#define CONFIG_SPI

/*
 * Stack sizes
 */
#define CONFIG_STACKSIZE        (128*1024)      /* regular stack */

#define POLL_MODE		1
#define FLASH_TOT_SECT		71
#define FLASH_SIZE		0x400000
#define CFG_FLASH_SIZE		0x400000

/*
 * Board NAND Infomation
 */

#define CFG_NAND_ADDR          0x20000000
#define CFG_NAND_BASE          CFG_NAND_ADDR
#define CFG_MAX_NAND_DEVICE    1
#define SECTORSIZE             512
#define ADDR_COLUMN             1
#define ADDR_PAGE               2
#define ADDR_COLUMN_PAGE        3
#define NAND_ChipID_UNKNOWN    0x00
#define NAND_MAX_FLOORS        1
#define NAND_MAX_CHIPS         1
#define BFIN_NAND_READY		PF9

#define NAND_WAIT_READY(nand)  			\
	do { 					\
		int timeout = 0; 		\
		while(!(*pPORTFIO & PF3)) 	\
			if (timeout++ > 100000)	\
				break;		\
	} while (0)

#define BFIN_NAND_CLE          (1<<2)                  /* A2 -> Command Enable */
#define BFIN_NAND_ALE          (1<<1)                  /* A1 -> Address Enable */

#define WRITE_NAND_COMMAND(d, adr) do{ *(volatile __u8 *)((unsigned long)adr | BFIN_NAND_CLE) = (__u8)(d); } while(0)
#define WRITE_NAND_ADDRESS(d, adr) do{ *(volatile __u8 *)((unsigned long)adr | BFIN_NAND_ALE) = (__u8)(d); } while(0)
#define WRITE_NAND(d, adr) do{ *(volatile __u8 *)((unsigned long)adr) = (__u8)d; } while(0)
#define READ_NAND(adr) ((volatile unsigned char)(*(volatile __u8 *)(unsigned long)adr))

/*
 * Initialize PSD4256 registers for using I2C
 */
#define CONFIG_MISC_INIT_R

#define CFG_BOOTM_LEN			0x4000000       /* Large Image Length, set to 64 Meg */

/*
 * I2C settings
 * By default PF1 is used as SDA and PF0 as SCL on the Stamp board
 */
/*#define CONFIG_SOFT_I2C			1*/	/* I2C bit-banged		*/
#define CONFIG_HARD_I2C			1	/* I2C TWI */
#if defined CONFIG_HARD_I2C
#define CONFIG_TWICLK_KHZ		50
#endif

#if defined CONFIG_SOFT_I2C
/*
 * Software (bit-bang) I2C driver configuration
 */
#define PF_SCL				PF0
#define PF_SDA				PF1

#define I2C_INIT			(*pFIO_DIR |=  PF_SCL); asm("ssync;")
#define I2C_ACTIVE			(*pFIO_DIR |=  PF_SDA); *pFIO_INEN &= ~PF_SDA; asm("ssync;")
#define I2C_TRISTATE			(*pFIO_DIR &= ~PF_SDA); *pFIO_INEN |= PF_SDA; asm("ssync;")
#define I2C_READ			((volatile)(*pFIO_FLAG_D & PF_SDA) != 0); asm("ssync;")
#define I2C_SDA(bit)			if(bit) { \
							*pFIO_FLAG_S = PF_SDA; \
							asm("ssync;"); \
						} \
					else    { \
							*pFIO_FLAG_C = PF_SDA; \
							asm("ssync;"); \
						}
#define I2C_SCL(bit)			if(bit) { \
							*pFIO_FLAG_S = PF_SCL; \
							asm("ssync;"); \
						} \
					else    { \
							*pFIO_FLAG_C = PF_SCL; \
							asm("ssync;"); \
						}
#define I2C_DELAY			udelay(5)	/* 1/4 I2C clock duration */
#endif

#define CFG_I2C_SPEED			50000
#define CFG_I2C_SLAVE			0xFE

/* 0xFF, 0x7BB07BB0, 0x22547BB0 */
/* #define AMGCTLVAL            (AMBEN_P0 | AMBEN_P1 | AMBEN_P2 | AMCKEN)
#define AMBCTL0VAL              (B1WAT_7 | B1RAT_11 | B1HT_2 | B1ST_3 | B1TT_4 | ~B1RDYPOL |    \
                                ~B1RDYEN | B0WAT_7 | B0RAT_11 | B0HT_2 | B0ST_3 | B0TT_4 | ~B0RDYPOL | ~B0RDYEN)
#define AMBCTL1VAL              (B3WAT_2 | B3RAT_2 | B3HT_1 | B3ST_1 | B3TT_4 | B3RDYPOL | ~B3RDYEN |   \
                                B2WAT_7 | B2RAT_11 | B2HT_2 | B2ST_3 | B2TT_4 | ~B2RDYPOL | ~B2RDYEN)
*/

#define AMGCTLVAL               0xFF
#define AMBCTL0VAL              0x7BB07BB0
#define AMBCTL1VAL              0xFFC27BB0

#define CONFIG_VDSP		1

#ifdef CONFIG_VDSP
#define ET_EXEC_VDSP		0x8
#define SHT_STRTAB_VDSP		0x1
#define ELFSHDRSIZE_VDSP	0x2C
#define VDSP_ENTRY_ADDR		0xFFA00000
#endif

#if defined(CONFIG_BFIN_IDE)

#define CONFIG_DOS_PARTITION            1
/*
 * IDE/ATA stuff
 */
#undef  CONFIG_IDE_8xx_DIRECT           /* no pcmcia interface required */
#undef  CONFIG_IDE_LED                  /* no led for ide supported */
#undef  CONFIG_IDE_RESET                /* no reset for ide supported */

#define CFG_IDE_MAXBUS  1               /* max. 1 IDE busses */
#define CFG_IDE_MAXDEVICE               (CFG_IDE_MAXBUS*1) /* max. 1 drives per IDE bus */

#undef  AMBCTL1VAL
#define AMBCTL1VAL			0xFFC3FFC3

#define CONFIG_CF_ATASEL_DIS 	0x20311800
#define CONFIG_CF_ATASEL_ENA    0x20311802

#if defined(CONFIG_BFIN_TRUE_IDE)
// Note that these settings aren't for the most part used in include/ata.h
// when all of the ATA registers are setup
#define CFG_ATA_BASE_ADDR               0x2031C000
#define CFG_ATA_IDE0_OFFSET             0x0000
#define CFG_ATA_DATA_OFFSET             0x0020  /* Offset for data I/O */
#define CFG_ATA_REG_OFFSET              0x0020  /* Offset for normal register accesses */
#define CFG_ATA_ALT_OFFSET              0x001C  /* Offset for alternate registers */
#define CFG_ATA_STRIDE                  2 	/* CF.A0 --> Blackfin.Ax */
#endif /* CONFIG_BFIN_TRUE_IDE */

#if defined(CONFIG_BFIN_CF_IDE) /* USE CompactFlash Storage Card in the common memory space */
#define CFG_ATA_BASE_ADDR               0x20211800
#define CFG_ATA_IDE0_OFFSET             0x0000
#define CFG_ATA_DATA_OFFSET             0x0000  /* Offset for data I/O */
#define CFG_ATA_REG_OFFSET              0x0000  /* Offset for normal register accesses */
#define CFG_ATA_ALT_OFFSET              0x000E  /* Offset for alternate registers */
#define CFG_ATA_STRIDE                  1 	/* CF.A0 --> Blackfin.Ax */
#endif /* CONFIG_BFIN_CF_IDE */
 
#if defined(CONFIG_BFIN_HDD_IDE) /* USE TRUE IDE */
#define CFG_ATA_BASE_ADDR               0x20314000
#define CFG_ATA_IDE0_OFFSET             0x0000
#define CFG_ATA_DATA_OFFSET             0x0020  /* Offset for data I/O */
#define CFG_ATA_REG_OFFSET              0x0020  /* Offset for normal register accesses */
#define CFG_ATA_ALT_OFFSET              0x001C  /* Offset for alternate registers */
#define CFG_ATA_STRIDE                  2 	/* CF.A0 --> Blackfin.A1 */

#undef  CONFIG_SCLK_DIV
#define CONFIG_SCLK_DIV                 8
#endif /* CONFIG_BFIN_HDD_IDE */

#endif /*CONFIG_BFIN_IDE*/

#endif
