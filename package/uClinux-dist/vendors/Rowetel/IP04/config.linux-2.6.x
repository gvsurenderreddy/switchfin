#
# Automatically generated make config: don't edit
# Linux kernel version: 2.6.22.18
# Sun Mar 30 21:37:06 2008
#
# CONFIG_MMU is not set
# CONFIG_FPU is not set
CONFIG_RWSEM_GENERIC_SPINLOCK=y
# CONFIG_RWSEM_XCHGADD_ALGORITHM is not set
CONFIG_BLACKFIN=y
CONFIG_ZONE_DMA=y
CONFIG_SEMAPHORE_SLEEPERS=y
CONFIG_GENERIC_FIND_NEXT_BIT=y
CONFIG_GENERIC_HWEIGHT=y
CONFIG_GENERIC_HARDIRQS=y
CONFIG_GENERIC_IRQ_PROBE=y
# CONFIG_GENERIC_TIME is not set
CONFIG_GENERIC_GPIO=y
CONFIG_FORCE_MAX_ZONEORDER=14
CONFIG_GENERIC_CALIBRATE_DELAY=y
CONFIG_DEFCONFIG_LIST="/lib/modules/$UNAME_RELEASE/.config"

#
# Code maturity level options
#
CONFIG_EXPERIMENTAL=y
CONFIG_BROKEN_ON_SMP=y
CONFIG_INIT_ENV_ARG_LIMIT=32

#
# General setup
#
CONFIG_LOCALVERSION="switchfin"
CONFIG_LOCALVERSION_AUTO=y
CONFIG_SYSVIPC=y
# CONFIG_IPC_NS is not set
CONFIG_SYSVIPC_SYSCTL=y
# CONFIG_POSIX_MQUEUE is not set
# CONFIG_BSD_PROCESS_ACCT is not set
# CONFIG_TASKSTATS is not set
# CONFIG_UTS_NS is not set
# CONFIG_AUDIT is not set
# CONFIG_IKCONFIG is not set
CONFIG_LOG_BUF_SHIFT=14
CONFIG_SYSFS_DEPRECATED=y
# CONFIG_RELAY is not set
CONFIG_BLK_DEV_INITRD=y
CONFIG_INITRAMFS_SOURCE=""
# CONFIG_CC_OPTIMIZE_FOR_SIZE is not set
CONFIG_SYSCTL=y
CONFIG_EMBEDDED=y
CONFIG_UID16=y
CONFIG_SYSCTL_SYSCALL=y
CONFIG_KALLSYMS=y
# CONFIG_KALLSYMS_EXTRA_PASS is not set
CONFIG_HOTPLUG=y
CONFIG_PRINTK=y
CONFIG_BUG=y
CONFIG_ELF_CORE=y
CONFIG_BASE_FULL=y
CONFIG_FUTEX=y
CONFIG_ANON_INODES=y
CONFIG_EPOLL=y
CONFIG_SIGNALFD=y
CONFIG_EVENTFD=y
CONFIG_VM_EVENT_COUNTERS=y
CONFIG_BIG_ORDER_ALLOC_NOFAIL_MAGIC=3
# CONFIG_NP2 is not set
# CONFIG_SLAB is not set
CONFIG_SLUB=y
CONFIG_SLUB_DEBUG=y
# CONFIG_SLUB_DEBUG_ON is not set
# CONFIG_SLUB_STATS is not set
# CONFIG_SLOB is not set
CONFIG_RT_MUTEXES=y
CONFIG_TINY_SHMEM=y
CONFIG_BASE_SMALL=0

#
# Loadable module support
#
CONFIG_MODULES=y
CONFIG_MODULE_UNLOAD=y
# CONFIG_MODULE_FORCE_UNLOAD is not set
# CONFIG_MODVERSIONS is not set
# CONFIG_MODULE_SRCVERSION_ALL is not set
CONFIG_KMOD=y

#
# Block layer
#
CONFIG_BLOCK=y
# CONFIG_LBD is not set
# CONFIG_BLK_DEV_IO_TRACE is not set
# CONFIG_LSF is not set

#
# IO Schedulers
#
CONFIG_IOSCHED_NOOP=y
CONFIG_IOSCHED_AS=y
# CONFIG_IOSCHED_DEADLINE is not set
CONFIG_IOSCHED_CFQ=y
CONFIG_DEFAULT_AS=y
# CONFIG_DEFAULT_DEADLINE is not set
# CONFIG_DEFAULT_CFQ is not set
# CONFIG_DEFAULT_NOOP is not set
CONFIG_DEFAULT_IOSCHED="anticipatory"
CONFIG_PREEMPT_NONE=y
# CONFIG_PREEMPT_VOLUNTARY is not set
# CONFIG_PREEMPT is not set

#
# Blackfin Processor Options
#

