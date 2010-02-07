/*
multi_spi.c

Multi SPI Framework - This framework provides multi cs for multi FXS interfaces.

Author: Pawel Pastuszak @ gmail . com

*/
/*
Copyright (C) 2009 Pawel Pastuszak <pawelpastuszak@gmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. 
*/

#ifdef MULTI_SPI_FRAMEWORK

#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/fcntl.h>
#include <linux/delay.h>
#include <asm/blackfin.h>
#include <asm/system.h>
#include <asm/uaccess.h>


#include "multi_spi.h"

#include "bfsi.h"


static void _multi_spi_reset(int reset_bit);

int multi_spi_board_size() {
	return dev_board_size;
}

void multi_spi_init() {
	unsigned int mask = 0;
	int i=0;
	
	printk("%s: board size: %d\n", __FUNCTION__, dev_board_size);

	for (i=0; i < dev_board_size; i++) {
		mask |= ( 1 << dev_board[i].chip_cs );
	}

	printk("%s: mask: %d\n", __FUNCTION__, mask);
	bfsi_spi_init(SPI_BAUDS, mask);
}

void multi_spi_reset() {
	if (dev_board_size > 0 ) {
		#ifdef SINGLE_RESET
			_multi_spi_reset ( dev_board[0].reset );
		#else 
			int i=0;
			for (i=0; i < dev_board_size; i++) {
				_multi_spi_reset ( dev_board[i].reset );
			}
		#endif
	}
}

static void _multi_spi_reset(int reset_bit)
{
#if (defined(CONFIG_BF536) || defined(CONFIG_BF537))
	//printk("_multi_spi_reset: Reset PF%d\n",reset_bit); 
	bfin_write_PORTF_FER(bfin_read_PORTF_FER() & (0xFFFF & ~ (1 << reset_bit)) );
    __builtin_bfin_ssync();
					                
	bfin_write_PORTFIO_DIR(bfin_read_PORTFIO_DIR() | (1<<reset_bit));
	__builtin_bfin_ssync();

	bfin_write_PORTFIO_CLEAR((1<<reset_bit));
	__builtin_bfin_ssync();
	udelay(100);

	bfin_write_PORTFIO_SET((1<<reset_bit));

	__builtin_bfin_ssync();
#endif
  	udelay(1000); 
}


/**
 * multi_spi_read_fxo  - Detect if this fxo interface 
 * return: unsigned char 
 */
static u8 multi_spi_read_fxo(int cs) {
	udelay(100);
	bfsi_spi_write_8_bits(cs, 0x02);
	return bfsi_spi_read_8_bits(cs);
}

/**
 * multi_spi_read_fxs  - Detect if this fxs interface
 * @param 	int - Chip Select
 * @param 	u8  - register
 * @return unsigned char 
 */
static u8 multi_spi_read_fxs(int cs,u8 bits) {
	u8  reg;
	
	udelay(100);

    bfsi_spi_write_8_bits(cs, bits | 0x80);
    reg =  bfsi_spi_read_8_bits(cs);
    
  	return reg;
}

/**
 * multi_spi_write_8_bits - Write 8 bits on the spi 
 *
 * @param  int - Port Number of the Device
 * @param  u8  - Register to read
 *
 * @return void
 */
void multi_spi_write_8_bits(int port, u8 reg) {
	if ( port < 0 ) 
		return;

	bfsi_spi_write_8_bits(dev_board[port].chip_cs, reg);
}

/**
 * multi_spi_read_8_bits - Read 8 bits of the spi flash
 *
 * @param  int - Port Number of the Device
 * @param  u8  - Register to read
 *
 * @return u8
 */
u8 multi_spi_read_8_bits(int port, u8 reg) {
	u8 val;
	if ( port < 0 ) 
		return -1;

	return bfsi_spi_read_8_bits(dev_board[port].chip_cs);
}


/**
 * multi_spi_auto_detect 
 * 		Auto detect FXS/FXO
 *
 * @param char * - port_type
 *
 * @return void
 */
void multi_spi_auto_detect(char port_type[]) {
  int i;
  u8  reg;

  for(i=0; i < dev_board_size; i++) {
    port_type[i] = '-';

	_multi_spi_reset(dev_board[i].reset);

    reg = multi_spi_read_fxo(dev_board[i].chip_cs);

    if (reg == 0x3) {
      port_type[i] = 'O';
    }
    else {
	  _multi_spi_reset(dev_board[i].reset);
	  reg = multi_spi_read_fxs(dev_board[i].chip_cs, 0);
      if (reg == 0x5 ) {
		port_type[i] = 'S';
      }
      else
      {
		 _multi_spi_reset(dev_board[i].reset);

         reg = multi_spi_read_fxs(dev_board[i].chip_cs, 0x1);

         if ( ( reg & 0xC0) == 0x80 )
         {
            port_type[i] = 'S';
         }
      }
    }

  }

}

EXPORT_SYMBOL( multi_spi_init );
EXPORT_SYMBOL( multi_spi_board_size );
EXPORT_SYMBOL( multi_spi_reset );
EXPORT_SYMBOL( multi_spi_auto_detect );
EXPORT_SYMBOL( multi_spi_write_8_bits );
EXPORT_SYMBOL( multi_spi_read_8_bits );

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pawel Pastuszak <pawelpastuszak@gmail.com>");



#endif
