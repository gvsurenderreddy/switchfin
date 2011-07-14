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
#define WLTC_GET        10
#define WLTC_SET        11

int main(int argc, char *argv[])
{
	int fd, res, cmd;

	if (argc < 4) goto help;

	fd = open(argv[1], O_RDWR);
	if (fd < 0) {
		fprintf(stderr, "Unable to open %s: %s\n", argv[1], strerror(errno));
		exit(1);
	}
	
	if (!strcasecmp(argv[2], "set")){
		cmd=atoi(argv[3]);
                res=ioctl(fd, WLTC_SET, (long)&cmd);
		if(!res)
                	fprintf(stdout, "OK\n"); 
		else
			fprintf(stdout, "TIME OUT\n");

	} else
		fprintf(stderr, "Invalid command\n");
	
	close(fd);
	return 0;

help:
	fprintf(stderr, "Usage: home-auto <wltc device> <cmd> <data>\n"
			"       <wltc device> is a char device exported by the wltc driver for\n"
                        "       comunication with WLTC hardware, usually /dev/wltc\n\n"
                        "       <cmd> is one of:\n"
                        "            set - sends the data to the remote home automation device.\n"
			"                  For example WLRR uses the LSB of data to control the appliance\n"
//                        "            get - gets the status of the remote home automation device\n"
//			"		   For example WLRR reports the status of the appliance\n"
			"	<data> should fit in 1 byte. For WLRR only Least Significant Bit (LSB) is used)\n\n"
			"Example:\n\n"
			"Switch on the Appliance\n"
			"	home-auto /dev/wltc set 1\n\n"
                        "Switch off the Appliance\n"
                        "       home-auto /dev/wltc set 0\n");
        exit(1);

}