#
# Processor and Board Settings
#
# CONFIG_BF512 is not set
# CONFIG_BF514 is not set 
# CONFIG_BF516 is not set 
# CONFIG_BF518 is not set 
# CONFIG_BF522 is not set
# CONFIG_BF523 is not set
# CONFIG_BF524 is not set
# CONFIG_BF525 is not set
# CONFIG_BF526 is not set
# CONFIG_BF527 is not set
# CONFIG_BF531 is not set
# CONFIG_BF532 is not set
CONFIG_BF533=y
# CONFIG_BF534 is not set
# CONFIG_BF536 is not set
# CONFIG_BF538 is not set
# CONFIG_BF539 is not set
# CONFIG_BF537 is not set
# CONFIG_BF542 is not set
# CONFIG_BF542M is not set
# CONFIG_BF544 is not set
# CONFIG_BF544M is not set
# CONFIG_BF547 is not set
# CONFIG_BF547M is not set
# CONFIG_BF548 is not set
# CONFIG_BF548M is not set
# CONFIG_BF549 is not set
# CONFIG_BF549M is not set
# CONFIG_BF561 is not set
# CONFIG_BF_REV_0_0 is not set
# CONFIG_BF_REV_0_1 is not set
# CONFIG_BF_REV_0_2 is not set
# CONFIG_BF_REV_0_3 is not set
# CONFIG_BF_REV_0_4 is not set
CONFIG_BF_REV_0_5=y
# CONFIG_BF_REV_0_6 is not set
# CONFIG_BF_REV_ANY is not set
# CONFIG_BF_REV_NONE is not set
CONFIG_BF53x=y
CONFIG_BFIN_SINGLE_CORE=y
CONFIG_IRQ_PLL_WAKEUP=7
CONFIG_IRQ_RTC=8
CONFIG_IRQ_PPI=8
CONFIG_IRQ_SPORT0_RX=9
CONFIG_IRQ_SPORT0_TX=9
CONFIG_IRQ_SPORT1_RX=9
CONFIG_IRQ_SPORT1_TX=9
CONFIG_IRQ_TWI=10
CONFIG_IRQ_SPI=10
CONFIG_IRQ_UART0_RX=10
CONFIG_IRQ_UART0_TX=10
CONFIG_IRQ_UART1_RX=10
CONFIG_IRQ_UART1_TX=10
CONFIG_IRQ_MAC_RX=11
CONFIG_IRQ_MAC_TX=11
CONFIG_IRQ_TMR0=12
CONFIG_IRQ_TMR1=12
CONFIG_IRQ_TMR2=12
CONFIG_IRQ_TMR3=12
CONFIG_IRQ_TMR4=12
CONFIG_IRQ_TMR5=12
CONFIG_IRQ_TMR6=12
CONFIG_IRQ_TMR7=12
CONFIG_IRQ_PORTG_INTB=12
CONFIG_IRQ_MEM_DMA0=13
CONFIG_IRQ_MEM_DMA1=13
CONFIG_IRQ_WATCH=13
CONFIG_BFIN532=y
CONFIG_BFIN532_IP0X=y
# CONFIG_BFIN537_STAMP is not set
# CONFIG_BFIN537_BLUETECHNIX_CM is not set
# CONFIG_BFIN537_BLUETECHNIX_TCM is not set
# CONFIG_PNAV10 is not set
# CONFIG_CAMSIG_MINOTAUR is not set
# CONFIG_GENERIC_BF537_BOARD is not set
CONFIG_MEM_MT48LC32M8A2_75=y
#
# BF537 Specific Configuration
#

#
# Interrupt Priority Assignment
#

#
# Priority
#
CONFIG_IRQ_DMA_ERROR=7
CONFIG_IRQ_ERROR=7
CONFIG_IRQ_CAN_RX=11
CONFIG_IRQ_CAN_TX=11
CONFIG_IRQ_PROG_INTA=12
CONFIG_UART_ERROR=7
CONFIG_SPORT0_ERROR=7
CONFIG_SPI_ERROR=7
CONFIG_SPORT1_ERROR=7
CONFIG_PPI_ERROR=7
CONFIG_DMA_ERROR=7
CONFIG_PLLWAKE_ERROR=7
CONFIG_RTC_ERROR=8
CONFIG_DMA0_PPI=8
CONFIG_DMA1_SPORT0RX=9
CONFIG_DMA2_SPORT0TX=9
CONFIG_DMA3_SPORT1RX=9
CONFIG_DMA4_SPORT1TX=9
CONFIG_DMA5_SPI=10
CONFIG_DMA6_UARTRX=10
CONFIG_DMA7_UARTTX=10
CONFIG_TIMER0=8
CONFIG_TIMER1=11
CONFIG_TIMER2=11
CONFIG_PFA=12
CONFIG_PFB=12
CONFIG_MEMDMA0=13
CONFIG_MEMDMA1=13
CONFIG_WDTIMER=13


#
# Board customizations
#
# CONFIG_CMDLINE_BOOL is not set

#
# Clock/PLL Setup
#
CONFIG_CLKIN_HZ=10000000
# CONFIG_BFIN_KERNEL_CLOCK is not set
CONFIG_MAX_VCO_HZ=400000000
CONFIG_MIN_VCO_HZ=50000000
CONFIG_MAX_SCLK_HZ=133333333
CONFIG_MIN_SCLK_HZ=27000000


#
# Kernel Timer/Scheduler
#
# CONFIG_HZ_100 is not set
CONFIG_HZ_250=y
# CONFIG_HZ_300 is not set
# CONFIG_HZ_1000 is not set
CONFIG_HZ=250

#
# Hardware addresses
#
CONFIG_IP0X_NET1=0x20100000
CONFIG_IP0X_NET2=0x20200000
CONFIG_IP0X_USB=0x20300000


#
# Memory Setup
#
CONFIG_BOOT_LOAD=0x1000
CONFIG_BFIN_SCRATCH_REG_RETN=y
# CONFIG_BFIN_SCRATCH_REG_RETE is not set
# CONFIG_BFIN_SCRATCH_REG_CYCLES is not set

#
# Blackfin Kernel Optimizations
#

#
# Memory Optimizations
#
CONFIG_I_ENTRY_L1=y
CONFIG_EXCPT_IRQ_SYSC_L1=y
CONFIG_DO_IRQ_L1=y
CONFIG_CORE_TIMER_IRQ_L1=y
CONFIG_IDLE_L1=y
CONFIG_SCHEDULE_L1=y
CONFIG_ARITHMETIC_OPS_L1=y
CONFIG_ACCESS_OK_L1=y
CONFIG_MEMSET_L1=y
CONFIG_MEMCPY_L1=y
CONFIG_SYS_BFIN_SPINLOCK_L1=y
# CONFIG_IP_CHECKSUM_L1 is not set
CONFIG_CACHELINE_ALIGNED_L1=y
# CONFIG_SYSCALL_TAB_L1 is not set
# CONFIG_CPLB_SWITCH_TAB_L1 is not set
CONFIG_RAMKERNEL=y
# CONFIG_ROMKERNEL is not set
CONFIG_SELECT_MEMORY_MODEL=y
CONFIG_FLATMEM_MANUAL=y
# CONFIG_DISCONTIGMEM_MANUAL is not set
# CONFIG_SPARSEMEM_MANUAL is not set
CONFIG_FLATMEM=y
CONFIG_FLAT_NODE_MEM_MAP=y
# CONFIG_SPARSEMEM_STATIC is not set
CONFIG_SPLIT_PTLOCK_CPUS=4
# CONFIG_RESOURCES_64BIT is not set
CONFIG_ZONE_DMA_FLAG=1
CONFIG_LARGE_ALLOCS=y
CONFIG_BFIN_GPTIMERS=y
CONFIG_BFIN_DMA_5XX=y
# CONFIG_DMA_UNCACHED_4M is not set 
# CONFIG_DMA_UNCACHED_2M is not set
CONFIG_DMA_UNCACHED_1M=y
# CONFIG_DMA_UNCACHED_NONE is not set

