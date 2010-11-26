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
#include <dahdi/user.h>

#include "dahdi_tools_version.h"


#define WCGSM_GET_CALLERID    10
#define WCGSM_GET_SMS_COUNT   11
#define WCGSM_GET_SMS         12
#define WCGSM_SEND_SMS        13


int main(int argc, char *argv[])
{
	int fd;
	int res;
	int sms_count;
	char sms[250];

	if (argc < 3) goto help;

	fd = open(argv[1], O_RDWR);
	if (fd < 0) {
		fprintf(stderr, "Unable to open %s: %s\n", argv[1], strerror(errno));
		exit(1);
	}
	
	if (!strcasecmp(argv[2], "smscount")){

                sms_count=ioctl(fd, WCGSM_GET_SMS_COUNT, (long)sms);
                fprintf(stdout, "%d\n", sms_count);
                return 0;
        } else if (!strcasecmp(argv[2], "smsget")){
		//One typical SMS read comand may return
		//at+cmgr=1,1
		//+CMGR: "REC UNREAD","+359878583013",,"07/11/28,16:07:34+00"
		//Test SMS message 
		//OK 
                res=ioctl(fd, WCGSM_GET_SMS, (long)sms);
		if (!res) {
                	sms[strlen(sms)-2]=0;           //Remove trailing OK
                	fprintf(stdout, "%s\n", sms+9); //Don't print '+CMGR: '
                }
                return 0;
	}else if (!strcasecmp(argv[2], "smssend")){
      		//We will send the information to the kernel in the following format
		//number:text
		if (argc < 5) goto help;
		sms[0]=0;
		strcat(sms,argv[3]);	//put the number
		strcat(sms,":");
		strcat(sms,argv[4]);	//put the text
		res=ioctl(fd, WCGSM_SEND_SMS, (long)sms);
                if (!res) {
			fprintf(stdout, "SMS Sent!\n");
                	return 0;
		}
		else {
			fprintf(stderr, "Sending Failed!\n");
                        exit(1);
		}
	} else
		fprintf(stderr, "Invalid command\n");
	
	close(fd);
	return 0;

help:
	fprintf(stderr, "Usage: gsm-smstest <wcgsm device> <cmd> <number> <text>\n"
			"       <wcgsm> is a char device exported by the wcfxs driver for SMS\n"
                        "       comunication with GSM1 hardware, usually /dev/dahdi/wcgsm\n\n"
                        "       <cmd> is one of:\n"
                        "            smscount - gets the number of received messages available\n"
                        "            smsget   - gets one sms from the received queue, sender\n"
                        "                       and time infomation is also included\n"
                        "            smssend  - sends <text> as SMS to the number <number>\n");
        exit(1);

}
