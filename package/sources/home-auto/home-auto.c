/*
 * Written by Dimitar Penev <dpn@switchfin.org>
 *
 * Copyright (C) 2010 Switchfin Org.
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
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>


//ioctl commands 
enum commands {WLTC_GET_ID=10,          //Host send this command if wnats to read the ID of the WLTC,
                                        //so the host CPU can detect it
               WLTC_GET_ADDRESS_LIST,   //HOst request the list of the WLRR devices available 
               WLTC_SET_DEVICE,         //Host will send command to switch on / off of the certain WLRR 
               WLTC_GET_DEVICE,         //Host request status of given WLRR
              };
#define MAX_WLRR_NUM    10
int main(int argc, char *argv[])
{
	int fd, res, i, j, data, addr1[9];
	unsigned char addr[9], address_list[8*MAX_WLRR_NUM+1];
	char a[]="fe:00";
	//Those variables keeps
	//	addr = a7 a6 .. a0 		- in case of get_device
	//	addr = a7 a6 .. a0 on_off	- in case of set_device
	//	address_list = WLRRnum a[0][7] a[0][6] .. a[0][0] a[1][7] ..  
	
	if (argc < 2) goto help;

	fd = open(argv[1], O_RDWR);
	if (fd < 0) {
		fprintf(stderr, "Unable to open %s: %s\n", argv[1], strerror(errno));
		exit(1);
	}
	
	if (!strcasecmp(argv[2], "set_device")){
	
		if (argc < 4) goto help;
		//Parse the address
		if (!sscanf(argv[3], "%x:%x:%x:%x:%x:%x:%x:%x", addr1+7, addr1+6, addr1+5, addr1+4, addr1+3, addr1+2, addr1+1, addr1)) {

			fprintf(stdout, "Wrong address\n");
			goto help;
						
		}
		addr[0]=addr1[0];addr[1]=addr1[1];addr[2]=addr1[2];addr[3]=addr1[3];
		addr[4]=addr1[4];addr[5]=addr1[5];addr[6]=addr1[6];addr[7]=addr1[7];
		
		addr[8]=atoi(argv[4]);
                res=ioctl(fd, WLTC_SET_DEVICE, (long)addr);
		if(!res)
                	fprintf(stdout, "OK\n"); 
		else
			fprintf(stdout, "FAIL\n");
	} else if (!strcasecmp(argv[2], "get_device")){

                if (argc < 3) goto help;
                //Parse the address
                if (!sscanf(argv[3], "%x:%x:%x:%x:%x:%x:%x:%x", addr1+7, addr1+6, addr1+5, addr1+4, addr1+3, addr1+2, addr1+1, addr1
)) {

                        fprintf(stdout, "Wrong address\n");
                        goto help;

                }
                addr[0]=addr1[0];addr[1]=addr1[1];addr[2]=addr1[2];addr[3]=addr1[3];
                addr[4]=addr1[4];addr[5]=addr1[5];addr[6]=addr1[6];addr[7]=addr1[7];

                res=ioctl(fd, WLTC_GET_DEVICE, (long)addr);
                if(!res)
                        fprintf(stdout, "%d\n", *(unsigned char *)addr);
                else
                        fprintf(stdout, "FAIL\n");

	} else if (!strcasecmp(argv[2], "get_address_list")){
		//in address_list we get the WLRR count following with 8 bytes address of each WLRR device
 
		if (argc < 3) goto help;
		
		res=ioctl(fd, WLTC_GET_ADDRESS_LIST, (long)address_list);
		if(!res){
			if(!address_list[0])  fprintf(stdout, "No WLRR devices available\n");

			for(i=0;i<address_list[0]; i++)
				fprintf(stdout, "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n", address_list[i*8+8], address_list[i*8+7],address_list[i*8+6],address_list[i*8+5],address_list[i*8+4],address_list[i*8+3],address_list[i*8+2],address_list[i*8+1]);					
		}
                else
                        fprintf(stdout, "FAIL\n");
	} else	
		fprintf(stderr, "Invalid command\n");
	
	close(fd);
	return 0;

help:
	fprintf(stderr, "Usage: home-auto <wltc device> <cmd> [<addr>] [<data>]\n"
			"       <wltc device> is a char device exported by the wltc driver for\n"
                        "       comunication with WLTC hardware, usually /dev/wltc\n\n"
                        "       <cmd> is one of:\n"
			"	     get_address_list -get the list with mac addresses of the WLRR\n"
			"			       devices available in the netweok\n"
			"			       <addr> and <data> are not used\n"	  	
                        "            set_device - sends the <data> to the remote home automation device with address <addr>.\n"
			"                  	  For example WLRR uses the LSB of <data> to control the appliance\n"
                        "            get_device - gets the status of the remote home automation device with address <addr>\n"
			"		          For example WLRR reports the status of its relay\n"
			"	<addr> is the MAC address of the reffered wireless device. See examples.\n"
			"	       to get the list of available addresses use get_address_list command.\n"		
			"	<data> should fit in 1 byte. For WLRR only Least Significant Bit (LSB) is used)\n\n"
			"Example:\n\n"
                        "Get the list of available  addresses\n"
                        "       home-auto /dev/wltc get_address_list\n\n"
			"Switch on the Appliance\n"
			"	home-auto /dev/wltc set_device 55:44:33:fe:ff:22:00:02 1\n\n"
                        "Switch off the Appliance\n"
                        "       home-auto /dev/wltc get_device 55:44:33:fe:ff:22:00:02\n");
        exit(1);

}