#
# Cache Support
#
CONFIG_BFIN_ICACHE=y
CONFIG_BFIN_DCACHE=y
# CONFIG_BFIN_DCACHE_BANKA is not set
# CONFIG_BFIN_ICACHE_LOCK is not set
CONFIG_BFIN_WB=y
# CONFIG_BFIN_WT is not set
CONFIG_L1_MAX_PIECE=16
# CONFIG_MPU is not set

#
# Asynchonous Memory Configuration
#

#
# EBIU_AMGCTL Global Control
#
CONFIG_C_AMCKEN=y
CONFIG_C_CDPRIO=y
# CONFIG_C_AMBEN is not set
# CONFIG_C_AMBEN_B0 is not set
# CONFIG_C_AMBEN_B0_B1 is not set
# CONFIG_C_AMBEN_B0_B1_B2 is not set
CONFIG_C_AMBEN_ALL=y

#
# EBIU_AMBCTL Control
#
CONFIG_BANK_0=0x7BB0
CONFIG_BANK_1=0x7BB0
CONFIG_BANK_2=0x7BB0
CONFIG_BANK_3=0x99B3

#
# Bus options (PCI, PCMCIA, EISA, MCA, ISA)
#
# CONFIG_PCI is not set
# CONFIG_ARCH_SUPPORTS_MSI is not set

#
# PCCARD (PCMCIA/CardBus) support
#
# CONFIG_PCCARD is not set

#
# Executable file formats
#
CONFIG_BINFMT_ELF_FDPIC=y
CONFIG_BINFMT_FLAT=y
# CONFIG_BINFMT_ZFLAT is not set
# CONFIG_BINFMT_SHARED_FLAT is not set
# CONFIG_BINFMT_MISC is not set

#
# Power management options
#
# CONFIG_PM is not set
# CONFIG_PM_WAKEUP_BY_GPIO is not set

#
# CPU Frequency scaling
#
# CONFIG_CPU_FREQ is not set

#
# Networking
#
CONFIG_NET=y

#
# Networking options
#
CONFIG_PACKET=y
# CONFIG_PACKET_MMAP is not set
CONFIG_UNIX=y
CONFIG_XFRM=y
# CONFIG_XFRM_USER is not set
# CONFIG_XFRM_SUB_POLICY is not set
# CONFIG_XFRM_MIGRATE is not set
# CONFIG_NET_KEY is not set
CONFIG_INET=y
# CONFIG_IP_MULTICAST is not set
# CONFIG_IP_ADVANCED_ROUTER is not set
CONFIG_IP_FIB_HASH=y
CONFIG_IP_PNP=y
# CONFIG_IP_PNP_DHCP is not set
# CONFIG_IP_PNP_BOOTP is not set
# CONFIG_IP_PNP_RARP is not set
# CONFIG_NET_IPIP is not set
# CONFIG_NET_IPGRE is not set
# CONFIG_ARPD is not set
CONFIG_SYN_COOKIES=y
# CONFIG_INET_AH is not set
# CONFIG_INET_ESP is not set
# CONFIG_INET_IPCOMP is not set
# CONFIG_INET_XFRM_TUNNEL is not set
# CONFIG_INET_TUNNEL is not set
CONFIG_INET_XFRM_MODE_TRANSPORT=y
CONFIG_INET_XFRM_MODE_TUNNEL=y
CONFIG_INET_XFRM_MODE_BEET=y
CONFIG_INET_DIAG=y
CONFIG_INET_TCP_DIAG=y
# CONFIG_TCP_CONG_ADVANCED is not set
CONFIG_TCP_CONG_CUBIC=y
CONFIG_DEFAULT_TCP_CONG="cubic"
# CONFIG_TCP_MD5SIG is not set
# CONFIG_IPV6 is not set
# CONFIG_INET6_XFRM_TUNNEL is not set
# CONFIG_INET6_TUNNEL is not set
# CONFIG_NETLABEL is not set
# CONFIG_NETWORK_SECMARK is not set
# CONFIG_NETFILTER is not set
# CONFIG_IP_DCCP is not set
# CONFIG_IP_SCTP is not set
# CONFIG_TIPC is not set
# CONFIG_ATM is not set
# CONFIG_BRIDGE is not set
CONFIG_VLAN_8021Q=y
# CONFIG_DECNET is not set
# CONFIG_LLC2 is not set
# CONFIG_IPX is not set
# CONFIG_ATALK is not set
# CONFIG_X25 is not set
# CONFIG_LAPB is not set
# CONFIG_ECONET is not set
# CONFIG_WAN_ROUTER is not set

#
# QoS and/or fair queueing
#
# CONFIG_NET_SCHED is not set

#
# Network testing
#
# CONFIG_NET_PKTGEN is not set
# CONFIG_HAMRADIO is not set
# CONFIG_IRDA is not set
# CONFIG_BT is not set
# CONFIG_AF_RXRPC is not set
# CONFIG_CAN is not set

#
# Wireless
#
# CONFIG_CFG80211 is not set
# CONFIG_WIRELESS_EXT is not set
# CONFIG_MAC80211 is not set
# CONFIG_IEEE80211 is not set
# CONFIG_RFKILL is not set

#
# Device Drivers
#

#
# Generic Driver Options
#
CONFIG_STANDALONE=y
CONFIG_PREVENT_FIRMWARE_BUILD=y
# CONFIG_FW_LOADER is not set
# CONFIG_SYS_HYPERVISOR is not set

#
# Connector - unified userspace <-> kernelspace linker
#
# CONFIG_CONNECTOR is not set
CONFIG_MTD=y
# CONFIG_MTD_DEBUG is not set
# CONFIG_MTD_CONCAT is not set
CONFIG_MTD_PARTITIONS=y
# CONFIG_MTD_REDBOOT_PARTS is not set
# CONFIG_MTD_CMDLINE_PARTS is not set

#
# User Modules And Translation Layers
#
CONFIG_MTD_CHAR=y
CONFIG_MTD_BLKDEVS=y
CONFIG_MTD_BLOCK=y
# CONFIG_FTL is not set
# CONFIG_NFTL is not set
# CONFIG_INFTL is not set
# CONFIG_RFD_FTL is not set
# CONFIG_SSFDC is not set

#
# RAM/ROM/Flash chip drivers
#
CONFIG_MTD_CFI=y
# CONFIG_MTD_JEDECPROBE is not set
CONFIG_MTD_GEN_PROBE=y
# CONFIG_MTD_CFI_ADV_OPTIONS is not set
CONFIG_MTD_MAP_BANK_WIDTH_1=y
CONFIG_MTD_MAP_BANK_WIDTH_2=y
CONFIG_MTD_MAP_BANK_WIDTH_4=y
# CONFIG_MTD_MAP_BANK_WIDTH_8 is not set
# CONFIG_MTD_MAP_BANK_WIDTH_16 is not set
# CONFIG_MTD_MAP_BANK_WIDTH_32 is not set
CONFIG_MTD_CFI_I1=y
CONFIG_MTD_CFI_I2=y
# CONFIG_MTD_CFI_I4 is not set
# CONFIG_MTD_CFI_I8 is not set
# CONFIG_MTD_CFI_INTELEXT is not set
# CONFIG_MTD_CFI_AMDSTD is not set
# CONFIG_MTD_CFI_STAA is not set
CONFIG_MTD_RAM=y
# CONFIG_MTD_ROM is not set
# CONFIG_MTD_ABSENT is not set

#
# Mapping drivers for chip access
#
CONFIG_MTD_COMPLEX_MAPPINGS=y
# CONFIG_MTD_PHYSMAP is not set
CONFIG_MTD_UCLINUX=y
CONFIG_MTD_PLATRAM=y

#
# Self-contained MTD device drivers
#
# CONFIG_MTD_DATAFLASH is not set
CONFIG_MTD_M25P80=y
CONFIG_M25PXX_USE_FAST_READ=y
# CONFIG_MTD_SLRAM is not set
# CONFIG_MTD_PHRAM is not set
# CONFIG_MTD_MTDRAM is not set
# CONFIG_MTD_BLOCK2MTD is not set

#
# Disk-On-Chip Device Drivers
#
# CONFIG_MTD_DOC2000 is not set
# CONFIG_MTD_DOC2001 is not set
# CONFIG_MTD_DOC2001PLUS is not set
CONFIG_MTD_NAND=y
# CONFIG_MTD_NAND_VERIFY_WRITE is not set
# CONFIG_MTD_NAND_ECC_SMC is not set
# CONFIG_MTD_NAND_MUSEUM_IDS is not set
CONFIG_MTD_NAND_BFIN=y
CONFIG_BFIN_NAND_BASE=0x20000000
CONFIG_BFIN_NAND_SIZE=0x10000000
CONFIG_BFIN_NAND_CLE=2
CONFIG_BFIN_NAND_ALE=1
CONFIG_BFIN_NAND_READY=10
CONFIG_MTD_NAND_IDS=y
# CONFIG_MTD_NAND_DISKONCHIP is not set
# CONFIG_MTD_NAND_NANDSIM is not set
# CONFIG_MTD_NAND_PLATFORM is not set
# CONFIG_MTD_ONENAND is not set

#
# UBI - Unsorted block images
#
CONFIG_MTD_UBI=y
CONFIG_MTD_UBI_WL_THRESHOLD=4096
CONFIG_MTD_UBI_BEB_RESERVE=1
# CONFIG_MTD_UBI_GLUEBI is not set
# CONFIG_MTD_UBI_DEBUG is not set

#
# Parallel port support
#
# CONFIG_PARPORT is not set

#
# Plug and Play support
#
# CONFIG_PNPACPI is not set

#
# Block devices
#
# CONFIG_BLK_DEV_COW_COMMON is not set
# CONFIG_BLK_DEV_LOOP is not set
# CONFIG_BLK_DEV_NBD is not set
CONFIG_BLK_DEV_RAM=y
CONFIG_BLK_DEV_RAM_COUNT=16
CONFIG_BLK_DEV_RAM_SIZE=4096
CONFIG_BLK_DEV_RAM_BLOCKSIZE=1024
# CONFIG_CDROM_PKTCDVD is not set
# CONFIG_ATA_OVER_ETH is not set

#
# Misc devices
#
# CONFIG_IDE is not set

#
# SCSI device support
#
# CONFIG_RAID_ATTRS is not set
# CONFIG_SCSI is not set
# CONFIG_SCSI_NETLINK is not set
# CONFIG_ATA is not set

#
# Multi-device support (RAID and LVM)
#
# CONFIG_MD is not set

#
# Network device support
#
CONFIG_NETDEVICES=y
# CONFIG_DUMMY is not set
# CONFIG_BONDING is not set
# CONFIG_EQUALIZER is not set
CONFIG_TUN=y
CONFIG_PHYLIB=y

#
# MII PHY device drivers
#
# CONFIG_MARVELL_PHY is not set
# CONFIG_DAVICOM_PHY is not set
# CONFIG_QSEMI_PHY is not set
# CONFIG_LXT_PHY is not set
# CONFIG_CICADA_PHY is not set
# CONFIG_VITESSE_PHY is not set
# CONFIG_SMSC_PHY is not set
# CONFIG_BROADCOM_PHY is not set
# CONFIG_FIXED_PHY is not set

#
# Ethernet (10 or 100Mbit)
#
CONFIG_NET_ETHERNET=y
CONFIG_MII=y
# CONFIG_SMC91X is not set
# CONFIG_SMSC911X is not set
CONFIG_DM9000=y
CONFIG_NETDEV_1000=y
# CONFIG_AX88180 is not set
CONFIG_NETDEV_10000=y
CONFIG_DM9000_DEBUGLEVEL=4
# CONFIG_DM9000_FORCE_SIMPLE_PHY_POLL is not set

#
# Wireless LAN
#
# CONFIG_WLAN_PRE80211 is not set
# CONFIG_WLAN_80211 is not set
# CONFIG_WAN is not set
# CONFIG_PPP is not set
# CONFIG_SLIP is not set
# CONFIG_SHAPER is not set
# CONFIG_NETCONSOLE is not set
# CONFIG_NETPOLL is not set
# CONFIG_NET_POLL_CONTROLLER is not set

#
# ISDN subsystem
#
# CONFIG_ISDN is not set

#
# Telephony Support
#
# CONFIG_PHONE is not set

#
# Input device support
#
# CONFIG_INPUT is not set

#
# Hardware I/O ports
#
# CONFIG_SERIO is not set
# CONFIG_GAMEPORT is not set

#
# Character devices
#
# CONFIG_AD9960 is not set
# CONFIG_SPI_ADC_BF533 is not set
# CONFIG_BF5xx_PFLAGS is not set
# CONFIG_BF5xx_PPIFCD is not set
# CONFIG_BFIN_SIMPLE_TIMER is not set
# CONFIG_BF5xx_PPI is not set
CONFIG_BFIN_SPORT=y
# CONFIG_BFIN_TIMER_LATENCY is not set
# CONFIG_AD5304 is not set
# CONFIG_VT is not set
# CONFIG_SERIAL_NONSTANDARD is not set

#
# Serial drivers
#
# CONFIG_SERIAL_8250 is not set

#
# Non-8250 serial port support
#
CONFIG_SERIAL_BFIN=y
CONFIG_SERIAL_BFIN_CONSOLE=y
CONFIG_SERIAL_BFIN_DMA=y
# CONFIG_SERIAL_BFIN_PIO is not set
CONFIG_SERIAL_BFIN_UART0=y
# CONFIG_BFIN_UART0_CTSRTS is not set
# CONFIG_SERIAL_BFIN_UART1 is not set
CONFIG_SERIAL_CORE=y
CONFIG_SERIAL_CORE_CONSOLE=y
# CONFIG_SERIAL_BFIN_SPORT is not set
CONFIG_UNIX98_PTYS=y
# CONFIG_LEGACY_PTYS is not set
# CONFIG_SDIO_UART is not set

#
# CAN, the car bus and industrial fieldbus
#
# CONFIG_CAN4LINUX is not set

#
# IPMI
#
# CONFIG_IPMI_HANDLER is not set
CONFIG_WATCHDOG=y
# CONFIG_WATCHDOG_NOWAYOUT is not set

#
# Watchdog Device Drivers
#
# CONFIG_SOFT_WATCHDOG is not set
CONFIG_BFIN_WDT=y
CONFIG_HW_RANDOM=y
# CONFIG_GEN_RTC is not set
# CONFIG_R3964 is not set
# CONFIG_RAW_DRIVER is not set

#
# TPM devices
#
# CONFIG_TCG_TPM is not set
# CONFIG_I2C is not set

#
# SPI support
#
CONFIG_SPI=y
CONFIG_SPI_MASTER=y

#
# SPI Master Controller Drivers
#
CONFIG_SPI_BFIN=y
# CONFIG_SPI_BITBANG is not set

#
# SPI Protocol Masters
#
# CONFIG_SPI_AT25 is not set
# CONFIG_SPI_SPIDEV is not set

#
# Dallas's 1-wire bus
#
# CONFIG_W1 is not set
# CONFIG_HWMON is not set

#
# Multifunction device drivers
#
# CONFIG_MFD_SM501 is not set

#
# Multimedia devices
#
# CONFIG_VIDEO_DEV is not set
# CONFIG_DVB_CORE is not set
# CONFIG_DAB is not set

#
# Graphics support
#
# CONFIG_BACKLIGHT_LCD_SUPPORT is not set

#
# Display device support
#
# CONFIG_DISPLAY_SUPPORT is not set
# CONFIG_VGASTATE is not set
# CONFIG_FB is not set

#
# Sound
#
# CONFIG_SOUND is not set

#
# USB support
#
# CONFIG_USB_ARCH_HAS_HCD is not set
# CONFIG_USB_ARCH_HAS_OHCI is not set
# CONFIG_USB_ARCH_HAS_EHCI is not set
# CONFIG_USB is not set
# CONFIG_USB_OTG_WHITELIST is not set
# CONFIG_USB_OTG_BLACKLIST_HUB is not set

#
# Enable Host or Gadget support to see Inventra options
#

#
# NOTE: USB_STORAGE enables SCSI, and 'SCSI disk support'
#

#
# USB Gadget Support
#
# CONFIG_USB_GADGET is not set
CONFIG_MMC=m
# CONFIG_MMC_DEBUG is not set
# CONFIG_MMC_UNSAFE_RESUME is not set

#
# MMC/SD Card Drivers
#
CONFIG_MMC_BLOCK=m

#
# MMC/SD Host Controller Drivers
#
CONFIG_SPI_MMC=m
CONFIG_MMC_SPI=m  
CONFIG_SPI_MMC_FRAMEWORK_DRIVER=y
# CONFIG_SPI_MMC_BFIN_PIO_SPI is not set
CONFIG_SPI_MMC_CS_CHAN=4
CONFIG_SPI_MMC_MAX_HZ=20000000
# CONFIG_SPI_MMC_CARD_DETECT is not set
# CONFIG_SPI_MMC_DEBUG_MODE is not set
CONFIG_MMC_BLOCK_BOUNCE=y
# CONFIG_MMC_TEST is not set
# CONFIG_MMC_SDHCI is not set

#
# LED devices
#
# CONFIG_NEW_LEDS is not set

#
# LED drivers
#

#
# LED Triggers
#

#
# InfiniBand support
#

#
# EDAC - error detection and reporting (RAS) (EXPERIMENTAL)
#

#
# Real Time Clock
#
# CONFIG_RTC_LIB is not set
# CONFIG_RTC_CLASS is not set
# CONFIG_RTC_HCTOSYS is not set
# CONFIG_RTC_HCTOSYS_DEVICE="rtc0"
# CONFIG_RTC_DEBUG is not set

#
# RTC interfaces
#
# CONFIG_RTC_INTF_SYSFS is not set
# CONFIG_RTC_INTF_PROC is not set
# CONFIG_RTC_INTF_DEV is not set
# CONFIG_RTC_INTF_DEV_UIE_EMUL is not set
# CONFIG_RTC_DRV_TEST is not set

#
# I2C RTC drivers
#

#
# SPI RTC drivers
#
# CONFIG_RTC_DRV_RS5C348 is not set
# CONFIG_RTC_DRV_MAX6902 is not set

#
# Platform RTC drivers
#
# CONFIG_RTC_DRV_DS1553 is not set
# CONFIG_RTC_DRV_DS1742 is not set
# CONFIG_RTC_DRV_M48T86 is not set
# CONFIG_RTC_DRV_V3020 is not set

#
# on-CPU RTC drivers
#
# CONFIG_RTC_DRV_BFIN is not set

#
# DMA Engine support
#
# CONFIG_DMA_ENGINE is not set

#
# DMA Clients
#

#
# DMA Devices
#

#
# PBX support
#
# CONFIG_PBX is not set

#
# File systems
#
CONFIG_EXT2_FS=y
CONFIG_EXT2_FS_XATTR=y
# CONFIG_EXT2_FS_POSIX_ACL is not set
# CONFIG_EXT2_FS_SECURITY is not set
# CONFIG_EXT3_FS is not set
# CONFIG_EXT4DEV_FS is not set
CONFIG_FS_MBCACHE=y
# CONFIG_REISERFS_FS is not set
# CONFIG_JFS_FS is not set
# CONFIG_FS_POSIX_ACL is not set
# CONFIG_XFS_FS is not set
# CONFIG_GFS2_FS is not set
# CONFIG_OCFS2_FS is not set
# CONFIG_MINIX_FS is not set
# CONFIG_ROMFS_FS is not set
CONFIG_INOTIFY=y
CONFIG_INOTIFY_USER=y
# CONFIG_QUOTA is not set
CONFIG_DNOTIFY=y
# CONFIG_AUTOFS_FS is not set
# CONFIG_AUTOFS4_FS is not set
# CONFIG_FUSE_FS is not set

#
# CD-ROM/DVD Filesystems
#
# CONFIG_ISO9660_FS is not set
# CONFIG_UDF_FS is not set

#
# DOS/FAT/NT Filesystems
#
CONFIG_FAT_FS=y
CONFIG_MSDOS_FS=y
CONFIG_VFAT_FS=y
CONFIG_FAT_DEFAULT_CODEPAGE=437
CONFIG_FAT_DEFAULT_IOCHARSET="iso8859-1"
# CONFIG_NTFS_FS is not set

#
# Pseudo filesystems
#
CONFIG_PROC_FS=y
CONFIG_PROC_SYSCTL=y
CONFIG_SYSFS=y
# CONFIG_TMPFS is not set
# CONFIG_HUGETLB_PAGE is not set
CONFIG_RAMFS=y
# CONFIG_CONFIGFS_FS is not set

#
# Miscellaneous filesystems
#
# CONFIG_ADFS_FS is not set
# CONFIG_AFFS_FS is not set
# CONFIG_HFS_FS is not set
# CONFIG_HFSPLUS_FS is not set
# CONFIG_BEFS_FS is not set
# CONFIG_BFS_FS is not set
# CONFIG_EFS_FS is not set
# CONFIG_YAFFS_FS is not set
# CONFIG_YAFFS_YAFFS1 is not set
# CONFIG_YAFFS_DOES_ECC is not set
# CONFIG_YAFFS_YAFFS2 is not set
# CONFIG_YAFFS_AUTO_YAFFS2 is not set
# CONFIG_YAFFS_DISABLE_LAZY_LOAD is not set
# CONFIG_YAFFS_CHECKPOINT_RESERVED_BLOCKS is not set
# CONFIG_YAFFS_DISABLE_WIDE_TNODES is not set
# CONFIG_YAFFS_ALWAYS_CHECK_CHUNK_ERASED is not set
# CONFIG_YAFFS_SHORT_NAMES_IN_RAM is not set
CONFIG_UBIFS_FS=y
CONFIG_UBIFS_FS_ADVANCED_COMPR=y
CONFIG_UBIFS_FS_LZO=y
# CONFIG_UBIFS_FS_XATTR is not set
# CONFIG_UBIFS_FS_ZLIB is not set
# CONFIG_UBIFS_FS_DEBUG is not set
# CONFIG_UBIFS_FS_DEBUG_CHKS is not set
# CONFIG_UBIFS_FS_DEBUG_MSG_LVL is not set
CONFIG_JFFS2_FS=m
CONFIG_JFFS2_FS_DEBUG=0
CONFIG_JFFS2_FS_WRITEBUFFER=y
# CONFIG_JFFS2_SUMMARY is not set
# CONFIG_JFFS2_FS_XATTR is not set
# CONFIG_JFFS2_COMPRESSION_OPTIONS is not set
CONFIG_JFFS2_ZLIB=y
CONFIG_JFFS2_RTIME=y
# CONFIG_JFFS2_RUBIN is not set
# CONFIG_CRAMFS is not set
# CONFIG_VXFS_FS is not set
# CONFIG_HPFS_FS is not set
# CONFIG_QNX4FS_FS is not set
# CONFIG_SYSV_FS is not set
# CONFIG_UFS_FS is not set

#
# Network File Systems
#
# CONFIG_NFS_FS is not set
# CONFIG_NFSD is not set
# CONFIG_SMB_FS is not set
# CONFIG_CIFS is not set
# CONFIG_NCP_FS is not set
# CONFIG_CODA_FS is not set
# CONFIG_AFS_FS is not set
# CONFIG_9P_FS is not set

#
# Partition Types
#
# CONFIG_PARTITION_ADVANCED is not set
CONFIG_MSDOS_PARTITION=y

#
# Native Language Support
#
CONFIG_NLS=y
CONFIG_NLS_DEFAULT="iso8859-1"
CONFIG_NLS_CODEPAGE_437=y
# CONFIG_NLS_CODEPAGE_737 is not set
# CONFIG_NLS_CODEPAGE_775 is not set
CONFIG_NLS_CODEPAGE_850=y
# CONFIG_NLS_CODEPAGE_852 is not set
# CONFIG_NLS_CODEPAGE_855 is not set
# CONFIG_NLS_CODEPAGE_857 is not set
# CONFIG_NLS_CODEPAGE_860 is not set
# CONFIG_NLS_CODEPAGE_861 is not set
# CONFIG_NLS_CODEPAGE_862 is not set
# CONFIG_NLS_CODEPAGE_863 is not set
# CONFIG_NLS_CODEPAGE_864 is not set
# CONFIG_NLS_CODEPAGE_865 is not set
# CONFIG_NLS_CODEPAGE_866 is not set
# CONFIG_NLS_CODEPAGE_869 is not set
# CONFIG_NLS_CODEPAGE_936 is not set
# CONFIG_NLS_CODEPAGE_950 is not set
# CONFIG_NLS_CODEPAGE_932 is not set
# CONFIG_NLS_CODEPAGE_949 is not set
# CONFIG_NLS_CODEPAGE_874 is not set
# CONFIG_NLS_ISO8859_8 is not set
# CONFIG_NLS_CODEPAGE_1250 is not set
# CONFIG_NLS_CODEPAGE_1251 is not set
# CONFIG_NLS_ASCII is not set
CONFIG_NLS_ISO8859_1=y
# CONFIG_NLS_ISO8859_2 is not set
# CONFIG_NLS_ISO8859_3 is not set
# CONFIG_NLS_ISO8859_4 is not set
# CONFIG_NLS_ISO8859_5 is not set
# CONFIG_NLS_ISO8859_6 is not set
# CONFIG_NLS_ISO8859_7 is not set
# CONFIG_NLS_ISO8859_9 is not set
# CONFIG_NLS_ISO8859_13 is not set
# CONFIG_NLS_ISO8859_14 is not set
# CONFIG_NLS_ISO8859_15 is not set
# CONFIG_NLS_KOI8_R is not set
# CONFIG_NLS_KOI8_U is not set
# CONFIG_NLS_UTF8 is not set

#
# Distributed Lock Manager
#
# CONFIG_DLM is not set

#
# Profiling support
#
# CONFIG_PROFILING is not set

#Penev!!!! take use of hardware performance monitor to profiling the kernel and application.     
# CONFIG_HARDWARE_PM is not set

#
# Kernel hacking
#
# CONFIG_PRINTK_TIME is not set
CONFIG_ENABLE_MUST_CHECK=y
# CONFIG_MAGIC_SYSRQ is not set
# CONFIG_UNUSED_SYMBOLS is not set
# CONFIG_DEBUG_FS is not set
# CONFIG_HEADERS_CHECK is not set
# CONFIG_DEBUG_KERNEL is not set
# CONFIG_DEBUG_BUGVERBOSE is not set
# CONFIG_DEBUG_MMRS is not set
CONFIG_DEBUG_HUNT_FOR_ZERO=y
CONFIG_DEBUG_BFIN_HWTRACE_ON=y
CONFIG_DEBUG_BFIN_HWTRACE_COMPRESSION_OFF=y
# CONFIG_DEBUG_BFIN_HWTRACE_COMPRESSION_ONE is not set
# CONFIG_DEBUG_BFIN_HWTRACE_COMPRESSION_TWO is not set
CONFIG_DEBUG_BFIN_HWTRACE_COMPRESSION=0
# CONFIG_DEBUG_BFIN_HWTRACE_EXPAND is not set
# CONFIG_DEBUG_BFIN_NO_KERN_HWTRACE is not set
# CONFIG_EARLY_PRINTK is not set
CONFIG_CPLB_INFO=y
CONFIG_ACCESS_CHECK=y

#
# Security options
#
# CONFIG_KEYS is not set
CONFIG_SECURITY=y
# CONFIG_SECURITY_NETWORK is not set
CONFIG_SECURITY_CAPABILITIES=m

#
# Cryptographic options
#
# CONFIG_CRYPTO is not set

#
# Library routines
#
CONFIG_BITREVERSE=y
# CONFIG_CRC_ITU_T is not set
CONFIG_CRC32=y
# CONFIG_LIBCRC32C is not set
CONFIG_ZLIB_INFLATE=m
CONFIG_ZLIB_DEFLATE=m
CONFIG_PLIST=y
CONFIG_HAS_IOMEM=y
CONFIG_HAS_IOPORT=y
CONFIG_HAS_DMA=y


#Penev added ========================================== 
# CONFIG_CGROUPS is not set
# CONFIG_GROUP_SCHED is not set
# CONFIG_SYSFS_DEPRECATED_V2 is not set
# CONFIG_NAMESPACES is not set
# CONFIG_TIMERFD is not set
# CONFIG_AIO is not set

#Should be tested
CONFIG_COMPAT_BRK=y

#
# General setup
#
#
# CONFIG_PROFILING is not set
# CONFIG_MARKERS is not set
# CONFIG_MODULE_FORCE_LOAD is not set
# CONFIG_BLK_DEV_BSG is not set
# CONFIG_BLK_DEV_INTEGRITY is not set

#
# Processor and Board Settings
#
CONFIG_IRQ_TIMER0=12
CONFIG_IRQ_TIMER1=12
CONFIG_IRQ_TIMER2=12
CONFIG_IRQ_TIMER3=12
CONFIG_IRQ_TIMER4=12
CONFIG_IRQ_TIMER5=12
CONFIG_IRQ_TIMER6=12
CONFIG_IRQ_TIMER7=12

#
# Memory Optimizations
#
CONFIG_APP_STACK_L1=y

#
# Speed Optimizations
#
CONFIG_BFIN_INS_LOWOVERHEAD=y

#
# Networking options
#
# CONFIG_XFRM_STATISTICS is not set
# CONFIG_INET_LRO is not set
# CONFIG_NET_DSA is not set

#
# linux embedded drivers
#
# CONFIG_CAN_BLACKFIN is not set

#
# FIR device drivers
#
# CONFIG_PHONET is not set
# CONFIG_WIRELESS is not set
# CONFIG_NET_9P is not set

#
# Generic Driver Options
#
CONFIG_UEVENT_HELPER_PATH="/sbin/hotplug"
# CONFIG_MTD_AR7_PARTS is not set

#
# User Modules And Translation Layers
#
# CONFIG_MTD_OOPS is not set

#
# UBI - Unsorted block images
#
CONFIG_BLK_DEV=y
# CONFIG_BLK_DEV_XIP is not set
# CONFIG_BLK_DEV_HD is not set
# CONFIG_MISC_DEVICES is not set
# CONFIG_EEPROM_93CX6 is not set
# CONFIG_ENCLOSURE_SERVICES is not set
# CONFIG_C2PORT is not set

#
# SCSI device support
#
# CONFIG_MACVLAN is not set
# CONFIG_VETH is not set

#
# MII PHY device drivers
#
# CONFIG_ICPLUS_PHY is not set
# CONFIG_REALTEK_PHY is not set
# CONFIG_MDIO_BITBANG is not set
# CONFIG_ENC28J60 is not set
# CONFIG_B44 is not set

#
# Character devices
#
CONFIG_BFIN_DMA_INTERFACE=m
# CONFIG_BFIN_PPI is not set
# CONFIG_BFIN_PPIFCD is not set
# CONFIG_BFIN_SPI_ADC is not set
# CONFIG_SIMPLE_GPIO is not set
# CONFIG_DEVKMEM is not set
# CONFIG_BFIN_JTAG_COMM is not set

#
# SPI Master Controller Drivers
#
# CONFIG_SPI_BFIN_LOCK is not set
# CONFIG_SPI_BFIN_SPORT is not set
# CONFIG_SPI_TLE62X0 is not set
# CONFIG_GPIOLIB is not set
# CONFIG_POWER_SUPPLY is not set
# CONFIG_THERMAL is not set
# CONFIG_THERMAL_HWMON is not set


#
# Watchdog Device Drivers
#
# CONFIG_SSB is not set


#
# Multifunction device drivers
#
# CONFIG_HTC_PASIC3 is not set
# CONFIG_REGULATOR is not set

#
# Graphics support
#
# CONFIG_VIDEO_OUTPUT_CONTROL is not set

#
# Special HID drivers
#
# CONFIG_USB_SUPPORT is not set
# CONFIG_MEMSTICK is not set
# CONFIG_ACCESSIBILITY is not set

#
# Platform RTC drivers
#
# CONFIG_RTC_DRV_M41T94 is not set
# CONFIG_RTC_DRV_DS1305 is not set
# CONFIG_RTC_DRV_DS1390 is not set
# CONFIG_RTC_DRV_R9701 is not set
# CONFIG_RTC_DRV_DS3234 is not set
# CONFIG_RTC_DRV_DS1286 is not set
# CONFIG_RTC_DRV_DS1511 is not set
# CONFIG_RTC_DRV_STK17TA8 is not set
# CONFIG_RTC_DRV_M48T35 is not set
# CONFIG_RTC_DRV_M48T59 is not set
# CONFIG_RTC_DRV_BQ4802 is not set

#
# on-CPU RTC drivers
#
# CONFIG_DMADEVICES is not set
# CONFIG_UIO is not set
# CONFIG_STAGING is not set

#
# File systems
#
# CONFIG_EXT4_FS is not set
CONFIG_FILE_LOCKING=y
# CONFIG_JFFS2_FS_WBUF_VERIFY is not set
# CONFIG_YAFFS_9BYTE_TAGS is not set
# CONFIG_OMFS_FS is not set
CONFIG_NETWORK_FILESYSTEMS=y

#
# Kernel hacking
#

CONFIG_ENABLE_WARN_DEPRECATED=y
CONFIG_FRAME_WARN=1024

# CONFIG_DEBUG_MEMORY_INIT is not set
# CONFIG_RCU_CPU_STALL_DETECTOR is not set

#
# Tracers
#
# CONFIG_DYNAMIC_PRINTK_DEBUG is not set
# CONFIG_SAMPLES is not set

# CONFIG_DEBUG_VERBOSE is not set
# CONFIG_DEBUG_DOUBLEFAULT is not set
# CONFIG_DEBUG_DOUBLEFAULT_PRINT is not set

# CONFIG_DEBUG_DOUBLEFAULT_RESET is not set

#
# Security options
#
# CONFIG_SECURITYFS is not set
# CONFIG_SECURITY_FILE_CAPABILITIES is not set
CONFIG_SECURITY_DEFAULT_MMAP_MIN_ADDR=0

#
# Crypto core or helper
#
# CONFIG_CRYPTO_FIPS is not set
# CONFIG_CRYPTO_MANAGER is not set
# CONFIG_CRYPTO_GF128MUL is not set
# CONFIG_CRYPTO_NULL is not set
# CONFIG_CRYPTO_CRYPTD is not set
# CONFIG_CRYPTO_AUTHENC is not set
# CONFIG_CRYPTO_TEST is not set

#
# Authenticated Encryption with Associated Data
#
# CONFIG_CRYPTO_CCM is not set
# CONFIG_CRYPTO_GCM is not set
# CONFIG_CRYPTO_SEQIV is not set

#
# Block modes
#
# CONFIG_CRYPTO_CBC is not set
# CONFIG_CRYPTO_CTR is not set
# CONFIG_CRYPTO_CTS is not set
# CONFIG_CRYPTO_ECB is not set
# CONFIG_CRYPTO_LRW is not set
# CONFIG_CRYPTO_PCBC is not set
# CONFIG_CRYPTO_XTS is not set

#
# Hash modes
#
# CONFIG_CRYPTO_HMAC is not set
# CONFIG_CRYPTO_XCBC is not set

#
# Digest
#
# CONFIG_CRYPTO_CRC32C is not set
# CONFIG_CRYPTO_MD4 is not set
# CONFIG_CRYPTO_MD5 is not set
# CONFIG_CRYPTO_MICHAEL_MIC is not set
# CONFIG_CRYPTO_RMD128 is not set
# CONFIG_CRYPTO_RMD160 is not set
# CONFIG_CRYPTO_RMD256 is not set
# CONFIG_CRYPTO_RMD320 is not set
# CONFIG_CRYPTO_SHA1 is not set
# CONFIG_CRYPTO_SHA256 is not set
# CONFIG_CRYPTO_SHA512 is not set
# CONFIG_CRYPTO_TGR192 is not set
# CONFIG_CRYPTO_WP512 is not set

#
# Ciphers
#
# CONFIG_CRYPTO_AES is not set
# CONFIG_CRYPTO_ANUBIS is not set
# CONFIG_CRYPTO_ARC4 is not set
# CONFIG_CRYPTO_BLOWFISH is not set
# CONFIG_CRYPTO_CAMELLIA is not set
# CONFIG_CRYPTO_CAST5 is not set
# CONFIG_CRYPTO_CAST6 is not set
# CONFIG_CRYPTO_DES is not set
# CONFIG_CRYPTO_FCRYPT is not set
# CONFIG_CRYPTO_KHAZAD is not set
# CONFIG_CRYPTO_SALSA20 is not set
# CONFIG_CRYPTO_SEED is not set
# CONFIG_CRYPTO_SERPENT is not set
# CONFIG_CRYPTO_TEA is not set
# CONFIG_CRYPTO_TWOFISH is not set

#
# Compression
#
# CONFIG_CRYPTO_DEFLATE is not set
# CONFIG_CRYPTO_LZO is not set

# Random Number Generation
#
# CONFIG_CRYPTO_ANSI_CPRNG is not set
CONFIG_CRYPTO_HW=y

#
# Library routines
#
CONFIG_CRC_CCITT=m
# CONFIG_CRC16 is not set
# CONFIG_CRC_T10DIF is not set
# CONFIG_CRC7 is not set


# CONFIG_VLAN_8021Q_GVRP is not set
# CONFIG_MTD_GPIO_ADDR is not set 
# CONFIG_SYSCTL_SYSCALL_CHECK is not set

